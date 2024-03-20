/*
 * This sample program creates an empty PDF document with one page and imports the specified image file.
 * This file is then converted to a PDEImage, afterwards it is modified with several API's:
 * DLPDEImageRotate(), DLPDEImageTranslate(), DLPDEImageScale(), DLColorConvertPDEImage(),
 * DLPDEImageSetSoftMask() and DLCreateResampledPDEImage().
 * This program also utilizes DLPDEImageGetWidth() and DLPDEImageGetHeight() to assist in translating the PDEImage within the page.
 *
 * Copyright (c) 2024, Datalogics, Inc. All rights reserved.
 *
 */

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "DLExtrasCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "ducky_alpha.tif"
#define DEF_OUTPUT "ImageImport-ducky.pdf"

int main(int argc, char **argv) {
    std::cout << "Initialized the library." << std::endl;
    APDFLib libInit;
    if (libInit.isValid() == false) {
        std::cout << "Initialization failed with code " << libInit.getInitError() << std::endl;
        return libInit.getInitError();
    }
    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    ASPathName sInput = APDFLDoc::makePath(csInputFileName.c_str());
    ASPathName sOutput = APDFLDoc::makePath(csOutputFileName.c_str());

    DURING
        // Create an empty pdf document.
        PDDoc doc = PDDocCreate();

        char message[1024];
        char *iName = ASFileSysDisplayStringFromPath(NULL, sInput);
        char *oName = ASFileSysDisplayStringFromPath(NULL, sOutput);
        sprintf(message, "Reading image file %s and writing to %s", iName, oName);
        std::cout << message << std::endl;
        ASfree((void *)iName);
        ASfree((void *)oName);

        // Convert the input file into a PDEImage.
        PDEImage newImage = DLCreatePDEImageFromFile(doc, sInput, NULL);

        /*
         * Create a PDF page which is one inch larger all around this image.
         * The width and height for the image are carried in the
         * matrix.a and matrix.d fields, respectively.
         * There are 72 PDF user space units in one inch.
         */
        ASDoubleMatrix matrix = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        PDEElementGetMatrixEx((PDEElement)newImage, &matrix);

        ASFixedRect bbox;
        bbox.left = 0;
        bbox.top = FloatToASFixed(matrix.d + 144);
        bbox.right = FloatToASFixed(matrix.a + 144);
        bbox.bottom = 0;

        PDPage page = PDDocCreatePage(doc, PDDocGetNumPages(doc) - 1, bbox);

        // Center the image in its bounding box.
        DLPDEImageTranslate(newImage, 72, 72);

        // Manipulate the image by rotating and scaling it.
        DLPDEImageRotate(newImage, 180);
        DLPDEImageScale(newImage, 0.5, 0.5);

        // Adjust the image location on the page to have more space for the next steps
        DLPDEImageTranslate(newImage, DLPDEImageGetWidth(newImage) * -1, DLPDEImageGetHeight(newImage) * -1.25);

        // Make a clone of the original then manipulate it on the page.
        PDEElement copyElement = PDEElementCopy(reinterpret_cast<PDEElement>(newImage), 0);
        PDEImage clone = reinterpret_cast<PDEImage>(copyElement);

        DLPDEImageTranslate(clone, 72, 72);
        DLPDEImageRotate(clone, 90);
        DLPDEImageScale(clone, 0.5, 0.5);

        // Color convert the image to a grayscale version.
        PDEImage grayscaleImage =
            DLColorConvertPDEImage(&doc, clone, AC_Profile_DotGain30, AC_Saturation, true);
        DLPDEImageTranslate(grayscaleImage, -72, -72);
        DLPDEImageRotate(grayscaleImage, 180);
        DLPDEImageScale(grayscaleImage, 0.5, 0.5);

        // Create a copy of the cloned PDEImage, but with a new DPI.
        DLPDEImageExportParams exportParams = DLPDEImageGetExportParams();
        PDEImage newRezImage = DLCreateResampledPDEImage(clone, &exportParams, 16);
        DLPDEImageTranslate(newRezImage, -172, 72);
        DLPDEImageRotate(newRezImage, 270);

        // Remove the PDEImage's soft mask by passing a nullptr.
        DLPDEImageSetSoftMask(newRezImage, nullptr);

        // Insert the PDEImage into the page.
        PDEContent content = PDPageAcquirePDEContent(page, 0);
        PDEContentAddElem(content, 0, reinterpret_cast<PDEElement>(newImage));
        PDEContentAddElem(content, 1, reinterpret_cast<PDEElement>(clone));
        PDEContentAddElem(content, 2, reinterpret_cast<PDEElement>(newRezImage));
        PDEContentAddElem(content, 3, reinterpret_cast<PDEElement>(grayscaleImage));

        // Set the PDEContent into the page and release it after it is used.
        PDPageSetPDEContentCanRaise(page, 0);
        PDPageReleasePDEContent(page, 0);
        PDPageRelease(page);

        // Save and close the output document.
        PDDocSave(doc, PDSaveFull, sOutput, NULL, NULL, NULL);
        PDDocClose(doc);

        // Release remaining resources when no longer needed.
        PDERelease(reinterpret_cast<PDEObject>(newImage));
        PDERelease(reinterpret_cast<PDEObject>(clone));
        PDERelease(reinterpret_cast<PDEObject>(newRezImage));
        PDERelease(reinterpret_cast<PDEObject>(grayscaleImage));
        ASFileSysReleasePath(NULL, sInput);
        ASFileSysReleasePath(NULL, sOutput);

    HANDLER
        libInit.displayError(ERRORCODE);
        return ERRORCODE;
    END_HANDLER

    return 0;
}

