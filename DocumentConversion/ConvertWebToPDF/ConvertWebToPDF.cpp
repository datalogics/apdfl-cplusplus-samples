// Copyright (c) 2024-2025, Datalogics, Inc. All rights reserved.
//
//
// This sample demonstrates the WebToPDF plugin, which converts a web page
// (http/https URL) or a local HTML file into a PDF document using the
// Chromium Embedded Framework (CEF) rendering engine.
//
// By default the sample fetches https://www.datalogics.com and writes the
// resulting PDF to ConvertWebToPDF-out.pdf in the current working directory.
//

#include "InitializeLibrary.h"
#include "APDFLDoc.h"

#include "PDCalls.h"
#include "ASExtraCalls.h"

#include "WebToPDFCalls.h"

// The WebToPDFCalls.h header declares gWebToPDFHFT as extern; the client
// application must provide the actual definition. The HFT accessor macros in
// WebToPDFCalls.h dereference this pointer to call functions through the
// Host Function Table obtained from APDFL's extension manager.
void *gWebToPDFHFT = NULL;

#define DEF_INPUT_URL "https://www.datalogics.com"
#define DEF_OUTPUT "ConvertWebToPDF-out.pdf"

// Progress callback invoked by the plugin during conversion. Returning false
// requests cancellation; here we simply print progress to stdout and continue.
static ASBool WebToPDFProgress(ASInt32 /*pageNum*/, ASInt32 /*totalPages*/,
                               float progress, void * /*clientData*/) {
    std::cout << "\rProgress: " << static_cast<int>(progress * 100) << "%  " << std::flush;
    return true;
}

// Log callback. The plugin emits messages on its internal worker thread, so
// implementations should be thread-safe. We just forward everything to stdout.
static void WebToPDFLog(ASInt32 level, const char *message, void * /*clientData*/) {
    const char *levelStr = "INFO";
    switch (level) {
    case 0: levelStr = "ERROR";   break;
    case 1: levelStr = "WARNING"; break;
    case 2: levelStr = "INFO";    break;
    case 3: levelStr = "DEBUG";   break;
    }
    std::cout << "[WebToPDF " << levelStr << "] " << message << std::endl;
}

int main(int argc, char **argv) {
    APDFLib lib;             // Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0; // Variable used to report any exceptions/errors if they occurred.

    if (lib.isValid() == false) // If there was a problem in initialization, return the error code.
        return lib.getInitError();

    DURING

        //=========================================================================================================================
        // 1) Locate the WebToPDF plugin in the APDFL extension manager and initialize it.
        //=========================================================================================================================

        // Ask APDFL for the WebToPDF Host Function Table by name and version.
        // The name and version constants are defined in WebToPDFCalls.h.
        gWebToPDFHFT = reinterpret_cast<void *>(
            ASExtensionMgrGetHFT(ASAtomFromString(WebToPDFHFTName), WebToPDFHFTVersion));
        if (gWebToPDFHFT == NULL) {
            std::wcout << L"Could not locate the WebToPDF plugin. Ensure the "
                          L"plugin binary is present in the APDFL Binaries directory."
                       << std::endl;
            ASRaise(ERRORCODE); // The handler will display the error code.
        }

        // Initialize the plugin. This allocates the CEF runtime the first
        // time a conversion is performed.
        if (!WebToPDFInitialize()) {
            std::wcout << L"WebToPDF could not initialize." << std::endl;
            ASRaise(ERRORCODE);
        }

        std::string csInputURL(argc > 1 ? argv[1] : DEF_INPUT_URL);
        std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);

        std::cout << "Converting " << csInputURL.c_str() << " and saving as "
                  << csOutputFileName.c_str() << std::endl;

        //=========================================================================================================================
        // 2) Configure conversion parameters.
        //=========================================================================================================================

        // Fill the parameters structure with defaults, then override the
        // specific settings we care about. WebToPDFInitParams sets the size
        // field for versioning and populates sensible defaults.
        WebToPDFParamsRec params;
        WebToPDFInitParams(&params);

        params.viewportSize = kWebToPDFViewportDesktop;
        params.pageOrientation = kWebToPDFOrientationPortrait;
        params.pageSize = kWebToPDFPageSizeLetter;
        params.margins.top = 0.5;
        params.margins.right = 0.5;
        params.margins.bottom = 0.5;
        params.margins.left = 0.5;
        params.imageCompression = kWebToPDFCompressionJPEG;
        params.downsamplingDPI = kWebToPDFDPI300;
        params.printBackground = true;
        params.timeoutSeconds = 300;

        params.progressCallback = WebToPDFProgress;
        params.progressClientData = NULL;
        params.logCallback = WebToPDFLog;
        params.logClientData = NULL;

        //=========================================================================================================================
        // 3) Perform the conversion.
        //=========================================================================================================================

        WebToPDFResultCode result =
            WebToPDFConvertURL(csInputURL.c_str(), csOutputFileName.c_str(), &params);

        std::cout << std::endl; // Newline after the progress indicator.

        if (result != kWebToPDFSuccess) {
            char errorBuffer[1024] = {0};
            WebToPDFGetLastError(errorBuffer, sizeof(errorBuffer));
            std::wcout << L"Conversion failed: ";
            std::cout << errorBuffer << std::endl;
        } else {
            //=========================================================================================================================
            // 4) Report information about the converted document.
            //=========================================================================================================================

            WebToPDFConversionInfoRec info;
            info.size = sizeof(info);
            if (WebToPDFGetConversionInfo(&info)) {
                std::cout << "Pages written: " << info.pageCount
                          << "  Conversion time: " << info.conversionTimeMs << " ms"
                          << std::endl;
                if (info.title) {
                    std::cout << "Document title: " << info.title << std::endl;
                }
            }

            std::wcout << L"Successfully converted the web page." << std::endl;
        }

        // Close the plugin. After this, no WebToPDF* function may be called
        // until WebToPDFInitialize() is invoked again.
        WebToPDFTerminate();

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode); // If there was an error, display it.

    END_HANDLER

    return errCode; // APDFLib's destructor terminates the library.
}
