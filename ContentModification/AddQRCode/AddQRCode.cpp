//
// Copyright (c) 2025, Datalogics, Inc. All rights reserved.
//
// This sample shows how to add a QR barcode to a PDF page
//
// Command line:  <text-to-encode> <input-file> <output-file>   (all optional)

#include "APDFLDoc.h"
#include "InitializeLibrary.h"

#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "ASCalls.h"
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "AddContent.pdf"
#define DEF_OUTPUT "AddQRCode-out.pdf"
#define DEF_ENCODED_TEXT "Datalogics"

int main(int argc, char **argv) {
    ASErrorCode errCode = 0;

    // Initialize the Adobe PDF Library.
    APDFLib lib;
    if (lib.isValid() == false) {
        errCode = lib.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return errCode;
    }

    std::string csEncodedText(argc > 1 ? argv[1] : DEF_ENCODED_TEXT);
    std::string csInputFileName(argc > 2 ? argv[2] : INPUT_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 3 ? argv[3] : DEF_OUTPUT);

    DURING
        APDFLDoc APDoc(csInputFileName.c_str(), true);
        PDDoc inDoc = APDoc.getPDDoc();

        ASFixedRect cropBox;
        memset(&cropBox, 0, sizeof(ASFixedRect));

        PDPage inPage = PDDocAcquirePage(inDoc, 0);

        PDPageGetCropBox(inPage, &cropBox);

        ASUTF16Val *ucs = (ASUTF16Val*)csEncodedText.c_str();

        ASText text = ASTextFromUnicode(ucs, kUTF8);

        PDPageAddQRBarcode(inPage, text, 72.0, ASFixedToFloat(cropBox.top) - 1.5 * 72.0, 72.0, 72.0);

        APDoc.saveDoc(csOutputFileName.c_str());

        ASTextDestroy(text);
        PDPageRelease(inPage);
    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);
    END_HANDLER

    return errCode;
};
