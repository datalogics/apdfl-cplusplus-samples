//
// Copyright (c) 2026, Datalogics, Inc. All rights reserved.
//
// This sample demonstrates adding a PAdES (PDF Advanced Electronic Signatures)
// signature with an explicit signature policy to a PDF document. PAdES policy
// signatures (EPES) conform to the ETSI EN 319 142 standard, use the
// ETSI.CAdES.detached SubFilter, and embed one or more signature policy
// identifiers along with optional policy qualifiers.
//
// Command-line:  <input-file> <output-file> <logo file>   (All optional)
//

#include <iostream>
#include <string>
#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "PERCalls.h"

// Header that includes Digital Signature methods
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "SixPages.pdf"
#define DEF_OUTPUT "PAdESPolicySignature-out.pdf"
#define DEF_LOGO_FILE "ducky_alpha.tif"

// DEF_CERT_FILE points to the signer certificate in PEM (base-64 encoded) format.
// PAdES signatures use EC (Elliptic Curve) credentials with the secp521r1 curve.
#define DEF_CERT_FILE "Credentials/PEM/ecSecP521r1Cert.pem"

// DEF_KEY_FILE points to the private key corresponding to the signer certificate defined above.
#define DEF_KEY_FILE "Credentials/PEM/ecSecP521r1Key.pem"

int main(int argc, char **argv) {
    APDFLib lib;

    ASErrorCode errCode = 0;
    if (lib.isValid() == false) {
        errCode = lib.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return errCode;
    }

    std::string const csInputFileName(argc > 1 ? argv[1] : INPUT_LOC DEF_INPUT);
    std::string const csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::string const csLogoFileName(argc > 3 ? argv[3] : INPUT_LOC DEF_LOGO_FILE);
    std::cout << "Will apply a PAdES policy signature to " << csInputFileName.c_str()
              << " with a logo " << csLogoFileName.c_str()
              << " and save as " << csOutputFileName.c_str() << std::endl;

    DURING

        // Open the input document.
        APDFLDoc APDoc(csInputFileName.c_str(), true);
        PDDoc inDoc = APDoc.getPDDoc();

        // Setup Sign params
        PDSignDocSignParams const signParams = PDSignDocSignInitParams();

        PDSignDocSetFieldID(signParams, CreateFieldWithQualifiedName);

        // Set the size and location of the signature box (optional)
        // If not set, invisible signature will be placed on first page
        ASFixedRect annotLocation;
        annotLocation.left = ASFloatToFixed(1.0 * 72);
        annotLocation.right = ASFloatToFixed(4.0 * 72);
        annotLocation.top = ASFloatToFixed(6.0 * 72);
        annotLocation.bottom = ASFloatToFixed(8.0 * 72);
        PDSignDocSetSignatureBoxPageNumber(signParams, 0);
        PDSignDocSetSignatureBoxRectangle(signParams, &annotLocation);

        ASText fieldName = ASTextFromEncoded("Signature_es_:signatureblock", PDGetHostEncoding());
        PDSignDocSetFieldName(signParams, fieldName);

        // Set credential related attributes
        // PAdES signatures use SHA-384 digest with EC credentials
        PDSignDocSetDigestCategory(signParams, sha384);
        PDSignDocSetCredentialDataFormat(signParams, NonPFX);

        ASPathName certPath = APDFLDoc::makePath(INPUT_LOC DEF_CERT_FILE);
        ASPathName keyPath = APDFLDoc::makePath(INPUT_LOC DEF_KEY_FILE);

        ASFile asCertFile{nullptr};
        ASErrorCode err = ASFileSysOpenFile64(nullptr, certPath, (ASFILE_READ | ASFILE_SERIAL), &asCertFile);

        ASFile asKeyFile{nullptr};
        err = ASFileSysOpenFile64(nullptr, keyPath, (ASFILE_READ | ASFILE_SERIAL), &asKeyFile);

        PDSignDocSetNonPfxSignerCert(signParams, asCertFile, 0, OnDisk);
        PDSignDocSetNonPfxPrivateKey(signParams, asKeyFile, 0, OnDisk);

        // Set the signature type to PAdES (PDF Advanced Electronic Signatures).
        // NOTE: Signature type must be set prior to adding policies.
        PDSignDocSetDocSignType(signParams, PADES);

        // Define a signature policy (SigPolicyId) using an OID.
        ASText oid = ASTextFromEncoded("2.16.724.1.3.1.1.2.1.9", PDGetHostEncoding());
        PDSignDocSetSigPolicy(signParams, oid);

        // Add a policy qualifier (SPuri) pointing to the policy specification document.
        ASText uri = ASTextFromEncoded(
            "https://sede.administracion.gob.es/politica_de_firma_anexo_1.pdf",
            PDGetHostEncoding());
        PDSignDocSetSigPolicyQualifierURI(signParams, uri);

        // Setup the signer information
        ASText name = ASTextFromEncoded("John Doe", PDGetHostEncoding());
        ASText location = ASTextFromEncoded("Chicago, IL", PDGetHostEncoding());
        ASText reason = ASTextFromEncoded("Approval", PDGetHostEncoding());
        ASText contact = ASTextFromEncoded("Datalogics, Inc.", PDGetHostEncoding());

        // Setup the logo image (optional)
        ASPathName logoPath = APDFLDoc::makePath(csLogoFileName.c_str());
        ASFile asLogoFile{nullptr};
        err = ASFileSysOpenFile64(nullptr, logoPath, (ASFILE_READ | ASFILE_SERIAL), &asLogoFile);
        PDEImage logo{DLCreatePDEImageFromASFile(asLogoFile)};
        ASFixed const opacity{FloatToASFixed(0.50f)};

        PDSignDocSetSignerInfo(signParams, logo, opacity, name, location, reason, contact,
                               DisplayTraits::kDisplayAll);

        // Setup Save params
        PDSignDocSaveParams const saveParams = PDSignDocSaveInitParams();
        ASPathName outPathName = APDFLDoc::makePath(csOutputFileName.c_str());

        PDSignDocSetOutputPath(saveParams, outPathName);

        // Finally, sign and save the document
        PDSignDocWithParams(inDoc, saveParams, signParams);

        // Cleanup
        PDSignDocSaveReleaseParams(saveParams);
        PDSignDocSignReleaseParams(signParams);

        ASTextDestroy(fieldName);
        ASTextDestroy(oid);
        ASTextDestroy(uri);
        ASTextDestroy(name);
        ASTextDestroy(location);
        ASTextDestroy(reason);
        ASTextDestroy(contact);

        // Release logo object
        PDERelease(reinterpret_cast<PDEObject>(logo));
        ASFileSysReleasePath(nullptr, logoPath);

        // Release credential objects
        ASFileSysReleasePath(nullptr, certPath);
        ASFileSysReleasePath(nullptr, keyPath);
        ASFileClose(asCertFile);
        ASFileClose(asKeyFile);

        ASFileClose(asLogoFile);
        PDDocClose(inDoc);
        ASFileSysReleasePath(nullptr, outPathName);

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);
    END_HANDLER

    return errCode;
};
