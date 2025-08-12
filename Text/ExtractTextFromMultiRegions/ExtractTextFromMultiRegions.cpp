//
// Copyright (c) 2023-2025, Datalogics, Inc. All rights reserved.
//
//
// This sample processes PDF files in a folder and extracts text from specific regions
// of its pages and saves the text to a file.
//

#include <fstream>
#include <string>

#include "InitializeLibrary.h"
#include "APDFLDoc.h"
#include "TextExtract.h"

std::string DEF_INPUT_DIR = "../../../../Resources/Sample_Input/ExtractTextFromMultiRegions/";
std::vector<std::string> DEF_INPUTS = { "Bing1312.pdf", "Fakeserv139203.pdf", "Fakeserv139599.pdf", "Huge112.pdf", "Huge1122.pdf" };

const char *DEF_OUTPUT = "ExtractTextFromMultiRegions-out.csv";

// Rectangular regions to extract text in points (origin of the page is bottom left)
// (Left, Right, Bottom, Top)
std::vector<int> invoice_number{500, 590, 692, 710};
std::vector<int> date{500, 590, 680, 700};
std::vector<int> order_number{500, 590, 672, 688};
std::vector<int> cust_id{500, 590, 636, 654};
std::vector<int> total{500, 590, 52, 73};

std::vector<std::vector<int>> regions{invoice_number, date, order_number, cust_id, total};

enum { left, right, bottom, top };

bool CheckWithinRegion(DLQuadFloat wordQuad, std::vector<int> region);

int main(int argc, char **argv) {

    // Initialize the library
    APDFLib libInit;
    ASErrorCode errCode = 0;
    if (libInit.isValid() == false) {
        errCode = libInit.getInitError();
        std::cout << "Initialization failed with code " << errCode << std::endl;
        return libInit.getInitError();
    }

    DURING
        std::ofstream outputFile(DEF_OUTPUT);

        // Print out a heading for CSV file
        outputFile << "Filename,Invoice Number,Date,Order Number,Customer ID,Total" << std::endl;
        for (int stringIndex = 0; stringIndex < DEF_INPUTS.size(); stringIndex++) {
            std::string inputFile = DEF_INPUT_DIR + DEF_INPUTS[stringIndex];
            outputFile << DEF_INPUTS[stringIndex];
            {
                APDFLDoc inAPDoc(inputFile.c_str(), true);

                TextExtract textExtract(inAPDoc.getPDDoc());

                std::vector<PDTextAndDetailsExtractRec> extractedText = textExtract.GetTextAndDetails();

                for (auto &region : regions) {
                    outputFile << ",";
                    for (size_t textIndex = 0; textIndex < extractedText.size(); ++textIndex) {
                        bool allQuadsWithinRegion = true;
                        // A Word typically has only 1 quad, but can have more than one
                        // for hyphenated words, words on a curve, etc.
                        for (size_t quadIndex = 0;
                             quadIndex < extractedText[textIndex].boundingQuads.size(); ++quadIndex) {

                            DLQuadFloat wordQuad = extractedText[textIndex].boundingQuads[quadIndex];
                            if (!CheckWithinRegion(wordQuad, region)) {
                                allQuadsWithinRegion = false;
                                break;
                            }
                        }
                        if (allQuadsWithinRegion) {
                            // Put this Word that meets our criteria in the output document
                            outputFile << extractedText[textIndex].text;
                        }
                    }
                }
            }
            outputFile << std::endl;
        }
        outputFile.close();
    HANDLER
        errCode = ERRORCODE;
        libInit.displayError(errCode);
    END_HANDLER

    return errCode;
};

// For this sample, we will consider a Word to be in the region of interest if the
// complete Word fits within the specified rectangular box
bool CheckWithinRegion(DLQuadFloat wordQuad, std::vector<int> region) {

    if (wordQuad.bl.h >= region[left] && wordQuad.br.h <= region[right] &&
        wordQuad.tl.h >= region[left] && wordQuad.tr.h <= region[right] &&
        wordQuad.bl.v >= region[bottom] && wordQuad.tl.v <= region[top] &&
        wordQuad.br.v >= region[bottom] && wordQuad.tr.v <= region[top]) {

        return true;
    }
    return false;
}
