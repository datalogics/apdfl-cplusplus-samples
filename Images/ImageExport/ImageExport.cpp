/*
 * This sample program reads the pages of the provided PDF file then finds PDEImage's and extracts
 * them as external graphics files, one for each supported type  BMP, PNG, JPG, GIF and TIF.
 *
 * For example, if a page in a PDF file has three images, the program will create three
 * sets of graphics files for those three images. The sample program parses the
 * PDF syntax to identify all PDEImage objects on the given page while recursively checking
 * any PDEContainers, PDEGroups, and PDEForms.
 *
 * Copyright (c) 2024-2025, Datalogics, Inc. All rights reserved.
 *
 */

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "DLExtrasCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"

#define DIR_LOC "../../../../Resources/Sample_Input/"
#define DEF_INPUT "Images.pdf"

// This variable is used to give exported files unique names for the purposes of this sample.
// The values is incremented every time a PDEImage is found in the PDF document.
int nextImg = 1;

// This function is used to create unique export file names with appropriate file extension.
void ExportImage(PDEImage image, DLImageExportType exportType, DLPDEImageExportParams exportParams, int page) {
    PDEImageAttrs attrs;
    PDEImageGetAttrs(image, &attrs, sizeof(PDEImageAttrs));

    // Find the file extension.
    char *imgExt = "";
    switch (exportType) {
    case ExportType_BMP:
        imgExt = ".bmp";
        break;
    case ExportType_PNG:
        imgExt = ".png";
        break;
    case ExportType_JPEG:
        imgExt = ".jpg";
        break;
    case ExportType_GIF:
        imgExt = ".gif";
        break;
    case ExportType_TIF:
        imgExt = ".tif";
        break;
    default:
        std::cout << "Export Failed: Invalid DLImageExportType." << std::endl;
        return;
    }

    // Export to these formats is skipped as they don't support the concept of an Alpha Channel in a standard way
    if (PDEImageHasSMask(image) && (exportType == ExportType_BMP || exportType == ExportType_JPEG || exportType == ExportType_GIF)) {
        return;
    }
    if (attrs.bitsPerComponent != 8 && exportType == ExportType_JPEG) {
        return;
    }
    if (attrs.bitsPerComponent == 1 && exportType == ExportType_GIF) {
        return;
    }

    // Create and format the output file name.
    char buf[1024];
    snprintf(buf, sizeof(buf), "ImageExport_page%d_i%d%s", page + 1, nextImg, imgExt);
    ASPathName sOutput = APDFLDoc::makePath(buf);

    DLExportPDEImage(image, sOutput, exportType, exportParams);

    // Release path when no longer in use.
    ASFileSysReleasePath(NULL, sOutput);
}

// This function provides general information about the PDEImage.
void PrintImageInfo(PDEImage image) {
    PDEImageAttrs attrs;
    PDEImageGetAttrs(image, &attrs, sizeof(PDEImageAttrs));

    ASDouble width = attrs.width;
    ASDouble height = attrs.height;
    ASInt32 bpc = attrs.bitsPerComponent;
    ASInt32 numComps = attrs.numComponents;

    const char *renderIntent = DLPDEImageGetIntent(image);
    char message[1024];
    snprintf(message, sizeof(message),
            "PDEImage data:\n\tWidth:\t%f\n\tHeight:\t%f\n\tNumber of components:\t%i\n"
            "\tBits per component\t%i\n\tRender Intent:\t%s\n",
            width, height, numComps, bpc, renderIntent);
    std::cout << message << std::endl;
}

// This function loops through all PDEContent objects inside a given page and searches for PDEImage elements.
void FindAndExportImages(PDEContent content, int pageNum) {
    int numElements = PDEContentGetNumElems(content);
    int element = 0;

    // Loop through every element on the page.
    while (element < numElements) {
        PDEElement contentElement = PDEContentGetElem(content, element);

        /*
         * If a PDEElement is a PDEImage, then we can export it.
         * Otherwise we also check any PDEGroups, PDEForms, and PDEContainers
         * recursively in the page because they may also contain a PDEImage.
         */
        if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEImage) {
            PDEImage image = (PDEImage)contentElement;

            if (PDEImageIsCosObj(image)) {
                // DLPDEImageExportParams is a structure containing optional user defined parameters. See definition for more details.
                DLPDEImageExportParams exportParams = DLPDEImageGetExportParams();

                char message[1024];
                sprintf(message, "Exporting PDEImage file on page %i from element %i", pageNum + 1, element);
                std::cout << message << std::endl;
                PrintImageInfo(image);

                ExportImage(image, DLImageExportType::ExportType_BMP, exportParams, pageNum);
                ExportImage(image, DLImageExportType::ExportType_PNG, exportParams, pageNum);
                ExportImage(image, DLImageExportType::ExportType_JPEG, exportParams, pageNum);
                ExportImage(image, DLImageExportType::ExportType_GIF, exportParams, pageNum);

                exportParams.TIFFCompression = Compression_LZW;
                ExportImage(image, DLImageExportType::ExportType_TIF, exportParams, pageNum);

                // Increment the global variable to be used when the next PDEImage is found.
                nextImg++;
            }
        } else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEContainer) {
            std::cout << "Recursing through a PDEContainer." << std::endl;
            FindAndExportImages(PDEContainerGetContent((PDEContainer)contentElement), pageNum);
        } else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEGroup) {
            std::cout << "Recursing through a PDEGroup." << std::endl;
            FindAndExportImages(PDEGroupGetContent((PDEGroup)contentElement), pageNum);
        } else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEForm) {
            std::cout << "Recursing through a PDEForm." << std::endl;

            // Unlike other GetContent methods, this method increments the reference count of the returned PDEContent.
            // The returned PDEContent is temporarily stored and needs to be released after it is no longer in use.
            PDEContent tmpContent = PDEFormGetContent((PDEForm)contentElement);
            FindAndExportImages(tmpContent, pageNum);
            PDERelease((PDEObject)tmpContent);
        }
        element++;
    }
}

int main(int argc, char **argv) {
    std::cout << "Initialized the library." << std::endl;
    APDFLib libInit;
    if (libInit.isValid() == false) {
        std::cout << "Initialization failed with code " << libInit.getInitError() << std::endl;
        return libInit.getInitError();
    }
    std::string csInputFileName(argc > 1 ? argv[1] : DIR_LOC DEF_INPUT);
    ASPathName sInput = APDFLDoc::makePath(csInputFileName.c_str());

    DURING
        // Open the provided PDF document and find how many pages it contains.
        PDDoc inputDoc = PDDocOpen(sInput, NULL, NULL, false);
        int numPages = PDDocGetNumPages(inputDoc);

        // Loop through all pages in the PDF document, then find all images, and export them.
        for (int pageNum = 0; pageNum < numPages; pageNum++) {
            PDPage page = PDDocAcquirePage(inputDoc, pageNum);
            PDEContent content = PDPageAcquirePDEContent(page, 0);

            char message[1024];
            sprintf(message, "Searching for images on page %d.", pageNum + 1);
            std::cout << message << std::endl;

            FindAndExportImages(content, pageNum);

            PDPageReleasePDEContent(page, 0);
            PDPageRelease(page);
        }

        PDDocClose(inputDoc);
        ASFileSysReleasePath(NULL, sInput);
    HANDLER
        libInit.displayError(ERRORCODE);
        return ERRORCODE;
    END_HANDLER

    return 0;
}

