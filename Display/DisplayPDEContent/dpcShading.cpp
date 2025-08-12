//
// Copyright (c) 2008-2025, Datalogics, Inc. All rights reserved.
//

#include <string>

#include "ASCalls.h"
#include "CosCalls.h"
#include "PDCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"

#include "DisplayPDEContent.h"
#include "dpcOutput.h"

static ASBool PdeClipenumProc(PDEElement Element, void *clientData) {
    PDEGraphicState GState;
    ASBool HasGState = PDEElementHasGState(Element, &GState, sizeof(PDEGraphicState));

    ASFixedMatrix fm1;
    ASDoubleMatrix matrix;
    PDEElementGetMatrix(Element, &fm1);
    if ((fixedZero == fm1.a) && (fixedZero == fm1.b) && (fixedZero == fm1.c) &&
        (fixedZero == fm1.d) && (fixedZero == fm1.h) && (fixedZero == fm1.v)) {
        // Invert zero matrix --> identity matrix
        matrix = { 1,0,0,1,0,0 };
    } else {
        PDEElementGetMatrixEx(Element, &matrix);
    }

    PDEType Type = static_cast<PDEType>(PDEObjectGetType((PDEObject)Element));
    switch (Type) {
    case kPDEText:
        // Display a complete text object
        DisplayText(reinterpret_cast<PDEText>(Element), &matrix, &GState, HasGState);
        break;

    case kPDEPath:
        // Display a complete path
        DisplayPath(reinterpret_cast<PDEPath>(Element), &matrix, &GState, HasGState);
        break;
    default:
        Outputter::Inst()->GetOfs() << "******* Unknown Clip Element. Type " << Type << std::endl;
        break;
    }

    return true;
}

static void DisplayClip(PDEClip Clip) {
    ASInt32 nElems = PDEClipGetNumElems(Clip);
    if (nElems) {
        Outputter::Inst()->GetOfs() << "Begin Clip:\n";
        Outputter::Inst()->GetOfs() << "{\n";
        Outputter::Inst()->Indent();
        PDEClipFlattenedEnumElems(Clip, reinterpret_cast<PDEClipEnumProc>(PdeClipenumProc), NULL);
        Outputter::Inst()->Outdent();
        Outputter::Inst()->GetOfs() << "} End of Clip\n";
    }
}

void DisplayShading(PDEShading Shading, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState) {
    Outputter::Inst()->GetOfs() << "Shading op: At " << DisplayMatrix(Matrix).c_str() << std::endl;
    Outputter::Inst()->Indent();

    CosObj cobj;
    PDEShadingGetCosObj(Shading, &cobj);

    if (CosObjGetType(cobj) == CosDict) {
        Outputter::Inst()->GetOfs() << "Shading Dictionary: " << DisplayCosDict(cobj) << std::endl;
    }

    if (HasGState)
        DisplayGraphicState(GState);

    PDEClip clip = PDEElementGetClip(reinterpret_cast<PDEElement>(Shading));
    if (clip)
        DisplayClip(clip);

    Outputter::Inst()->Indent();
}
