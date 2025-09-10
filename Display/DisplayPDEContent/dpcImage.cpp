//
// Copyright (c) 2008-2025, Datalogics, Inc. All rights reserved.
//

#include <sstream>

#include "ASCalls.h"
#include "CosCalls.h"
#include "PDCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"

#include "DisplayPDEContent.h"
#include "dpcOutput.h"

void DisplayImage(PDEImage Image, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState) {
    PDEImageAttrs Attributes;
    PDEColorSpace ColorSpace = PDEImageGetColorSpace(Image);
    ASInt32 ImageDataSize = PDEImageGetDataLen(Image);
    ASInt32 ImageRequiredSize;

    PDEImageGetAttrs(Image, &Attributes, sizeof(Attributes));

    Outputter::Inst()->GetOfs() << "Image Object: at " << DisplayMatrix(Matrix) << std::endl;
    Outputter::Inst()->Indent();

    ASBool IsExternal(Attributes.flags & kPDEImageExternal);
    ASBool MaskedByPosition(Attributes.flags & kPDEImageMaskedByPosition);
    ASBool MaskedByColor(Attributes.flags & kPDEImageMaskedByColor);

    ASBool IsMask(Attributes.flags & kPDEImageIsMask);
    ASInt32 Comps = PDEColorSpaceGetNumComps(IsMask ? GState->fillColorSpec.space : ColorSpace);

    std::string sRI;
    if (Attributes.intent != ASAtomNull) {
        sRI = ", RenderingIntent: ";
        sRI += ASAtomGetString(Attributes.intent);
    }
    Outputter::Inst()->GetOfs() << (IsMask ? "ImageMask" : "Image") << " of " << Attributes.width
                                << " bits by " << Attributes.height << " bits, "
                                << Attributes.bitsPerComponent << " bits per "
                                << "component, " << Comps << " components" << sRI.c_str() << std::endl;

    /* Round the row size up to even 8 bit words */
    ImageRequiredSize = (((Attributes.width * Comps * Attributes.bitsPerComponent) + 7) / 8);
    ImageRequiredSize *= Attributes.height;

    ASInt32 NumberOfFilters = PDEImageGetFilterArray(Image, NULL);
    PDEFilterArray *Filters =
        static_cast<PDEFilterArray *>(ASmalloc(sizeof(PDEFilterArray) + (NumberOfFilters * sizeof(PDEFilterSpec))));
    NumberOfFilters = PDEImageGetFilterArray(Image, Filters);
    std::ostringstream ossFilter;
    if (NumberOfFilters > 1)
        ossFilter << "[";
    for (ASInt32 Index = 0; Index < NumberOfFilters; Index++) {
        ossFilter << ASAtomGetString(Filters->spec[Index].name) << (Index + 1 < NumberOfFilters ? ", " : "");
    }
    if (NumberOfFilters > 1)
        ossFilter << "]";

    std::string sInLine("Inline");
    if (IsExternal) {
        CosObj ImageObj;
        CosObj Length;
        PDEImageGetCosObj(Image, &ImageObj);
        Length = CosDictGet(ImageObj, ASAtomFromString("Length"));
        if (CosObjGetType(Length) == CosInteger)
            ImageDataSize = CosIntegerValue(Length);
        sInLine = "External";
    }

    Outputter::Inst()->GetOfs() << sInLine.c_str() << " Image requires " << ImageRequiredSize
                                << " bytes and is compressed to " << ImageDataSize << "("
                                << ((1.0 - ((ImageDataSize * 1.0) / (ImageRequiredSize * 1.0))) * 100.0)
                                << "% using " << ossFilter.str().c_str() << ')' << std::endl;

    if (PDEImageHasSMask(Image)) {
        Outputter::Inst()->GetOfs() << "Image has a soft Mask\n";
    }

    if (MaskedByPosition) {
        Outputter::Inst()->GetOfs() << "Image is masked by Position\n";
    }

    if (MaskedByColor) {
        Outputter::Inst()->GetOfs() << "Image is masked by Color\n";
    }

    Outputter::Inst()->Outdent();
    ASfree(Filters);
}
