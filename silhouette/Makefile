SOURCE_FOLDER	:= ./silhouette
DATA_FOLDER	:= ./data
SCRIPT_FOLDER	:= ./scripts
ENTRY_POINT	:= $(SOURCE_FOLDER)/Source.cpp
TARGET		:= ./build/
REPORTS_PATH	:= ./reports/
BASE_OPTIONS	:= -std=c++14 -qopenmp -O3 -opt-report-phase=all -I $(SOURCE_FOLDER) -ipo -parallel
WARNING_OPTIONS	:= -Who-unknown-pragmas -Wall -Warray-bounds -Wchar-subscripts -Wcomment -Wenum-compare -Wformat -Wuninitialized -Wmaybe-uninitialized -Wmain -Wnarrowing -Wnonnull -Wparentheses -Wpointer-sign -Wreorder -Wreturn-type -Wsign-compare -Wsequence-point -Wtrigraphs -Wunused-function -Wunused-but-set-variable -Wunused-variable -Wwrite-strings
PHI_OPTIONS	:= -mmic -fsource-asm -qopt-report-file=$(REPORTS_PATH)phi.optrpt
XEON_OPTIONS	:= -xSSE4.2 -qopt-report-file=$(REPORTS_PATH)xeon.optrpt -S

all: folders node mic files

phi: folders mic files

xeon: folders node files	
	
mic:
	icpc $(ENTRY_POINT) -o $(TARGET)SilhouettePhi.mic $(BASE_OPTIONS) $(WARNING_OPTIONS) $(PHI_OPTIONS)	
	
node:
	icpc $(ENTRY_POINT) -o $(TARGET)SilhouettePhi $(BASE_OPTIONS) $(WARNING_OPTIONS) $(XEON_OPTIONS)	
	
folders:
	mkdir -p $(TARGET)
	mkdir -p $(REPORTS_PATH)	
	
files:
	cp -a -n $(DATA_FOLDER)/. $(TARGET)
	cp -a -n $(SCRIPT_FOLDER)/. $(TARGET)
