/*
 * This sample program creates an empty PDF document with one page and imports the specified image file.
 * This image is then modified with included API's: DLPDEImageRotate(), DLPDEImageTranslate(), DLPDEImageScale(). 
 * This progam also utilizes DLPDEImageGetWidth() and DLPDEImageGetHeight() to assis in translating the image withing the page.
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

// In this scenario the Image object is used alone to create a
// new PDF page with the image as the content.
int main(int argc, char** argv) 
{
    std::cout << "Initialized the library." << std::endl;
    APDFLib libInit;
    if (libInit.isValid() == false) {
        std::cout << "Initialization failed with code " << libInit.getInitError() << std::endl;
        return libInit.getInitError();
    }

    ASPathName sInput = APDFLDoc::makePath("Sample_Input/ducky.jpg";
    ASPathName sOutput = APDFLDoc::makePath("ImageImport-ducky.pdf");
    
    DURING
		// Create an empty pdf document.
        PDDoc doc = PDDocCreate();
		
        char message[1024];
		sprintf(message, "Reading image file %s and writing %s", sInput, sOutput);
        std::cout << message << std::endl;

        // Convert the input file into a PDEImage.
        PDEImage newimage = DLCreatePDEImageFromFile(doc, sInput, NULL);

        // Create a PDF page which is one inch larger all around than this image
        // The design width and height for the image are carried in the
        // matrix.a and matrix.d fields, respectively.
        // There are 72 PDF user space units in one inch.
        ASDoubleMatrix matrix = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        PDEElementGetMatrixEx((PDEElement)newimage, &matrix);

        ASFixedRect bbox;
        bbox.left = 0;
        bbox.top = FloatToASFixed(matrix.d + 144);
        bbox.right = FloatToASFixed(matrix.a + 144);
        bbox.bottom = 0;

        PDPage page = PDDocCreatePage(doc, PDDocGetNumPages(doc) - 1, bbox);

        // Center the image in its bounding box.
        DLPDEImageTranslate(&newimage, 72, 72);

        // Manimulate the image by rotating and scaling it.
        DLPDEImageRotate(&newimage, 180);
        DLPDEImageScale(&newimage, 0.5, 0.5);

        // Recenter the image in its bounding box accounting for scale and rotation.
        DLPDEImageTranslate(&newimage, DLPDEImageGetWidth(&newimage) * -1.5, DLPDEImageGetHeight(&newimage) * -1.5);

        // Insert the PDEImage into the page.
        PDEContent content = PDPageAcquirePDEContent(page, 0);
        PDEContentAddElem(content, 0, reinterpret_cast<PDEElement>(newimage));

        // Set the update PDE Content into the page and release it after it is used.
        PDPageSetPDEContentCanRaise(page, 0);
        PDPageReleasePDEContent(page, 0);
        PDPageRelease(page);

        // Save and close the output document.
        PDDocSave(doc, PDSaveFull, sOutput, NULL, NULL, NULL);
        PDDocClose(doc);

        // Release remaining resources. 
        PDERelease(reinterpret_cast<PDEObject>(newimage));
        ASFileSysReleasePath(NULL, sInput);
        ASFileSysReleasePath(NULL, sOutput);

    HANDLER
        libInit.displayError(ERRORCODE);
        return ERRORCODE;
    END_HANDLER

    return 0;
}