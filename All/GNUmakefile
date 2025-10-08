#  Makefile to build DL samples

ifeq ($(OS), )
  PLATFORM = $(shell uname)
  MACHINE = $(shell uname -m)
  BUILD_64_BIT=true
  ifeq ($(PLATFORM), Linux)
    ifeq ($(MACHINE), x86_64)
      OS=i80386linux
    endif
    ifeq ($(MACHINE), aarch64)
      OS=armv8linux
    endif
  endif
  ifeq ($(PLATFORM), Darwin)
    ifeq ($(MACHINE), x86_64)
      OS=mac-x86-64
    endif
    ifeq ($(MACHINE), arm64)
      OS=armv8mac
    endif
  endif
  ifeq ($(PLATFORM), AIX)
    OS=rs6000aix
  endif
  ifeq ($(PLATFORM), SunOS)
    OS=sparcsolaris
  endif
endif


do_pdfl_samples: build_pdfl_samples run_pdfl_samples $(REDIRECT2)

SAMPLES = Annotations/CreateAnnotations \
          Annotations/FlattenAnnotations \
          ContentCreation/AddArt \
          ContentCreation/AddAttachments \
          ContentCreation/AddContent \
          ContentCreation/AddHeaderFooter \
          ContentCreation/CreateBookmarks \
          ContentCreation/CreateDocument \
          ContentCreation/CreateLayers \
          ContentCreation/CreateTransparency \
          ContentExtraction/CopyContent \
          ContentExtraction/ExtractAttachments \
          ContentExtraction/ExtractFonts \
          ContentModification/AddBookmarks \
          ContentModification/AddQRCode \
          ContentModification/AddDocumentInformation \
          ContentModification/AddLinks \
          ContentModification/AddPageNumbers \
          ContentModification/AddTextWatermark \
          ContentModification/AttachMimeToPDF \
          ContentModification/EmbedFonts \
          ContentModification/FlattenTransparency \
          ContentModification/ImportPages \
          ContentModification/MergeAcroforms \
          ContentModification/MergeDocuments \
          ContentModification/PDFMakeOCGVisible \
          ContentModification/PDFUncompress \
          ContentModification/SplitPDF \
          Display/DisplayPDEContent \
          DocumentConversion/ConvertPDFtoEPS \
          DocumentConversion/ConvertToOffice \
          DocumentConversion/ConvertPDFtoPostscript \
          DocumentConversion/ConvertToPDFA \
          DocumentConversion/ConvertToPDFX \
          DocumentOptimization/PDFOptimizer \
          FileSystem/AlternateFileSystem \
          Images/AddThumbnailsToPDF \
          Images/CalcImageDPI \
          Images/CreateImageWithTransparency \
          Images/CreateSeparations \
          Images/FindImageResolutions \
          Images/ImageExport \
          Images/ImageImport \
          Images/OutputPreview \
          Images/RenderPage \
          InformationExtraction/CountColorsInDoc \
          InformationExtraction/ExtractDocumentInfo \
          Printing/PostScriptInjection \
          Security/AddDigitalSignatureCMS \
          Security/AddDigitalSignatureRFC3161 \
          Security/AddPassword \
          Security/AddRedaction \
          Security/AddRegexRedaction \
          Security/AddTriangularRedaction \
          Security/AESEncryption \
          Security/EncryptDocument \
          Security/LockDocument \
          Security/OpenEncrypted \
          Security/SetUniquePermissions \
          Text/AddText \
          Text/ExtractAcroFormFieldData \
          Text/ExtractCJKTextByPatternMatch \
          Text/ExtractText \
          Text/ExtractTextByPatternMatch \
          Text/ExtractTextByRegion \
          Text/ExtractTextFromAnnotations \
          Text/ExtractTextFromMultiRegions \
          Text/ExtractTextPreservingStyleAndPositionInfo \
          Text/HelloJapan \
          Text/InsertHeadFoot \
          Text/RegexTextSearch \
          Text/RegexExtractText \
          Text/TextSearch \
          Text/TextSelectEnum \
          Text/UnicodeText

ifeq ($(findstring rs6000aix,$(OS)), )
ifeq ($(findstring mac,$(OS)), )
    SAMPLES +=  DocumentConversion/XPStoPDF
endif
endif

PATH_TO_FORMS = ../Forms
ifneq ("$(wildcard $(PATH_TO_FORMS))","")
SAMPLES += Forms/ConvertXFAToAcroForms
    SAMPLES += Forms/ExportFormsData
    SAMPLES += Forms/FlattenForms
    SAMPLES += Forms/ImportFormsData
endif

BUILD_ONLY_SAMPLES = $(SAMPLES)

ifneq ($(findstring mac,$(OS)), )
    BUILD_ONLY_SAMPLES += Printing/PDFPrintDefault
    BUILD_ONLY_SAMPLES += Printing/PDFPrintGUI
endif

build_pdfl_samples:
	$(info OS=$(OS))
	$(info BUILD_64_BIT=$(BUILD_64_BIT))
	set -e; \
	export OS=$(OS) \
	export BUILD_64_BIT=$(BUILD_64_BIT) \
	export; \
	for target in $(BUILD_ONLY_SAMPLES); do \
		(cd ../$$target && $(MAKE)); \
	done;

ifeq ($(LIBPATHS),)
    LIBPATHS=../../../Binaries
endif

run_pdfl_samples:
	set -e; \
	export LD_LIBRARY_PATH=$(LIBPATHS):$$LD_LIBRARY_PATH; \
	export; \
	for target in $(SAMPLES); do \
		echo "Running DL Sample $$target"; \
		export NAME=`echo "$$target" | cut -d'/' -f 2`; \
		(cd ../$$target && (./$$NAME $(REDIRECT))) ;\
		echo ; \
	done; \
	echo ;

clean:
	rm -f *.o *.out out.* ../dlutils/*.o; \
	for target in $(BUILD_ONLY_SAMPLES); do \
		(cd ../$$target && $(MAKE) clean); \
	done;
