##############################################
# please add new classes in alphabetical order

AUTOMAKE_OPTIONS = foreign

# List of shared libraries to produce
lib_LTLIBRARIES = \
  libg4calo.la

AM_CXXFLAGS = `geant4-config --cflags`

AM_CPPFLAGS = \
  -I$(includedir) \
  -isystem$(OFFLINE_MAIN)/include \
  -isystem$(ROOTSYS)/include

pkginclude_HEADERS = \
  g4hitshift.h \
  g4hitshifthcal.h \
  HcalRawTowerBuilder.h \
  RawTowerBuilder.h \
  RawTowerBuilderByHitIndex.h \
  RawTowerDigitizer.h 

libg4calo_la_SOURCES = \
  g4hitshift.cc \
  g4hitshifthcal.cc \
  HcalRawTowerBuilder.cc \
  RawTowerBuilder.cc \
  RawTowerBuilderByHitIndex.cc \
  RawTowerDigitizer.cc 

libg4calo_la_LDFLAGS = \
  -L$(libdir) \
  -L$(OFFLINE_MAIN)/lib

libg4calo_la_LIBADD = \
  -lcalo_io \
  -lcdbobjects \
  -lfun4all \
  -lg4detectors_io \
  -lg4testbench \
  -lgsl \
  -lgslcblas \
  -lphool \
  -lphg4hit \
  -lphparameter \
  -lSubsysReco

################################################
# linking tests

noinst_PROGRAMS = \
  testexternals_g4calo

BUILT_SOURCES = testexternals.cc

testexternals_g4calo_SOURCES = testexternals.cc
testexternals_g4calo_LDADD = libg4calo.la

testexternals.cc:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

##############################################
# please add new classes in alphabetical order

clean-local:
	rm -f $(BUILT_SOURCES)
