//
// Copyright (c) 2010-2025, Datalogics, Inc. All rights reserved.
//
//

CosObj Uncompress(CosObj Stream);
CosObj UncompressAnnot(CosObj Annot);
ASBool CompressXobj(CosObj Key, CosObj Value, void *Data);
ASBool UncompressResource(CosObj Key, CosObj Value, void *clientData);
ASBool SubAnnot(CosObj Key, CosObj Value, void *Data);
