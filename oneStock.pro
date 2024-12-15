QT       += core gui network texttospeech qml multimedia

RC_FILE += app.rc

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
CONFIG += c++17
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-unused-variable -Wunused-but-set-variable
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-unused-variable -Wunused-but-set-variable
QMAKE_CFLAGS += -Wno-unused-variable -Wunused-but-set-variable

#CONFIG += c++17 UTF8_SOURCE debug
#CONFIG += debug_and_release
#CONFIG -= release
#CONFIG += debug
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    NetworkManager.cpp \   
    commondelegate.cpp \
    csv_file.cpp \
    downloadtask.cpp \
    drawchart.cpp \
    f10view.cpp \
    fundflow.cpp \
    getf10info.cpp \
    globalvar.cpp \    
    jspickstock.cpp \
    main.cpp \
    mainwindow.cpp \
    modelfundflow.cpp \
    modeltablestock.cpp \
    modeltimeshare.cpp \
    requeststocsv.cpp \
    searchstock.cpp \
    tablestock.cpp \
    tech/BOLL.cpp \
    tech/KDJ.cpp \
    tech/KData.cpp \
    tech/MA.cpp \
    tech/MACD.cpp \
    tech/Tech.cpp \
    tech/Technique.cpp \
    tech/VOLUME.cpp \
    threadcandlechart.cpp \
    threadindex.cpp \
    threadnewsreport.cpp \
    threadtable.cpp \
    threadtimesharechart.cpp \
    threadtimesharetick.cpp \
    time_util.cpp \
    timeutil.cpp \
    ttscfgdlg.cpp \
    utilityex.cpp \


HEADERS += \
    NetworkManager.h \
    commondelegate.h \
    csv_file.h \
    downloadtask.h \
    drawchart.h \
    f10view.h \
    fundflow.h \
    getf10info.h \
    globalvar.h \
    gzip/compress.hpp \
    gzip/config.hpp \
    gzip/decompress.hpp \
    gzip/utils.hpp \
    gzip/version.hpp \
    include/Intst.h \
    include/StructKlineView.h \
    include/define.h \
    include/enums.h \
    include/fx_quote_def.hpp \
    jspickstock.h \
    mainwindow.h \
    modelfundflow.h \
    modeltablestock.h \
    modeltimeshare.h \
    requeststocsv.h \
    searchstock.h \
    stockinfo.h \
    tablestock.h \
    tech/BOLL.h \
    tech/Express.h \
    tech/KDJ.h \
    tech/KData.h \
    tech/MA.h \
    tech/MACD.h \
    tech/TechLib.h \
    tech/Technique.h \
    tech/VOLUME.h \
    threadcandlechart.h \
    threadindex.h \
    threadnewsreport.h \
    threadtable.h \
    threadtimesharechart.h \
    threadtimesharetick.h \
    time_util.h \
    ttscfgdlg.h \
    utilityex.h \

FORMS += \
    TextToSpeech.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

INCLUDEPATH += d:\Python39\include C:\Qt\Tools\OpenSSLv3\Win_x64\include D:\work\stk-src\oneStock\jsoncpp-src-0.5.0\include
LIBS +=-lwsock32 -lUser32 -Ld:\Python39\libs -lpython39  -LC:\Qt\Tools\OpenSSLv3\Win_x64\lib -llibssl -llibcrypto

#INCLUDEPATH += d:\Python311\include  d:\Qt\Tools\OpenSSLv3\Win_x64\include D:\oneStock\jsoncpp-src-0.5.0\include
#LIBS +=-lwsock32 -lUser32 -Ld:\Python311\libs -lpython311  -Ld:\Qt\Tools\OpenSSLv3\Win_x64\lib -llibssl -llibcrypto

#QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
#LIBS += -lDbgHelp

DISTFILES += \
    qmt.py
