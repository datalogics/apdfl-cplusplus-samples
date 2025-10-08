//
// Copyright (c) 2025, Datalogics, Inc. All rights reserved.
//
// This sample adds a timestamp signature to a PDF document.
// Signature type: RFC3161/TimeStamp
// Credential data format: na
// Credential storage format: na
// Signature Algorithm: RSA
//

#include <iostream>
#include <string>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "PERCalls.h"

// Header that includes Digital Signature methods
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "CreateAcroForm2h.pdf"
#define DEF_OUTPUT "AddDigitalSignatureRFC3161-out.pdf"

int main(int argc, char** argv) {
    APDFLib lib;

    ASErrorCode errCode = 0;
    if (lib.isValid() == false) {
        errCode = lib.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return errCode;
    }

    std::string const csInputFileName(argc > 1 ? argv[1] : INPUT_LOC DEF_INPUT);
    std::string const csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::cout << "Will apply a digital signature to " << csInputFileName.c_str() <<
        " and save as " << csOutputFileName.c_str() << std::endl;

    DURING

        // Open the input document.
        APDFLDoc APDoc(csInputFileName.c_str(), true);
        PDDoc inDoc = APDoc.getPDDoc();

        // Setup Sign params
        PDSignDocSignParams const signParams = PDSignDocSignInitParams();

        // Set the signature type to be used. The available types are defined in the SignatureType enum. Default CMS.
        // If ever unsure of the signature type applied, use PDSignDocGetDocSignType(signParams);
        PDSignDocSetDocSignType(signParams, RFC3161);

        // Search for first unsigned field in document
        PDSignDocSetFieldID(signParams, SearchForFirstUnsignedField);

        // Set credential related attributes
        PDSignDocSetDigestCategory(signParams, sha256);

        // Setup Save params
        PDSignDocSaveParams const saveParams{ PDSignDocSaveInitParams() };

        ASPathName outPathName = APDFLDoc::makePath(csOutputFileName.c_str());
        // Delete output file if present and ASFile objects for outPathName are closed
        (void)ASFileSysRemoveFile(nullptr, outPathName);

        PDSignDocSetOutputPath(saveParams, outPathName);

        // Finally, sign the document
        PDSignDocWithParams(inDoc, saveParams, signParams);

        PDDocClose(inDoc);
        ASFileSysReleasePath(nullptr, outPathName);

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);
    END_HANDLER
        return errCode;
};
