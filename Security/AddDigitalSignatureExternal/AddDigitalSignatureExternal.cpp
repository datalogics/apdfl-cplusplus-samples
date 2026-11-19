//
// Copyright (c) 2026, Datalogics, Inc. All rights reserved.
//
// This sample adds a digital signature whose private key the application never gives to APDFL.
// The key stays in a key service, such as Azure Key Vault, AWS KMS or a PKCS#11 token. APDFL builds
// the signature and calls the application's signing operation, HsmSign, to sign it with that key.
//
// HsmSign here is a stand-in: it signs with a local test key file, using OpenSSL. A real
// application replaces its body with a call to its key service; the comments in HsmSign show how.
// To build the sample, set OPENSSL_DIR to an OpenSSL 3 or later installation; see the Makefile
// and the Visual Studio project.
//
// Command-line:  <input-file> <output-file> <timestamp-server-url>   (All optional)
//
// Every signature made this way carries an RFC 3161 timestamp. Without a timestamp server URL,
// APDFL uses its built-in one.
//

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

// Header that includes Digital Signature methods
#include "DLExtrasCalls.h"

#define INPUT_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "SixPages.pdf"
#define DEF_OUTPUT "AddDigitalSignatureExternal-out.pdf"

// DEF_CERT_FILE is the signer certificate, DER-encoded. With a real key service, get the
// certificate from the service or from the certificate authority that issued it.
#define DEF_CERT_FILE "Credentials/DER/RSA_certificate.der"

// DEF_KEY_FILE is the stand-in key service's private key. A real key service never releases it.
#define DEF_KEY_FILE "Credentials/DER/RSA_privKey.der"

// Error codes HsmSign returns. APDFL includes a nonzero code in the error it raises.
enum HsmSignError { kHsmUnsupportedScheme = 1, kHsmUnsupportedDigest = 2, kHsmSignFailed = 3 };

// The stand-in key service: a private key read from a file.
struct KeyService {
    EVP_PKEY *key;
};

// Reads a whole file into memory. Returns false if it can't be read.
static bool readFile(std::string const &path, std::vector<ASUns8> &contents) {
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file) {
        return false;
    }
    contents.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return !contents.empty();
}

// The external signing operation. APDFL calls it once, on this thread, while
// PDSignDocWithParams() runs, and waits for it to return.
//
// It gets the CMS signed attributes to sign (signedAttrs) and their hash (digestBytes) under the
// digest algorithm the application chose. It writes the RSA PKCS#1 v1.5 signature to signature
// and its length to signatureSize, and returns 0, or a nonzero error code on failure.
//
// It must not throw a C++ exception or raise an APDFL error, and must not use the document being
// signed or call any PDSignDoc function.
static ACCB1 ASInt32 ACCB2 HsmSign(void *clientData, PDSignDocExternalSigScheme scheme,
                                   DigestCategory digest, const ASUns8 *digestBytes, ASSize_t digestSize,
                                   const ASUns8 *signedAttrs, ASSize_t signedAttrsSize, ASUns8 *signature,
                                   ASSize_t signatureCapacity, ASSize_t *signatureSize) {
    (void)signedAttrs;
    (void)signedAttrsSize;

    if (scheme != kPDSignDocExternalRSAPKCS1v15) {
        return kHsmUnsupportedScheme;
    }

    // With a real key service, sign digestBytes, or signedAttrs, here instead:
    //  - Azure Key Vault: CryptographyClient Sign with RS256 (RS384, RS512 for the other
    //    digests) and digestBytes as the digest.
    //  - AWS KMS: Sign with MessageType DIGEST, SigningAlgorithm RSASSA_PKCS1_V1_5_SHA_256 (or
    //    _SHA_384, _SHA_512) and digestBytes as the message.
    //  - PKCS#11: C_SignInit with CKM_SHA256_RSA_PKCS (or CKM_SHA384_RSA_PKCS,
    //    CKM_SHA512_RSA_PKCS), then C_Sign over signedAttrs; the token hashes them itself.
    // Catch any exception the service's client library throws, and return an error code instead.
    // APDFL has no timeout of its own, so set one on the call to the service.
    EVP_MD const *md{nullptr};
    switch (digest) {
    case sha256:
        md = EVP_sha256();
        break;
    case sha384:
        md = EVP_sha384();
        break;
    case sha512:
        md = EVP_sha512();
        break;
    default:
        return kHsmUnsupportedDigest;
    }

    KeyService const *keyService{static_cast<KeyService const *>(clientData)};
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> context(
        EVP_PKEY_CTX_new(keyService->key, nullptr), EVP_PKEY_CTX_free);
    size_t size{signatureCapacity};
    if (!context || (EVP_PKEY_sign_init(context.get()) != 1) ||
        (EVP_PKEY_CTX_set_rsa_padding(context.get(), RSA_PKCS1_PADDING) != 1) ||
        (EVP_PKEY_CTX_set_signature_md(context.get(), md) != 1) ||
        (EVP_PKEY_sign(context.get(), signature, &size, digestBytes, digestSize) != 1)) {
        return kHsmSignFailed;
    }
    *signatureSize = static_cast<ASSize_t>(size);
    return 0;
}

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
    std::string const csTimestampURL(argc > 3 ? argv[3] : "");
    std::cout << "Will apply a digital signature made by an external signing operation to "
              << csInputFileName.c_str() << " and save as " << csOutputFileName.c_str() << std::endl;

    // The signer certificate, and the stand-in key service's key
    std::vector<ASUns8> certificate;
    std::vector<ASUns8> keyBytes;
    if (!readFile(INPUT_LOC DEF_CERT_FILE, certificate) || !readFile(INPUT_LOC DEF_KEY_FILE, keyBytes)) {
        std::cout << "Could not read the signer certificate or the test key" << std::endl;
        return -1;
    }
    unsigned char const *keyData{keyBytes.data()};
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> key(
        d2i_AutoPrivateKey(nullptr, &keyData, static_cast<long>(keyBytes.size())), EVP_PKEY_free);
    if (!key) {
        std::cout << "Could not load the test key" << std::endl;
        return -1;
    }
    KeyService keyService{key.get()};

    DURING

        // Open the input document.
        APDFLDoc APDoc(csInputFileName.c_str(), true);
        PDDoc inDoc = APDoc.getPDDoc();

        // Setup Sign params
        PDSignDocSignParams const signParams = PDSignDocSignInitParams();

        // External signing makes CMS signatures, with a SHA-256, SHA-384 or SHA-512 digest.
        PDSignDocSetDocSignType(signParams, CMS);
        PDSignDocSetDigestCategory(signParams, sha256);

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

        // The signer certificate. APDFL copies it.
        PDSignDocSetExternalSignerCert(signParams, certificate.data(),
                                       static_cast<ASSize_t>(certificate.size()));

        // This test certificate is self-signed. For a certificate issued by an intermediate
        // certificate authority, add the chain to embed in the signature, starting with the
        // signer certificate's issuer:
        //     PDSignDocAddExternalSignerChainCert(signParams, issuer.data(), issuer.size());

        // The signing operation, instead of a private key. APDFL passes &keyService back to it.
        PDSignDocSetExternalSigner(signParams, HsmSign, &keyService);

        // The timestamp server, if one was given
        if (!csTimestampURL.empty()) {
            ASText timestampURL = ASTextFromEncoded(csTimestampURL.c_str(), PDGetHostEncoding());
            PDSignDocSetTimestampServerURL(signParams, timestampURL);
            ASTextDestroy(timestampURL);
        }

        // Setup the signer information
        ASText name = ASTextFromEncoded("John Doe", PDGetHostEncoding());
        ASText location = ASTextFromEncoded("Chicago, IL", PDGetHostEncoding());
        ASText reason = ASTextFromEncoded("Approval", PDGetHostEncoding());
        ASText contact = ASTextFromEncoded("Datalogics, Inc.", PDGetHostEncoding());

        PDSignDocSetSignerInfo(signParams, nullptr, 0, name, location, reason, contact, DisplayTraits::kDisplayAll);

        // Setup Save params
        PDSignDocSaveParams const saveParams = PDSignDocSaveInitParams();
        ASPathName outPathName = APDFLDoc::makePath(csOutputFileName.c_str());

        PDSignDocSetOutputPath(saveParams, outPathName);

        // Finally, sign and save the document. Only one thread in a process may sign at a time,
        // so an application that signs on several threads must make these calls one at a time.
        PDSignDocWithParams(inDoc, saveParams, signParams);
        std::cout << "Signed and saved as " << csOutputFileName.c_str() << std::endl;

        // Cleanup
        ASTextDestroy(fieldName);
        ASTextDestroy(name);
        ASTextDestroy(location);
        ASTextDestroy(reason);
        ASTextDestroy(contact);

        PDSignDocSignReleaseParams(signParams);
        PDSignDocSaveReleaseParams(saveParams);

        // PDSignDocWithParams has closed the document; PDDocClose does nothing in that case.
        PDDocClose(inDoc);
        ASFileSysReleasePath(nullptr, outPathName);

    HANDLER
        errCode = ERRORCODE;
        lib.displayError(errCode);
    END_HANDLER

    return errCode;
};
