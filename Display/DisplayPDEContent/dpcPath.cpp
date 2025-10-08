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

void DisplayPath(PDEPath Path, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState) {
    PDEPathOpFlags PaintOp = static_cast<PDEPathOpFlags>(PDEPathGetPaintOp(Path));
    ASInt32 PathSize = PDEPathGetDataFloat(Path, NULL, 0);
    ASFloat *PathData = static_cast<ASFloat *>(ASmalloc(PathSize + 24)); // Allow for me to read ahead, without crashing
    ASInt32 DataSize = PathSize / 4;
    PDEPathGetDataFloat(Path, PathData, PathSize);

    std::ostringstream oss;

    switch (PaintOp) {
    case kPDEInvisible:
        oss << "Invisible";
        break;
    case kPDEStroke:
        oss << "Stroke";
        break;
    case kPDEFill:
        oss << "Fill";
        break;
    case kPDEEoFill:
        oss << "EOFill";
        break;
    case (kPDEStroke | kPDEFill):
        oss << "Stroke and Fill";
        break;
    case (kPDEStroke | kPDEEoFill):
        oss << "Stroke and EOFill";
        break;
    }
    Outputter::Inst()->GetOfs() << "Path Object: " << oss.str().c_str() << " At "
                                << DisplayMatrix(Matrix).c_str() << std::endl;
    DisplayGraphicState(GState);
    Outputter::Inst()->GetOfs() << "{ Operators\n";
    Outputter::Inst()->Indent();

    ASInt32 Index = 0;
    while (1) {
        oss.str("");
        PDEPathElementType Operator;
        ASDoublePoint One = { 0,0 }, Two = { 0,0 },Three = { 0,0 };
        ASInt32 Increment;

        if (Index >= DataSize)
            break;

        Operator = static_cast<PDEPathElementType>(PathData[Index]);
        One.h = static_cast<ASDouble>(PathData[Index + 1]);
        One.v = static_cast<ASDouble>(PathData[Index + 2]);
        Two.h = static_cast<ASDouble>(PathData[Index + 3]);
        Two.v = static_cast<ASDouble>(PathData[Index + 4]);
        Three.h = static_cast<ASDouble>(PathData[Index + 5]);
        Three.v = static_cast<ASDouble>(PathData[Index + 6]);

        switch (Operator) {
        case kPDEMoveTo:
            oss << "MoveTo " << AppendPoint(&One, Matrix, false).c_str();
            Increment = 3;
            break;
        case kPDELineTo:
            oss << "LineTo " << AppendPoint(&One, Matrix, false).c_str();
            Increment = 3;
            break;
        case kPDECurveTo:
            oss << "CurveTo " << AppendPoint(&One, Matrix, true).c_str()
                << AppendPoint(&Two, Matrix, true).c_str() << AppendPoint(&Three, Matrix, false).c_str();
            Increment = 7;
            break;
        case kPDECurveToV:
            oss << "CurveToV " << AppendPoint(&One, Matrix, true).c_str()
                << AppendPoint(&Two, Matrix, false).c_str();
            Increment = 5;
            break;
        case kPDECurveToY:
            oss << "CurveToY " << AppendPoint(&One, Matrix, true).c_str()
                << AppendPoint(&Two, Matrix, false).c_str();
            Increment = 5;
            break;
        case kPDERect:
            oss << "Rectangle at  " << AppendPoint(&One, Matrix, true)
                << "Width: " << Two.h << " Height: " << Two.v;
            Increment = 5;
            break;
        case kPDEClosePath:
            oss << "Close Path";
            Increment = 1;
            break;
        default:
            oss << "Invalid Path Operator";
            Increment = 1;
            break;
        }
        Outputter::Inst()->GetOfs() << oss.str().c_str() << std::endl;
        Index += Increment;
    }
    ASfree(PathData);
    Outputter::Inst()->Outdent();
    Outputter::Inst()->GetOfs() << "} End Path Content\n";
}
