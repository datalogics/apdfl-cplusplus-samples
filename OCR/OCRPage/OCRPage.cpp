//
// Copyright (c) 2017-2025, Datalogics, Inc. All rights reserved.
//
//
// The OCRPage sample demonstrates how the Library works to OCR a page.
//
// Command-line:  <output-file>     (Optional)
//

#include <iostream>

#include "ASExtraCalls.h"
#include "DLExtrasCalls.h"
#include "OCREngineCalls.h"

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "OCRPage.pdf"
#define DEF_OUTPUT "OCRPage-out.pdf"

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

    DURING
        // Sets the correct location for the OCREngine function table.
        gOCREngineHFT = InitOCREngineHFT;

        // Initialize the OCREngine plugin.
        if (!OCREngineInitialize()) {
            std::cout << "The OCREngine plugin failed to initialize." << std::endl;
            errCode = -1;
        }

        if (0 == errCode) {
            // Obtain first page in document.
            APDFLDoc inDoc(csInputFileName.c_str(), true);
            PDDoc pdDoc = inDoc.getPDDoc();
            PDPage inputPage1 = PDDocAcquirePage(pdDoc, 0);

            // Set default OCR parameters.
            OCRParams ocrParams = PDOCRDefaultParams();

            // Set languages to configure OCREngine with.
            OCRLanguage newLanguages[] = {OCRLanguage_English, OCRLanguage_French, OCRLanguage_ChineseTraditional,
                                          OCRLanguage_ChineseSimplified, OCRLanguage_Japanese};

            ASInt32 numLanguages = sizeof(newLanguages) / sizeof(newLanguages[0]);
            PDOCRParamsSetLanguagesConfigured(ocrParams, newLanguages, numLanguages);

            // Create an OCR engine from the parameters.
            OCREngine ocrEngine = PDOCRCreateEngineFromParams(ocrParams);

            // The parameters are no longer needed after creating the engine.
            PDOCRReleaseParams(ocrParams);

            // Run OCR on the page.
            PDOCRRecognizePage(inputPage1, ocrEngine, OCRMissingFontStrategy_Raise);

            // Save document with recognized text.
            ASPathName path = APDFLDoc::makePath(csOutputFileName.c_str());
            PDDocSave(pdDoc, PDSaveFull, path, ASGetDefaultFileSys(), nullptr, nullptr);
            ASFileSysReleasePath(nullptr, path);

            // Release resources.
            PDPageRelease(inputPage1);

            // Release OCREngine resources and terminate the plugin.
            PDOCRReleaseEngine(ocrEngine);
            OCREngineTerminate();
        } // if 0 == errCode
    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode; // APDFLib's destructor terminates the library.
}
