# Makefile.am for qmidiroute
SUBDIRS = pixmaps

bin_PROGRAMS = qmidiroute

nodist_qmidiroute_SOURCES = \
	mainwindow_moc.cpp \
	logwidget_moc.cpp \
	mapdata_moc.cpp \
	mapwidget_moc.cpp \
	midimap_moc.cpp \
	passwidget_moc.cpp \
	seqdriver_moc.cpp

qmidiroute_SOURCES = \
	mainwindow.cpp mainwindow.h \
	logwidget.cpp logwidget.h \
 	main.cpp main.h \
	mapdata.cpp mapdata.h \
	mapwidget.cpp mapwidget.h \
	midimap.cpp midimap.h \
	passwidget.cpp passwidget.h \
	seqdriver.cpp seqdriver.h


translationsdir = $(pkgdatadir)/translations
translations = \
	translations/qmidiroute_de.ts \
	translations/qmidiroute_fr.ts \
	translations/qmidiroute_ru.ts

translations_DATA = \
	translations/qmidiroute_de.qm \
	translations/qmidiroute_fr.qm \
	translations/qmidiroute_ru.qm


AM_LDFLAGS = @QT_LIBS@ @DEFS@
AM_CXXFLAGS = @QT_CXXFLAGS@
DEFS = -Wall -Wextra -D_REENTRANT -DTRANSLATIONSDIR=\"$(translationsdir)\" @DEFS@

# misc files which are distributed but not installed
EXTRA_DIST = qmidiroute.pro $(translations)

# all generated files to be removed by "make clean"
CLEANFILES = $(nodist_qmidiroute_SOURCES)  $(translations_DATA)

# rule for moc-file generation
SUFFIXES = _moc.cpp

.h_moc.cpp: $(qmidiroute_SOURCES)
	$(MOC) $< -o $@

# rule for translation file updates
$(translations): $(qmidiroute_SOURCES)
	$(LUPDATE) -verbose $(qmidiroute_SOURCES) -ts $@

# rule to generate binary translation files
.ts.qm: $(translations)
	$(LRELEASE) -verbose $< -qm $@

