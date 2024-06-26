QT += widgets
QT += charts
QT += multimedia

CONFIG += c++11 #console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../chiptune.c \
        AudioPlayer.cpp \
        HardwareChiptunePanelWidget.cpp \
        InstrumentPlainTextEdit.cpp \
        SongPlainTextEdit.cpp \
        TrackPlainTextEdit.cpp \
        TuneManager.cpp \
        WaveChartViewchartview.cpp \
        main.cpp \
        tune_manager.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    ../chiptune.h \
    AudioPlayer.h \
    HardwareChiptunePanelWidget.h \
    HighlightWholeLinePlainTextEdit.h \
    InstrumentPlainTextEdit.h \
    SongPlainTextEdit.h \
    TrackPlainTextEdit.h \
    TuneManager.h \
    WaveChartView.h \
    tune_manager.h

FORMS += \
    HardwareChiptunePanelWidget.ui

RC_ICONS = chip.ico
