#-------------------------------------------------
#
# Project created by QtCreator 2014-04-05T11:02:47
#
#-------------------------------------------------

QT += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = kosqlite
TEMPLATE = app


SOURCES += main.cpp\
        frmmainwindow.cpp \
    findlineedit.cpp \
    labelvalue.cpp \
    querysyntaxhighlighter.cpp \
    sqleditor.cpp \
    sqlfile.cpp \
    frmunsaveddocuments.cpp \
    queryexecutionobject.cpp \
    queryvalidator.cpp \
    frmoptions.cpp \
    pages.cpp \
    frmabout.cpp \
    sqltabwidget.cpp \
    downloader.cpp \
    frmupdate.cpp \
    frmrenametable.cpp \
    frmvacuum.cpp \
    frmanalyze.cpp \
    frmimportdata.cpp \
    frmeditdata.cpp \
    frmattachdatabase.cpp \
    frmpragmas.cpp \
    frmblobdetails.cpp \
    kosqlquerymodel.cpp \
    kosqltablemodel.cpp \
    koabstracttablemodel.cpp \
    frmfindandreplace.cpp \
    kolistview.cpp \
    sqlcompleterloader.cpp

HEADERS  += frmmainwindow.h \
    findlineedit.h \
    labelvalue.h \
    querysyntaxhighlighter.h \
    sqleditor.h \
    sqlfile.h \
    frmunsaveddocuments.h \
    queryexecutionobject.h \
    queryvalidator.h \
    frmoptions.h \
    pages.h \
    frmabout.h \
    sqltabwidget.h \
    downloader.h \
    frmupdate.h \
    frmrenametable.h \
    frmvacuum.h \
    frmanalyze.h \
    frmimportdata.h \
    frmeditdata.h \
    frmattachdatabase.h \
    frmpragmas.h \
    frmblobdetails.h \
    kosqlquerymodel.h \
    kosqltablemodel.h \
    koabstracttablemodel.h \
    frmfindandreplace.h \
    kolistview.h \
    sqlcompleterloader.h

FORMS    += frmmainwindow.ui \
    frmunsaveddocuments.ui \
    frmoptions.ui \
    frmabout.ui \
    frmupdate.ui \
    frmrenametable.ui \
    frmvacuum.ui \
    frmanalyze.ui \
    frmimportdata.ui \
    frmeditdata.ui \
    frmattachdatabase.ui \
    frmpragmas.ui \
    frmblobdetails.ui \
    frmfindandreplace.ui

RESOURCES += \
    resursi.qrc

#translations
TRANSLATIONS += i18n/sr_RS.ts
TRANSLATIONS += i18n/sr_RS@latin.ts

RC_FILE = kosqlite.rc

#instalation
target.path = /usr/bin
icon.path = /usr/share/pixmaps
icon.files = icons/kosqlite.png
desktop.path = /usr/share/applications
desktop.files = desktop/kosqlite.desktop
#this is the path of translations, translations will be loaded only from this path on LINUX
trans.path = /usr/share/kosqlite/i18n
trans.files = i18n/*.qm

INSTALLS += target
INSTALLS += icon
INSTALLS += desktop
INSTALLS += trans
