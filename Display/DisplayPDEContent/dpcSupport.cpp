//
// Copyright (c) 2008-2025, Datalogics, Inc. All rights reserved.
//

#include <sstream>
#include <iostream>
#include <iomanip>

#include "ASCalls.h"
#include "PEWCalls.h"

#include "DisplayPDEContent.h"

std::string DisplayQuad(ASFixedQuad *Quad) {
    char LLX[20], LLY[20], ULX[20], ULY[20];
    char LRX[20], LRY[20], URX[20], URY[20];

    ASFixedToCString(Quad->bl.h, LLX, 20, 5);
    ASFixedToCString(Quad->bl.v, LLY, 20, 5);
    ASFixedToCString(Quad->tl.h, ULX, 20, 5);
    ASFixedToCString(Quad->tl.v, ULY, 20, 5);
    ASFixedToCString(Quad->br.h, LRX, 20, 5);
    ASFixedToCString(Quad->br.v, LRY, 20, 5);
    ASFixedToCString(Quad->tr.h, URX, 20, 5);
    ASFixedToCString(Quad->tr.v, URY, 20, 5);
    std::ostringstream oss;
    oss << "[LL(" << LLX << ',' << LLY << "), UL(" << ULX << ',' << ULY << "), LR(" << LRX << ','
        << LRY << "), UR(" << URX << ',' << URY << ")]";
    return oss.str();
}

std::string DisplayFixedMatrix(ASFixedMatrix *Matrix) {
    char a[20], b[20], c[20], d[20], h[20], v[20];

    ASFixedToCString(Matrix->a, a, 20, 5);
    ASFixedToCString(Matrix->b, b, 20, 5);
    ASFixedToCString(Matrix->c, c, 20, 5);
    ASFixedToCString(Matrix->d, d, 20, 5);
    ASFixedToCString(Matrix->h, h, 20, 5);
    ASFixedToCString(Matrix->v, v, 20, 5);
    std::ostringstream oss;
   
    oss <<  "[" << a << ", " << b << ", " << c << ", " << d << ", " << h << ", " << v << "]";
    return oss.str();
}

std::string DisplayMatrix(ASDoubleMatrix* Matrix) {
    std::ostringstream oss;
    oss.precision(4);
    oss << "[" << Matrix->a << ", " << Matrix->b << ", " << Matrix->c << ", " << Matrix->d << ", " << Matrix->h << ", " << Matrix->v << "]";
    return oss.str();
}


std::string DisplayFixed(ASFixed *Fixed) {
    char LocalText[120];
    ASFixedToCString(*Fixed, LocalText, 20, 5);
    return std::string(LocalText);
}

std::string AppendPoint(ASDoublePoint *Point, ASDoubleMatrix *Matrix, ASBool MoreToCome) {
    ASDoublePoint Local;
    if (Matrix)
        ASDoubleMatrixTransform(&Local, Matrix, Point);
    else {
        Local.h = Point->h;
        Local.v = Point->v;
    }

    std::ostringstream oss;
    oss.precision(4);
    oss << "(" << Local.h << ", " << Local.v << ")" << (MoreToCome ? ", " : "");
    return oss.str();
}
