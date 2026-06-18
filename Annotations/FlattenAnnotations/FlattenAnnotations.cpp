//
// Copyright (c) 2017-2025, Datalogics, Inc. All rights reserved.
//
//
// This sample demonstrates flattening annotations within a PDF document.
//
// Command-line:  <input-file>  <output-file>       (Both optional)
//

#include <algorithm>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PEWCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "CosCalls.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "FlattenAnnotations.pdf"
#define DEF_OUTPUT "FlattenAnnotations-out.pdf"

static CosObj FindAppearanceResourceEntry(CosObj strm, PDPage page);
static CosObj FindAnnotAppearanceStream(CosObj annotCos);

int main(int argc, char **argv) {
    APDFLib libInit;
    ASErrorCode errCode = 0;
    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);

    DURING

        APDFLDoc doc(csInputFileName.c_str(), true);

        PDPage page = doc.getPage(0);
        PDEContent pageContent = PDPageAcquirePDEContent(page, 0);

        // Step 1) Convert each Annotation into a Form Xobject, and remove the annotation.

        ASInt32 nAnnotations = PDPageGetNumAnnots(page);

        std::cout << "Flattening " << nAnnotations << " from " << csInputFileName.c_str()
                  << " and writing to " << csOutputFileName.c_str() << std::endl;

        // We need to iterate through the annotations in "reverse" order, since PDPageRemoveAnnot
        //    updates the array of annotations with each removal.
        for (ASInt32 i = nAnnotations - 1; i >= 0; --i) {
            // Get the next annotation.
            PDAnnot next = PDPageGetAnnot(page, i);
            CosObj annotCos = PDAnnotGetCosObj(next);

            CosObj appearanceStrm = FindAnnotAppearanceStream(annotCos);
            CosObj resource = FindAppearanceResourceEntry(appearanceStrm, page);

            if (CosObjGetType(resource) != CosNull) {
                // Place the annotation's resources in the page's content
                ASFixedRect rect;
                PDAnnotGetRect(next, &rect);

                // The placement matrix maps the appearance to the annotation's rectangle.
                ASDoubleMatrix placement;
                placement.a = placement.d = 1.0;
                placement.b = placement.c = 0.0;
                placement.h = placement.v = 0.0;

                double leftValue = 0;
                double bottomValue = 0;
                double rightValue = 0;
                double topValue = 0;
                CosObj bbox = CosNewNull();
                if (CosDictKnown(appearanceStrm, ASAtomFromString("BBox"))) {
                    bbox = CosDictGet(appearanceStrm, ASAtomFromString("BBox"));
                    if (CosObjGetType(bbox) == CosArray) {
                        CosObj left = CosArrayGet(bbox, 0);
                        leftValue = CosDoubleValue(left);
                        CosObj bottom = CosArrayGet(bbox, 1);
                        bottomValue = CosDoubleValue(bottom);
                        CosObj right = CosArrayGet(bbox, 2);
                        rightValue = CosDoubleValue(right);
                        CosObj top = CosArrayGet(bbox, 3);
                        topValue = CosDoubleValue(top);
                    }
                }

                // The appearance stream is a form XObject and may carry its own Matrix entry, which is
                // applied to the appearance before it is positioned (PDF 32000-1 8.10.1 / 12.5.5). The
                // form's Matrix is still applied when the XObject is painted, so the placement matrix we
                // pass here must map the *transformed* bounding box (the BBox after the form Matrix is
                // applied) onto the annotation rectangle. Computing the placement from the raw BBox while
                // ignoring the form Matrix mis-positions appearances whose Matrix is not the identity --
                // for example masking rectangles drawn in page coordinates with a translating Matrix,
                // which would otherwise be relocated and expose the content they were hiding.
                ASDoubleMatrix apMatrix = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
                if (CosDictKnown(appearanceStrm, ASAtomFromString("Matrix"))) {
                    CosObj matrixObj = CosDictGet(appearanceStrm, ASAtomFromString("Matrix"));
                    if (CosObjGetType(matrixObj) == CosArray && CosArrayLength(matrixObj) == 6) {
                        apMatrix.a = CosDoubleValue(CosArrayGet(matrixObj, 0));
                        apMatrix.b = CosDoubleValue(CosArrayGet(matrixObj, 1));
                        apMatrix.c = CosDoubleValue(CosArrayGet(matrixObj, 2));
                        apMatrix.d = CosDoubleValue(CosArrayGet(matrixObj, 3));
                        apMatrix.h = CosDoubleValue(CosArrayGet(matrixObj, 4));
                        apMatrix.v = CosDoubleValue(CosArrayGet(matrixObj, 5));
                    }
                }

                // Transform the four BBox corners by the form Matrix and take the axis-aligned bounds,
                // giving the appearance box in the space the placement matrix must map from.
                double corners[4][2] = {{leftValue, bottomValue},
                                        {rightValue, bottomValue},
                                        {rightValue, topValue},
                                        {leftValue, topValue}};
                double transformedLeft = 0, transformedBottom = 0, transformedRight = 0, transformedTop = 0;
                for (int c = 0; c < 4; ++c) {
                    double tx = apMatrix.a * corners[c][0] + apMatrix.c * corners[c][1] + apMatrix.h;
                    double ty = apMatrix.b * corners[c][0] + apMatrix.d * corners[c][1] + apMatrix.v;
                    if (c == 0) {
                        transformedLeft = transformedRight = tx;
                        transformedBottom = transformedTop = ty;
                    } else {
                        transformedLeft = (std::min)(transformedLeft, tx);
                        transformedRight = (std::max)(transformedRight, tx);
                        transformedBottom = (std::min)(transformedBottom, ty);
                        transformedTop = (std::max)(transformedTop, ty);
                    }
                }

                double transformedWidth = transformedRight - transformedLeft;
                double transformedHeight = transformedTop - transformedBottom;
                if (transformedWidth != 0.0 && transformedHeight != 0.0) {
                    placement.a = ASFixedToFloat(rect.right - rect.left) / transformedWidth;
                    placement.d = ASFixedToFloat(rect.top - rect.bottom) / transformedHeight;
                    placement.h = ASFixedToFloat(rect.left) - transformedLeft * placement.a;
                    placement.v = ASFixedToFloat(rect.bottom) - transformedBottom * placement.d;
                }

                // Create and add the form xobject.
                PDEForm formXObject = PDEFormCreateFromCosObjEx(&appearanceStrm, &resource, &placement);
                PDEContentAddElem(pageContent, kPDEAfterLast, (PDEElement)formXObject);

                PDERelease((PDEObject)formXObject);
            } else {
                // This annotation has no appearance.
                std::cout << "Warning: The " << i << "th annotation, a "
                          << ASAtomGetString(PDAnnotGetSubtype(next))
                          << ", has no contained or inherited resources entry, so has no "
                             "appearance. "
                          << "It will still be removed." << std::endl;
            }

            PDPageRemoveAnnot(page, i);
        }
        PDPageSetPDEContentCanRaise(page, 0);

        // Step 2) Save and close.

        // Release resources.
        PDPageReleasePDEContent(page, 0);
        PDPageRelease(page);

        doc.saveDoc(csOutputFileName.c_str());

    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode;
}

// Try to find an annotation's appearance stream, which may contain the resource's CosObj
//    that we will need to create the PDEForm of its appearance.
//
CosObj FindAnnotAppearanceStream(CosObj annotCos) {
    CosObj retObj = CosNewNull();
    // The appearance dictionary of our annotation.
    if (CosDictKnownKeyString(annotCos, "AP")) {
        // The appearance dictionary of this annotation
        CosObj APDict = CosDictGetKeyString(annotCos, "AP");
        if (CosDictKnownKeyString(APDict, "N")) {
            // The normal appearance of our annotation.
            CosObj normal = CosDictGetKeyString(APDict, "N");

            // The normal appearance is either a stream or a dictionary. If the appearance is a dictionary,
            // we will need to get the appearance stream from the appearance state ("AS").
            if (CosObjGetType(normal) == CosStream) {
                retObj = normal;
            } else {
                if (CosDictKnownKeyString(annotCos, "AS")) {
                    ASAtom appearanceName = CosNameValue(CosDictGetKeyString(annotCos, "AS"));
                    if (CosDictKnown(normal, appearanceName)) {
                        retObj = CosDictGet(normal, appearanceName);
                    }
                }
            }
        }
    }
    return retObj;
}

// If we found an appearance stream, we must find its resources entry. Otherwise the annotation has no appearance.
//
CosObj FindAppearanceResourceEntry(CosObj strm, PDPage page) {
    CosObj retObj = CosNewNull();
    if (CosObjGetType(strm) != CosNull) {
        retObj = CosDictGetKeyString(strm, "Resources");

        // If the appearance stream doesn't have a Resources entry, we must look for an appearance
        // that might have been inherited from a parent page in the page tree.
        if (CosObjGetType(retObj) == CosNull) {
            CosObj pageObj = PDPageGetCosObj(page);
            while (CosObjGetType(retObj) == CosNull) {
                retObj = CosDictGetKeyString(pageObj, "Resources");
                if (CosObjGetType(retObj) == CosNull) {
                    pageObj = CosDictGetKeyString(pageObj, "Parent");
                    if (CosObjGetType(pageObj) == CosNull) {
                        break;
                    }
                } else {
                    break;
                }
            }
        }
    }
    return retObj;
}
