//
// Copyright (c) 2008-2025, Datalogics, Inc. All rights reserved.
//

std::string DisplayFixed(ASFixed *Fixed);
std::string AppendPoint(ASDoublePoint *Point, ASDoubleMatrix *Matrix, ASBool MoreToCome);
std::string DisplayColor(PDEColorSpec *);
std::string DisplayCosDict(CosObj cobj);
std::string DisplayQuad(ASFixedQuad *Quad);
std::string DisplayFixedMatrix(ASFixedMatrix *Matrix);
std::string DisplayMatrix(ASDoubleMatrix* Matrix);
void DisplayGraphicState(PDEGraphicState *GState);
void DisplayText(PDEText Text, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState);
void DisplayPath(PDEPath Path, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState);
void DisplayImage(PDEImage Image, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState);
void AnalyzePDEContent(PDEContent Content, ASDoubleMatrix *Matrix);

void DisplayShading(PDEShading Shading, ASDoubleMatrix *Matrix, PDEGraphicState *GState, ASBool HasGState);
