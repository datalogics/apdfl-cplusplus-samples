#!/bin/bash

# Exit immediately if there is an error.
set -e

if [ "$1" != "" ]
then
	echo "$0: Parameter 1 is $1."
fi
# Accept an input parameter that chooses the build stage,
# Debug or Release.  Default to Debug like Windows.
if [ "$1" == "Release" ] || [ "$1" == "release" ]
then
	stage=Release
else
	stage=Debug
fi
echo "$0: Setting stage to $stage."
echo ""

# Annotations

echo Running sample CreateAnnotations
cd ../Annotations/CreateAnnotations
./CreateAnnotations-${stage}.app/Contents/MacOS/CreateAnnotations-${stage}
echo ""

echo Running sample FlattenAnnotations
cd ../../Annotations/FlattenAnnotations
./FlattenAnnotations-${stage}.app/Contents/MacOS/FlattenAnnotations-${stage}
echo ""


# ContentCreation

echo Running sample AddArt
cd ../../ContentCreation/AddArt
./AddArt-${stage}.app/Contents/MacOS/AddArt-${stage}
echo ""

echo Running sample AddAttachments
cd ../../ContentCreation/AddAttachments
./AddAttachments-${stage}.app/Contents/MacOS/AddAttachments-${stage}
echo ""

echo Running sample AddContent
cd ../../ContentCreation/AddContent
./AddContent-${stage}.app/Contents/MacOS/AddContent-${stage}
echo ""

echo Running sample AddHeaderFooter
cd ../../ContentCreation/AddHeaderFooter
./AddHeaderFooter-${stage}.app/Contents/MacOS/AddHeaderFooter-${stage}
echo ""

echo Running sample CreateBookmarks
cd ../../ContentCreation/CreateBookmarks
./CreateBookmarks-${stage}.app/Contents/MacOS/CreateBookmarks-${stage}
echo ""

echo Running sample CreateDocument
cd ../../ContentCreation/CreateDocument
./CreateDocument-${stage}.app/Contents/MacOS/CreateDocument-${stage}
echo ""

echo Running sample CreateLayers
cd ../../ContentCreation/CreateLayers
./CreateLayers-${stage}.app/Contents/MacOS/CreateLayers-${stage}
echo ""

echo Running sample CreateTransparency
cd ../../ContentCreation/CreateTransparency
./CreateTransparency-${stage}.app/Contents/MacOS/CreateTransparency-${stage}
echo ""

# ContentExtraction

echo Running sample CopyContent
cd ../../ContentExtraction/CopyContent
./CopyContent-${stage}.app/Contents/MacOS/CopyContent-${stage}
echo ""

echo Running sample ExtractAttachments
cd ../../ContentExtraction/ExtractAttachments
./ExtractAttachments-${stage}.app/Contents/MacOS/ExtractAttachments-${stage}
echo ""

echo Running sample ExtractFonts
cd ../../ContentExtraction/ExtractFonts
./ExtractFonts-${stage}.app/Contents/MacOS/ExtractFonts-${stage}
echo ""

# ContentModification

echo Running sample AddBookmarks
cd ../../ContentModification/AddBookmarks
./AddBookmarks-${stage}.app/Contents/MacOS/AddBookmarks-${stage}
echo ""

echo Running sample AddQRCode
cd ../../ContentModification/AddQRCode
./AddQRCode-${stage}.app/Contents/MacOS/AddQRCode-${stage}
echo ""

echo Running sample AddDocumentInformation
cd ../../ContentModification/AddDocumentInformation
./AddDocumentInformation-${stage}.app/Contents/MacOS/AddDocumentInformation-${stage}
echo ""

echo Running sample AddLinks
cd ../../ContentModification/AddLinks
./AddLinks-${stage}.app/Contents/MacOS/AddLinks-${stage}
echo ""

echo Running sample AddPageNumbers
cd ../../ContentModification/AddPageNumbers
./AddPageNumbers-${stage}.app/Contents/MacOS/AddPageNumbers-${stage}
echo ""

echo Running sample AttachMimeToPDF
cd ../../ContentModification/AttachMimeToPDF
./AttachMimeToPDF-${stage}.app/Contents/MacOS/AttachMimeToPDF-${stage}
echo ""

echo Running sample FlattenTransparency
cd ../../ContentModification/FlattenTransparency
./FlattenTransparency-${stage}.app/Contents/MacOS/FlattenTransparency-${stage}
echo ""

echo Running sample MergeAcroforms
cd ../../ContentModification/MergeAcroforms
./MergeAcroforms-${stage}.app/Contents/MacOS/MergeAcroforms-${stage}
echo ""

echo Running sample MergeDocuments
cd ../../ContentModification/MergeDocuments
./MergeDocuments-${stage}.app/Contents/MacOS/MergeDocuments-${stage}
echo ""

echo Running sample PDFMakeOCGVisible
cd ../../ContentModification/PDFMakeOCGVisible
./PDFMakeOCGVisible-${stage}.app/Contents/MacOS/PDFMakeOCGVisible-${stage}
echo ""

echo Running sample PDFUncompress
cd ../../ContentModification/PDFUncompress
./PDFUncompress-${stage}.app/Contents/MacOS/PDFUncompress-${stage}
echo ""

echo Running sample SplitPDF
cd ../../ContentModification/SplitPDF
./SplitPDF-${stage}.app/Contents/MacOS/SplitPDF-${stage}
echo ""

echo Running sample ImportPages
cd ../../ContentModification/ImportPages
./ImportPages-${stage}.app/Contents/MacOS/ImportPages-${stage}
echo ""

echo Running sample AddTextWatermark
cd ../../ContentModification/AddTextWatermark
./AddTextWatermark-${stage}.app/Contents/MacOS/AddTextWatermark-${stage}
echo ""

echo Running sample EmbedFonts
cd ../../ContentModification/EmbedFonts
./EmbedFonts-${stage}.app/Contents/MacOS/EmbedFonts-${stage}
echo ""

# Display

echo Running sample DisplayPDEContent
cd ../../Display/DisplayPDEContent
./DisplayPDEContent-${stage}.app/Contents/MacOS/DisplayPDEContent-${stage}
echo ""

# DocumentOptimization

echo Running sample PDFOptimizer
cd ../../DocumentOptimization/PDFOptimizer
./PDFOptimizer-${stage}.app/Contents/MacOS/PDFOptimizer-${stage}
echo ""

# DocumentConversion

echo Running sample ConvertPDFtoEPS
cd ../../DocumentConversion/ConvertPDFtoEPS
./ConvertPDFtoEPS-${stage}.app/Contents/MacOS/ConvertPDFtoEPS-${stage}
echo ""

echo Running sample ConvertToPDFA
cd ../../DocumentConversion/ConvertToPDFA
./ConvertToPDFA-${stage}.app/Contents/MacOS/ConvertToPDFA-${stage}
echo ""

echo Running sample ConvertToPDFX
cd ../../DocumentConversion/ConvertToPDFX
./ConvertToPDFX-${stage}.app/Contents/MacOS/ConvertToPDFX-${stage}
echo ""

echo Running sample ConvertPDFtoPostscript
cd ../../DocumentConversion/ConvertPDFtoPostscript
./ConvertPDFtoPostscript-${stage}.app/Contents/MacOS/ConvertPDFtoPostscript-${stage}
echo ""

# FileSystem

echo Running sample AlternateFileSystem
cd ../../FileSystem/AlternateFileSystem
./AlternateFileSystem-${stage}.app/Contents/MacOS/AlternateFileSystem-${stage}
echo ""

# Images

echo Running sample AddThumbnailsToPDF
cd ../../Images/AddThumbnailsToPDF
./AddThumbnailsToPDF-${stage}.app/Contents/MacOS/AddThumbnailsToPDF-${stage}
echo ""

echo Running sample RenderPage
cd ../../Images/RenderPage
./RenderPage-${stage}.app/Contents/MacOS/RenderPage-${stage}
echo ""

echo Running sample CreateImageWithTransparency
cd ../../Images/CreateImageWithTransparency
./CreateImageWithTransparency-${stage}.app/Contents/MacOS/CreateImageWithTransparency-${stage}
echo ""

echo Running sample CreateSeparations
cd ../../Images/CreateSeparations
./CreateSeparations-${stage}.app/Contents/MacOS/CreateSeparations-${stage}
echo ""

echo Running sample CalcImageDPI
cd ../../Images/CalcImageDPI
./CalcImageDPI-${stage}.app/Contents/MacOS/CalcImageDPI-${stage}
echo ""

echo Running sample FindImageResolutions
cd ../../Images/FindImageResolutions
./FindImageResolutions-${stage}.app/Contents/MacOS/FindImageResolutions-${stage}
echo ""

echo Running sample OutputPreview
cd ../../Images/OutputPreview
./OutputPreview-${stage}.app/Contents/MacOS/OutputPreview-${stage}
echo ""

echo Running sample ImageExport
cd ../../Images/ImageExport
./ImageExport-${stage}.app/Contents/MacOS/ImageExport-${stage}
echo ""

echo Running sample ImageImport
cd ../../Images/ImageImport
./ImageImport-${stage}.app/Contents/MacOS/ImageImport-${stage}
echo ""

# InformationExtraction

echo Running sample CountColorsInDoc
cd ../../InformationExtraction/CountColorsInDoc
./CountColorsInDoc-${stage}.app/Contents/MacOS/CountColorsInDoc-${stage}
echo ""

echo Running sample ExtractDocumentInfo
cd ../../InformationExtraction/ExtractDocumentInfo
./ExtractDocumentInfo-${stage}.app/Contents/MacOS/ExtractDocumentInfo-${stage}
echo ""

# OCR

echo Running sample OCRImage
cd ../../OCR/OCRImage
./OCRImage-${stage}.app/Contents/MacOS/OCRImage-${stage}
echo ""

echo Running sample OCRPage
cd ../../OCR/OCRPage
./OCRPage-${stage}.app/Contents/MacOS/OCRPage-${stage}
echo ""

# Printing

echo Running sample PostScriptInjection
cd ../../Printing/PostScriptInjection
./PostScriptInjection-${stage}.app/Contents/MacOS/PostScriptInjection-${stage}
echo ""

echo Running sample PDFPrintDefault
cd ../../Printing/PDFPrintDefault
./PDFPrintDefault-${stage}.app/Contents/MacOS/PDFPrintDefault-${stage}
echo ""

# Security

echo Running sample AESEncryption
cd ../../Security/AESEncryption
./AESEncryption-${stage}.app/Contents/MacOS/AESEncryption-${stage}
echo ""

echo Running sample AddDigitalSignatureCMS
cd ../../Security/AddDigitalSignatureCMS
./AddDigitalSignatureCMS-${stage}.app/Contents/MacOS/AddDigitalSignatureCMS-${stage}
echo ""

echo Running sample AddDigitalSignatureRFC3161
cd ../../Security/AddDigitalSignatureRFC3161
./AddDigitalSignatureRFC3161-${stage}.app/Contents/MacOS/AddDigitalSignatureRFC3161-${stage}
echo ""

echo Running sample AddPassword
cd ../../Security/AddPassword
./AddPassword-${stage}.app/Contents/MacOS/AddPassword-${stage}
echo ""

echo Running sample AddRedaction
cd ../../Security/AddRedaction
./AddRedaction-${stage}.app/Contents/MacOS/AddRedaction-${stage}
echo ""

echo Running sample AddTriangularRedaction
cd ../../Security/AddTriangularRedaction
./AddTriangularRedaction-${stage}.app/Contents/MacOS/AddTriangularRedaction-${stage}
echo ""

echo Running sample EncryptDocument
cd ../../Security/EncryptDocument
./EncryptDocument-${stage}.app/Contents/MacOS/EncryptDocument-${stage}
echo ""

echo Running sample LockDocument
cd ../../Security/LockDocument
./LockDocument-${stage}.app/Contents/MacOS/LockDocument-${stage}
echo ""

echo Running sample SetUniquePermissions
cd ../../Security/SetUniquePermissions
./SetUniquePermissions-${stage}.app/Contents/MacOS/SetUniquePermissions-${stage}
echo ""

echo Running sample OpenEncrypted
cd ../../Security/OpenEncrypted
./OpenEncrypted-${stage}.app/Contents/MacOS/OpenEncrypted-${stage}
echo ""

echo Running sample AddRegexRedaction
cd ../../Security/AddRegexRedaction
./AddRegexRedaction-${stage}.app/Contents/MacOS/AddRegexRedaction-${stage}
echo ""

# Text

echo Running sample InsertHeadFoot
cd ../../Text/InsertHeadFoot
./InsertHeadFoot-${stage}.app/Contents/MacOS/InsertHeadFoot-${stage}
echo ""

echo Running sample ExtractText
cd ../../Text/ExtractText
./ExtractText-${stage}.app/Contents/MacOS/ExtractText-${stage}
echo ""

echo Running sample AddText
cd ../../Text/AddText
./AddText-${stage}.app/Contents/MacOS/AddText-${stage}
echo ""

echo Running sample UnicodeText
cd ../../Text/UnicodeText
./UnicodeText-${stage}.app/Contents/MacOS/UnicodeText-${stage}
echo ""

echo Running sample HelloJapan
cd ../../Text/HelloJapan
./HelloJapan-${stage}.app/Contents/MacOS/HelloJapan-${stage}
echo ""

echo Running sample TextSearch
cd ../../Text/TextSearch
./TextSearch-${stage}.app/Contents/MacOS/TextSearch-${stage}
echo ""

echo Running sample TextSelectEnum
cd ../../Text/TextSelectEnum
./TextSelectEnum-${stage}.app/Contents/MacOS/TextSelectEnum-${stage}
echo ""

echo Running sample RegexTextSearch
cd ../../Text/RegexTextSearch
./RegexTextSearch-${stage}.app/Contents/MacOS/RegexTextSearch-${stage}
echo ""

echo Running sample RegexExtractText
cd ../../Text/RegexExtractText
./RegexExtractText-${stage}.app/Contents/MacOS/RegexExtractText-${stage}
echo ""

echo Running sample ExtractTextByPatternMatch
cd ../../Text/ExtractTextByPatternMatch
./ExtractTextByPatternMatch-${stage}.app/Contents/MacOS/ExtractTextByPatternMatch-${stage}
echo ""

echo Running sample ExtractCJKTextByPatternMatch
cd ../../Text/ExtractCJKTextByPatternMatch
./ExtractCJKTextByPatternMatch-${stage}.app/Contents/MacOS/ExtractCJKTextByPatternMatch-${stage}
echo ""

echo Running sample ExtractTextByRegion
cd ../../Text/ExtractTextByRegion
./ExtractTextByRegion-${stage}.app/Contents/MacOS/ExtractTextByRegion-${stage}
echo ""

echo Running sample ExtractTextFromMultiRegions
cd ../../Text/ExtractTextFromMultiRegions
./ExtractTextFromMultiRegions-${stage}.app/Contents/MacOS/ExtractTextFromMultiRegions-${stage}
echo ""

echo Running sample ExtractAcroFormFieldData
cd ../../Text/ExtractAcroFormFieldData
./ExtractAcroFormFieldData-${stage}.app/Contents/MacOS/ExtractAcroFormFieldData-${stage}
echo ""

echo Running sample ExtractTextFromAnnotations
cd ../../Text/ExtractTextFromAnnotations
./ExtractTextFromAnnotations-${stage}.app/Contents/MacOS/ExtractTextFromAnnotations-${stage}
echo ""

echo All Datalogics samples are finished running.

