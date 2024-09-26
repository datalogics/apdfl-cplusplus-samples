//
// Copyright (c) 2010-2024, Datalogics, Inc. All rights reserved.
//
// This sample program demonstrates how to scan a PDF document to determine whether the fonts
// used in that document are embedded.
//
//

#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "CosCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#define INPUT_DIR "../../../../Resources/Sample_Input/"
#define INPUT_NAME "EmbedFonts-in.pdf"
#define OUTPUT_NAME "EmbedFonts-out.pdf"

#include <vector>

// Tracking information for the fonts used in a document
struct UnembeddedFont {
    CosObj cosObj;
};

void EmbedSysFontForFontEntry(UnembeddedFont fontEntry, PDDoc pdDoc) {
    DURING
        PDEFont pdeFontUnembedded = PDEFontCreateFromCosObj(&fontEntry.cosObj);

        PDEFontAttrs attrs;
        memset(&attrs, 0, sizeof(PDEFontAttrs));
        PDEFontGetAttrs(pdeFontUnembedded, &attrs, sizeof(PDEFontAttrs));

        PDSysEncoding sysEnc = NULL;
        if (attrs.type == ASAtomFromString("Type0")) {
            sysEnc = PDSysEncodingCreateFromCMapName(attrs.encoding);
        }
        else {
            sysEnc = PDSysEncodingCreateFromBaseName(attrs.encoding, NULL);
        }

        PDSysFont sysFont = PDFindSysFontForPDEFont(pdeFontUnembedded, kPDSysFontMatchNameAndCharSet);

        // If the font is not found on the system, sysFont will be 0.
        if (NULL == sysFont) {
            std::cout << "Could not find a pdSysFont " << std::endl;
            return;
        }

        PDEFontSetSysFont(pdeFontUnembedded, sysFont);

        if (sysEnc) {
            PDEFontSetSysEncoding(pdeFontUnembedded, sysEnc);
        }

        if (attrs.cantEmbed != 0) {
            std::cout << "Font " << ASAtomGetString(attrs.name) << " cannot be embedded" << std::endl;
        }
        else {
            if (PDEFontIsMultiByte(pdeFontUnembedded)) {
                PDEFontAttrs foundSysFontAttrs;
                memset(&foundSysFontAttrs, 0, sizeof(PDEFontAttrs));

                PDSysFontGetAttrs(sysFont, &foundSysFontAttrs, sizeof(PDEFontAttrs));

                PDFont currentFont = PDFontFromCosObj(fontEntry.cosObj);
                PDFont currentDescendantFont = PDFontGetDescendant(currentFont);
                ASAtom currentDescendantFontSubtype = PDFontGetSubtype(currentDescendantFont);

                //If the OS Font discovered is TrueType-based, but the existing CIDFont isn't, don't attempt to embed it.
                if (foundSysFontAttrs.type == ASAtomFromString("TrueType") && ASAtomFromString("CIDFontType2") != currentDescendantFontSubtype) {
                    return;
                }
                //If the OS Font discovered is Type1-based, but the existing CIDFont isn't, don't attempt to embed it.
                if (foundSysFontAttrs.type == ASAtomFromString("Type1") && ASAtomFromString("CIDFontType0") != currentDescendantFontSubtype) {
                    return;
                }

                // Subset embed font
                PDEFont pdeFont = NULL;
                if (sysEnc) {
                    pdeFont = PDEFontCreateFromSysFontAndEncoding(sysFont, sysEnc, attrs.name, kPDEFontCreateEmbedded);
                }
                else
                {
                    pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded | kPDEFontCreateSubset);
                }

                PDEFontSubsetNow(pdeFontUnembedded, PDDocGetCosDoc(pdDoc));

                PDERelease((PDEObject)pdeFont);
            }
            else {
                // Fully embed font
                PDEFont pdeFont = PDEFontCreateFromSysFont(sysFont, kPDEFontCreateEmbedded);
                PDEFontEmbedNow(pdeFontUnembedded, PDDocGetCosDoc(pdDoc));

                PDERelease((PDEObject)pdeFont);
            }
        }

        if (sysEnc) {
            PDERelease((PDEObject)sysEnc);
        }
    HANDLER
        APDFLib::displayError(ERRORCODE);
    END_HANDLER
}

ACCB1 ASBool ACCB2 GetFontInfoProc(PDFont pdFont, PDFontFlags* fontFlags, std::vector<UnembeddedFont>& clientData) {
    CosObj cosFont;
    PDEFontAttrs attrs;
    PDSysFont sysFont;
    char fontNameBuf[PSNAMESIZE];
    const char* fontSubtypeP;
    char* fontNameStart = 0;
    ASBool fontEmbedded = false, fontSubset = false, fontIsSysFont = false;
    DURING
        memset(&attrs, 0, sizeof(PDEFontAttrs));

        PDFontGetName(pdFont, fontNameBuf, PSNAMESIZE);
        attrs.name = ASAtomFromString(fontNameBuf);
        attrs.type = PDFontGetSubtype(pdFont);
        fontSubtypeP = ASAtomGetString(attrs.type);

        fontEmbedded = PDFontIsEmbedded(pdFont);

        if (fontEmbedded) {
            if ((strlen(fontNameBuf)) > 7 && (fontNameBuf[6] == '+')) {
                    fontSubset = true;
            }
        }
        if (fontSubset) {
            fontNameStart = fontNameBuf + 7;
        }
        else
        {
            fontNameStart = fontNameBuf;
        }

        sysFont = PDFindSysFont(&attrs, sizeof(PDEFontAttrs), 0);
        if (sysFont) {
            fontIsSysFont = true;
        }

        // Print font information
        std::cout << "Font " << fontNameStart << ", Subtype " << fontSubtypeP << " ("
            << (fontIsSysFont ? "" : "Not a ") << "System Font, ";
        if (fontEmbedded) {
            std::cout << "embedded" << (fontSubset ? " subset" : "");
        }
        else {
            std::cout << "unembedded";
        }
        std::cout << ")" << std::endl;

        // Add font to the list of fonts to be subset. This example only subsets System
        // fonts that are not embedded in this PDF document. The sample will not subset fonts
        // that are fully embedded in the PDF file.
        if (fontIsSysFont && !fontEmbedded) {
            UnembeddedFont fontToBeEmbedded;
            fontToBeEmbedded.cosObj = PDFontGetCosObj(pdFont);

            clientData.push_back(fontToBeEmbedded);
        }
    HANDLER
        std::cout << "Exception raised in GetFontInfoProc(): ";
        APDFLib::displayError(ERRORCODE);
    END_HANDLER

    return true;
}

int main(int argc, char **argv) {
    ASErrorCode errCode = 0;

    APDFLib libInit; // Initialize the library
    if (libInit.isValid() == false) {
        ASErrorCode errCode = libInit.getInitError();
        APDFLib::displayError(errCode);
        return errCode;
    }

    std::string csInputFileName(argc > 1 ? argv[1] : INPUT_DIR INPUT_NAME);
    std::string csOutputFileName(argc > 2 ? argv[2] : OUTPUT_NAME);
    std::cout << "Will embed fonts into " << csInputFileName.c_str() << " and rewrite file as "
              << csOutputFileName.c_str() << std::endl;

    std::vector<UnembeddedFont> fontsToBeEmbedded;

    DURING
        // Open the input document
        APDFLDoc apdflDoc(csInputFileName.c_str(), true);
        PDDoc pdDoc = apdflDoc.getPDDoc();

        // Enumerate the fonts used in the document, and save those currently unembedded
        PDDocEnumFonts(pdDoc, 0, PDDocGetNumPages(pdDoc) - 1, (PDFontEnumProc)GetFontInfoProc, &fontsToBeEmbedded, 0, 0);

        // Embed a suitable system font for each font in the to-embed list
        for (std::vector<UnembeddedFont>::iterator it = fontsToBeEmbedded.begin(); it != fontsToBeEmbedded.end(); it++) {
            EmbedSysFontForFontEntry(*it, pdDoc);
        }

        // Save the PDF to a new file
        apdflDoc.saveDoc(csOutputFileName.c_str(), PDSaveFull | PDSaveCollectGarbage);
    HANDLER
        errCode = ERRORCODE;
        APDFLib::displayError(errCode);
    END_HANDLER

    return errCode;
}
