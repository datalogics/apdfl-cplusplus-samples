//
// Copyright (c) 2017-2025, Datalogics, Inc. All rights reserved.
//
//
// The OCRImage sample demonstrates how the Library works to OCR an image.
//
// Command-line:  <output-file>     (Optional)
//

#include <iostream>

#include "ASExtraCalls.h"
#include "DLExtrasCalls.h"
#include "PSFCalls.h"
#include "PERCalls.h"
#include "PEWCalls.h"
#include "PagePDECntCalls.h"
#include "PagePDECntCalls.h"
#include "OCREngineCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "OCRImage.png"
#define DEF_OUTPUT "OCRImage-out.pdf"

int main(int argc, char **argv) {
    APDFLib libInit;
    ASErrorCode errCode = 0;
    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);
    std::cout << "Recognizing text in " << csInputFileName.c_str() << std::endl;

    ASPathName sInput = APDFLDoc::makePath(csInputFileName.c_str());
    ASPathName sOutput = APDFLDoc::makePath(csOutputFileName.c_str());

    DURING
        // Sets the correct location for the OCREngine function table.
        gOCREngineHFT = InitOCREngineHFT;

        // Initialize the OCREngine plugin.
        if (!OCREngineInitialize()) {
            std::cout << "The OCREngine plugin failed to initialize." << std::endl;
            errCode = -1;
        }

        if (0 == errCode) {
            // Create a PDEImage object to perform OCR on.
            PDEImage image = DLCreatePDEImageFromFile(sInput, nullptr);

            // Set default OCR parameters.
            OCRParams ocrParams = PDOCRDefaultParams();

            // Set languages to configure OCREngine with.
            OCRLanguage newLanguages[] = {OCRLanguage_English, OCRLanguage_French, OCRLanguage_ChineseTraditional,
                                          OCRLanguage_ChineseSimplified, OCRLanguage_Japanese};

            ASInt32 numLanguages = sizeof(newLanguages) / sizeof(newLanguages[0]);
            PDOCRParamsSetLanguagesConfigured(ocrParams, newLanguages, numLanguages);

            // Create the destination document for the created form.
            PDDoc doc = PDDocCreate();

            ASFixedRect mediaBox = {};
            mediaBox.left = fixedZero;
            mediaBox.right = FloatToASFixed(72.0 * 8.5);
            mediaBox.bottom = fixedZero;
            mediaBox.top = FloatToASFixed(72.0 * 11.0);
            
            PDPage page = PDDocCreatePage(doc, kPDEBeforeFirst, mediaBox);

            // Run OCR on the image to get Form element containing the image with text underneath.
            PDEForm form = PDOCRCreateForm(ocrParams, doc, image, 300, OCRMissingFontStrategy_Raise);

            // Put that form into the page in the destination document.
            PDEContent content = PDPageAcquirePDEContent(page, 0);
            PDEContentAddElem(content, PDEContentGetNumElems(content) - 1, (PDEElement)form);
            PDPageSetPDEContent(page, 0);

            // Save the output.
            PDDocSave(doc, PDSaveFull | PDSaveLinearized, sOutput, NULL, NULL, NULL);

            // Release resources.
            PDPageReleasePDEContent(page, 0);
            PDPageRelease(page);
            PDDocClose(doc);

            PDERelease((PDEObject)form);
            PDERelease(reinterpret_cast<PDEObject>(image));
            ASFileSysReleasePath(NULL, sInput);
            ASFileSysReleasePath(NULL, sOutput);

            // Release OCREngine resources and terminate the plugin.
            PDOCRReleaseParams(ocrParams);
            OCREngineTerminate();
        } // if 0 == errCode
    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode; // APDFLib's destructor terminates the library.
}
