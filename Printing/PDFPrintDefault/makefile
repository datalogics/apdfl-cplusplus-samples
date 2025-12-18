SAMPNAME=PDFPrintDefault
OTHER_OBJS = $(SAMPNAME).o SetupPrintParams.o

include ../../dlutils/common.mak

SetupPrintParams.o : $(COMMON)/SetupPrintParams.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(SAMPNAME).o: $(SRC)/$(SAMPNAME).cpp
