/*
 * This sample program reads the pages of the provided PDF file and extracts images
 * that it finds on each page and saves those images to external graphics files, one each for
 * BMP, PNG, JPG, GIF and TIF. 
 * 
 * The program examines the content stream for image elements and exports
 * those image objects. If a page in a PDF file has three images, the program will create three
 * sets of graphics files for those three images. The sample program parses the
 * PDF syntax to identify all image objects on the given page by also checking Container,
 * Groups and Forms. The sample program, ignores Text.
 *
 * Copyright (c) 2024, Datalogics, Inc. All rights reserved.
 *
 * For complete copyright information, refer to:
 * http://dev.datalogics.com/adobe-pdf-library/license-for-downloaded-pdf-samples/
 *
 */

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "DLExtrasCalls.h"
#include "PERCalls.h"
#include "PagePDECntCalls.h"
#include "PEWCalls.h"

// This variable is used to give exported files unique names for the purposes of this sample.
// The values is incremented every time an image is found in the PDF document.
int nextImg = 1;

// This function is used to create unique export file names with appropriate file extension.
void Export(PDEImage image, ImageExportType exporttype,int page) {
	// Find the file extension.
    char* imgExt = "";
    switch (exporttype) {
    case BMP:
        imgExt = ".bmp";
        break;
    case PNG:
        imgExt = ".png";
        break;
    case JPEG:
        imgExt = ".jpg";
        break;
    case GIF:
        imgExt = ".gif";
        break;
    case TIF:
        imgExt = ".tif";
        break;
    default:
        imgExt = "";
        break;
    }
	
	// Create and format the output file name.
    char buf[1024];
    sprintf(buf, "ImageExport_page%d_i%d%s", page + 1, nextImg, imgExt);
    ASPathName sOutput = APDFLDoc::makePath(buf);
	
	// This is the public API call 
    DLExportPDEImage(image, sOutput, exporttype);
	
	// Release path when no longer in use.
    ASFileSysReleasePath(NULL, sOutput);
}

// This function loops throught all content inside a given page and searches for image elements.
void FindImages(PDEContent content, int pageNum)
{
    int numElements = PDEContentGetNumElems(content);
    int element = 0;
	
    // Loop throught every element on the page.
    while (element < numElements) 
   {
        PDEElement contentElement = PDEContentGetElem(content, element);
        
        // If an image element is found then that image is exported.
        // Otherwise we also check any Groups, Forms, and Containers
        // in the page because they may also contain images.  
        if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEImage) 
        {
			char message[1024];
            sprintf(message, "Image found. Exporting as file ImageExport_page%d_i%d.", pageNum + 1, imgExt);
            std::cout << message << std::endl;

            PDEImage image = (PDEImage)contentElement;
            DLExport(image, ImageExportType::BMP, pageNum);
            DLExport(image, ImageExportType::PNG, pageNum);
            DLExport(image, ImageExportType::JPEG, pageNum);
            DLExport(image, ImageExportType::GIF, pageNum);
            DLExport(image, ImageExportType::TIF, pageNum);
			
			// Increment the global variable to be used when the next image is found.
            nextImg++;
        }
        else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEContainer)
        {
            std::cout << "Recursing through a Container." << std::endl;
            FindImages(PDEContainerGetContent((PDEContainer)contentElement), pageNum);
        }
        else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEGroup)
        {
            std::cout << "Recursing through a Group." << std::endl;
            FindImages(PDEGroupGetContent((PDEGroup)contentElement), pageNum);
        }
        else if (PDEObjectGetType(((PDEObject)contentElement)) == kPDEForm)
        {
            std::cout << "Recursing through a Form" << std::endl;
			
			// Unlike other GetContent methods, this method increments the reference count of the returned PDEContent.
			// The returned PDEContent is temporarily stored and is release after it's no longer in use.
            PDEContent tmpContent = PDEFormGetContent((PDEForm)contentElement);
            FindImages(tmpContent, pageNum);
            PDERelease((PDEObject)tmpContent);
        }
        element++;
    }
}

int main(int argc, char** argv) 
{
    std::cout << "Initialized the library." << std::endl;
    APDFLib libInit;
    if (libInit.isValid() == false) {
        std::cout << "Initialization failed with code " << libInit.getInitError() << std::endl;
        return libInit.getInitError();
    }

    ASPathName sInput = APDFLDoc::makePath("Sample_Input/Images.pdf");

    DURING
        // Open the provided PDF document and find how many pages it countains.
        PDDoc inputDoc = PDDocOpen(sInput, NULL, NULL, false);
        int numPages = PDDocGetNumPages(inputDoc);

        // Loop throught all pages in the PDF document, then find all images, and export them for supported image formats: BMP, PNG, JPG, GIF and TIF.
        for (int pageNum = 0; pageNum < numPages; pageNum++) 
        {
            PDPage page = PDDocAcquirePage(inputDoc, pageNum);
            PDEContent content = PDPageAcquirePDEContent(page, 0);
			
			char message[1024];
            sprintf(message, "Searching for images on page %d", pageNum + 1);
            std::cout << message << std::endl;
			
            FindImages(content, pageNum);
			
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
