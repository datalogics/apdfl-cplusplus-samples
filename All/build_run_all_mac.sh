#!/bin/bash
## ***
## ***  Copyright (c) 2015-2025, Datalogics, Inc. All rights reserved.
## ***

## **********************************************************************************************************************
## *** Sample: All - Builds and runs each APDFL sample, and outputs the results.
## ***
## *** By default, this occurs with the debug configuration.
## *** Pass in "release" as an argument to use the release configuration.
## *** It is important to note that this program assumes: 
## ***    1. A sample was built successfully <-> Its exe is located in <samplefolder>/<arch>/<stage>/<samplename>.exe
## ***    2. Above, <samplename> is equal to <samplefolder>.
## ***
## *** MAINTENANCE:
## ***   Step 3 is where you want to be if you want to add or ##ove samples from this script.
## ***   Add or ##ove items from DL_SAMPLE_LIST or AD_SAMPLE_LIST, making sure to update NUM_SAMPLES,
## ***   NUM_AD_SAMPLES, NUM_DL_SAMPLES, and NUM_PL_SAMPLES accordingly. Bear in mind that each item in the list must
## ***   be, simultaneously, the name of the sample's folder in ../, the name of the sample's .sln,
## ***   and the name of the executable it builds...
## ***   If necessary, you can also specify arguments for samples. Follow the examples therein.
## ***
## *** USAGE:
## ***   All arguments are case-insensitive.
## ***
## ***   ARGUMENT      EFFECT
## ***   -release      Build Release configuration instead of Debug configuration.
## ***
## *** Steps:
## *** 1) Initialize.
## *** 2) Build each sample.
## *** 3) Decide which samples to run.
## *** 4) Run the samples.
## *** 4) Output the results.
## **********************************************************************************************************************

# Exit immediately if there is an error.
set -e
# initialize variable

NUM_SUCCEED_RUN=0
NUM_FAIL_RUN=0


# Assume some default setting
STAGE=Debug

for var in "$@"
do 
	if [ "$var" == "-release" ]
	then
		STAGE=Release
	fi
done

echo "Configuration: "$STAGE
xcodebuild -project All_Datalogics.xcodeproj -target All_Datalogics -configuration $STAGE CODE_SIGN_IDENTITY="" CODE_SIGNING_REQUIRED=NO
echo $?
if [ "$?" -eq "0" ]; then
    echo "All Sample" $STAGE $ARCH "Build Success"
else
    echo "All Sample" $STAGE $ARCH "Build Failed"
fi

# Sample Name List
declare -a DL_SAMPLE_LIST=( \
  "Annotations/CreateAnnotations" \
  "Annotations/FlattenAnnotations" \
  "ContentCreation/AddArt" \
  "ContentCreation/AddAttachments" \
  "ContentCreation/AddContent" \
  "ContentCreation/AddHeaderFooter" \
  "ContentCreation/CreateBookmarks" \
  "ContentCreation/CreateDocument" \
  "ContentCreation/CreateLayers" \
  "ContentCreation/CreateTransparency" \
  "ContentExtraction/CopyContent" \
  "ContentExtraction/ExtractAttachments" \
  "ContentExtraction/ExtractFonts" \
  "ContentModification/AddBookmarks" \
  "ContentModification/AddQRCode" \
  "ContentModification/AddDocumentInformation" \
  "ContentModification/AddLinks" \
  "ContentModification/AddPageNumbers" \
  "ContentModification/AttachMimeToPDF" \
  "ContentModification/FlattenTransparency" \
  "ContentModification/MergeAcroforms" \
  "ContentModification/MergeDocuments" \
  "ContentModification/PDFMakeOCGVisible" \
  "ContentModification/PDFUncompress" \
  "ContentModification/SplitPDF" \
  "ContentModification/ImportPages" \
  "ContentModification/AddTextWatermark" \
  "ContentModification/EmbedFonts" \
  "DocumentOptimization/PDFOptimizer" \
  "DocumentConversion/ConvertToPDFA" \
  "DocumentConversion/ConvertToPDFX" \
  "DocumentConversion/ConvertPDFtoEPS" \
  "DocumentConversion/ConvertPDFtoPostscript" \
  "FileSystem/AlternateFileSystem" \
  "Images/AddThumbnailsToPDF" \
  "Images/RenderPage" \
  "Images/CreateImageWithTransparency" \
  "Images/CreateSeparations" \
  "Images/CalcImageDPI" \
  "Images/FindImageResolutions" \
  "Images/ImageExport" \
  "Images/ImageImport" \
  "Images/OutputPreview" \
  "InformationExtraction/CountColorsInDoc" \
  "InformationExtraction/ExtractDocumentInfo" \
  "OCR/OCRImage" \
  "Printing/PostScriptInjection" \
  "Security/AESEncryption" \
  "Security/AddDigitalSignatureCMS" \
  "Security/AddDigitalSignatureRFC3161" \
  "Security/AddPassword" \
  "Security/AddRedaction" \
  "Security/EncryptDocument" \
  "Security/LockDocument" \
  "Security/SetUniquePermissions" \
  "Security/OpenEncrypted" \
  "Security/AddRegexRedaction" \
  "Text/InsertHeadFoot" \
  "Text/ExtractText" \
  "Text/TextSelectEnum" \
  "Text/AddText" \
  "Text/UnicodeText" \
  "Text/HelloJapan" \
  "Text/TextSearch" \
  "Text/TextSelectEnum" \
  "Text/RegexTextSearch" \
  "Text/RegexExtractText" \
  "Text/ExtractTextByPatternMatch" \
  "Text/ExtractCJKTextByPatternMatch" \
  "Text/ExtractTextByRegion" \
  "Text/ExtractTextFromMultiRegions" \
  "Text/ExtractAcroFormFieldData" \
  "Text/ExtractTextFromAnnotations"
)

cd ..

## now loop through the DL Samples
for i in "${DL_SAMPLE_LIST[@]}"
do
   echo "$i"
   cd "$i"
   # Isolate the sample name from the group.
   TEMP="$i"
   SAMPLE_NAME=$(echo $TEMP | cut -d'/' -f 2)
   ./"$SAMPLE_NAME"-${STAGE}.app/Contents/MacOS/"$SAMPLE_NAME"-${STAGE}
   # check the status
   if [ "$?" -eq "0" ]; then
   	   NUM_SUCCEED_RUN=$(( $NUM_SUCCEED_RUN + 1 ))
   else
   	   NUM_FAIL_RUN=$(( $NUM_FAIL_RUN + 1))
   fi
   cd ../../
done

echo "Succeed Run number: " $NUM_SUCCEED_RUN

echo "Failed Run number: " $NUM_FAIL_RUN

echo All samples are finished running.

exit $NUM_FAIL_RUN
