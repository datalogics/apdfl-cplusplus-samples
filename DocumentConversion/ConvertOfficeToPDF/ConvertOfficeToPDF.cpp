// Copyright (c) 2026, Datalogics, Inc. All rights reserved.
//
//
// This sample demonstrates the OfficeToPDF plugin, which converts a Microsoft
// Word (.docx) document into a PDF.
//
// By default the sample converts DOCXLink.docx from the samples' input
// directory and writes the result to ConvertOfficeToPDF-out.pdf in the current
// working directory.
//
// OfficeToPDFConvertToFile takes an input path and an output path and returns
// an OfficeToPDFResult, which carries that conversion's status, its message and
// its per-asset diagnostics until the caller releases it.
//

#include "InitializeLibrary.h"

#include "ASExtraCalls.h"

#include "OfficeToPDFCalls.h"

// OfficeToPDFCalls.h declares gOfficeToPDFHFT as extern; the client application
// must provide the actual definition. The accessor macros dereference it to
// call through the Host Function Table obtained from APDFL's extension manager.
void *gOfficeToPDFHFT = NULL;

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "DOCXLink.docx"
#define DEF_OUTPUT "ConvertOfficeToPDF-out.pdf"

static const char *DiagnosticKindName(OfficeToPDFDiagnosticKind kind) {
    switch (kind) {
    case kOfficeToPDFDiagPlaceholderImage:         return "placeholder image";
    case kOfficeToPDFDiagSubstitutedFont:          return "substituted font";
    case kOfficeToPDFDiagEmbeddedFont:             return "embedded font";
    case kOfficeToPDFDiagSynthesizedFont:          return "synthesized font";
    case kOfficeToPDFDiagPlaceholderEquation:      return "placeholder equation";
    case kOfficeToPDFDiagMathFontFallback:         return "math font fallback";
    case kOfficeToPDFDiagChart:                    return "chart";
    case kOfficeToPDFDiagPlaceholderChart:         return "placeholder chart";
    case kOfficeToPDFDiagDroppedLink:              return "dropped link";
    case kOfficeToPDFDiagUnsupportedGraphic:       return "unsupported graphic";
    case kOfficeToPDFDiagSeparatorFallback:        return "separator fallback";
    case kOfficeToPDFDiagHeaderFooterNoteClipped:  return "header/footer note clipped";
    case kOfficeToPDFDiagInertNestedReference:     return "inert nested note reference";
    case kOfficeToPDFDiagNoteContinuationExhausted:return "note continuation exhausted";
    case kOfficeToPDFDiagApproximatedShape:        return "approximated shape";
    case kOfficeToPDFDiagUnshapedText:             return "unshaped text";
    case kOfficeToPDFDiagWatermark:                return "watermark";
    case kOfficeToPDFDiagApproximatedPicture:      return "approximated picture";
    case kOfficeToPDFDiagUnresolvedPictureStyle:   return "unresolved picture style";
    case kOfficeToPDFDiagFloatPlacementUnresolved: return "float placement unresolved";
    case kOfficeToPDFDiagDegenerateGeometry:       return "degenerate page geometry";
    case kOfficeToPDFDiagComment:                  return "comment";
    case kOfficeToPDFDiagFloatNotPlaced:           return "float not placed";
    // A newer plugin may report a kind this header predates. Read the
    // diagnostic's asset and message, and see OfficeToPDFGetVersionString().
    default:                                       return "other";
    }
}

int main(int argc, char **argv) {
    APDFLib lib;             // Initialize the Adobe PDF Library.
    ASErrorCode errCode = 0; // Variable used to report any exceptions/errors if they occurred.

    if (lib.isValid() == false) // If there was a problem in initialization, return the error code.
        return lib.getInitError();

    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    std::string csOutputFileName(argc > 2 ? argv[2] : DEF_OUTPUT);

    DURING

        //=========================================================================================================================
        // 1) Locate the OfficeToPDF plugin in the APDFL extension manager and initialize it.
        //=========================================================================================================================

        gOfficeToPDFHFT = reinterpret_cast<void *>(
            ASExtensionMgrGetHFT(ASAtomFromString(OfficeToPDFHFTName), OfficeToPDFHFTVersion));
        if (gOfficeToPDFHFT == NULL) {
            std::cout << "Could not locate the OfficeToPDF plugin. Ensure the "
                         "plugin binary is present in the APDFL Binaries directory."
                      << std::endl;
            // Raise a real code: ERRORCODE is the code of the last raise on this
            // thread, 0 when nothing has raised, and the sample would exit 0.
            ASRaise(GenError(genErrGeneral));
        }

        // The plugin does not initialize APDFL; it runs on the library session
        // this application already started. Reached through the HFT, the plugin
        // is ready already and this call confirms it.
        if (!OfficeToPDFInitialize()) {
            std::cout << "OfficeToPDF could not initialize." << std::endl;
            ASRaise(GenError(genErrGeneral));
        }

        std::cout << "OfficeToPDF " << OfficeToPDFGetVersionString() << std::endl;
        std::cout << "Converting " << csInputFileName.c_str() << " and saving as "
                  << csOutputFileName.c_str() << std::endl;

        //=========================================================================================================================
        // 2) Configure conversion parameters.
        //=========================================================================================================================

        // Set the size first: OfficeToPDFInitParams fills only that many bytes,
        // so a plugin newer than this header never writes past the record. It
        // then populates the defaults: the system clock, and comments omitted.
        // One record serves any number of conversions on this one plugin session.
        OfficeToPDFParamsRec params;
        params.size = sizeof(OfficeToPDFParamsRec);
        OfficeToPDFInitParams(&params);

        // kOfficeToPDFCommentsOmit (default), kOfficeToPDFCommentsMargin, or
        // kOfficeToPDFCommentsAnnotations.
        params.comments = kOfficeToPDFCommentsMargin;

        // A fixed instant is stamped as /CreationDate and /ModDate and used for
        // DATE and TIME fields, so the same document converts to the same bytes.
        // Leave useConversionTime false to stamp the system clock instead.
        params.useConversionTime = true;
        params.conversionTime.year = 2026;
        params.conversionTime.month = 1;
        params.conversionTime.day = 1;
        params.conversionTime.hour = 0;
        params.conversionTime.minute = 0;
        params.conversionTime.second = 0;

        //=========================================================================================================================
        // 3) Perform the conversion.
        //=========================================================================================================================

        OfficeToPDFResult result =
            OfficeToPDFConvertToFile(csInputFileName.c_str(), csOutputFileName.c_str(), &params);

        // A NULL result means the plugin could not allocate one. Every accessor
        // below tolerates NULL and yields its documented default.
        const OfficeToPDFStatus status = OfficeToPDFResultGetStatus(result);

        if (status != kOfficeToPDFSuccess) {
            std::cout << "Conversion failed: " << OfficeToPDFResultGetMessage(result) << std::endl;
            switch (status) {
            case kOfficeToPDFErrorInputNotFound:
                std::cout << "  (source document missing or unreadable)" << std::endl;
                break;
            case kOfficeToPDFErrorInvalidInput:
                std::cout << "  (source is not a valid .docx document)" << std::endl;
                break;
            case kOfficeToPDFErrorInputProtected:
                std::cout << "  (source is password-protected)" << std::endl;
                break;
            case kOfficeToPDFErrorDestinationNotWritable:
                std::cout << "  (destination directory is missing or read-only)" << std::endl;
                break;
            default:
                break;
            }
        } else {
            std::cout << "Successfully converted the document." << std::endl;
        }

        //=========================================================================================================================
        // 4) Report anything the conversion approximated.
        //=========================================================================================================================

        // A successful conversion can still report diagnostics: a substituted
        // font, an undecodable image, a hyperlink dropped as unsafe. They
        // describe the rendered assets; they do not change the status.
        const ASSize_t diagCount = OfficeToPDFResultGetDiagnosticCount(result);
        for (ASSize_t i = 0; i < diagCount; ++i) {
            std::cout << "  [" << DiagnosticKindName(OfficeToPDFResultGetDiagnosticKind(result, i))
                      << "] " << OfficeToPDFResultGetDiagnosticAsset(result, i) << ": "
                      << OfficeToPDFResultGetDiagnosticMessage(result, i) << std::endl;
        }

        // Release the result once. It owns the message and the diagnostics, so
        // copy anything you keep before this call.
        OfficeToPDFResultRelease(result);

        // Close the plugin. After this, a conversion selector returns
        // kOfficeToPDFErrorNotInitialized until OfficeToPDFInitialize() runs again.
        OfficeToPDFTerminate();

    HANDLER

        errCode = ERRORCODE;
        lib.displayError(errCode); // If there was an error, display it.

    END_HANDLER

    return errCode; // APDFLib's destructor terminates the library.
}
