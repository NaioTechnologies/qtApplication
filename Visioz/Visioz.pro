#-------------------------------------------------
#
# Project created by QtCreator 2015-05-11T12:40:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Visioz
TEMPLATE = app

INCLUDEPATH += /usr/include/opencv2
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_objdetect -lopencv_features2d

QMAKE_CXXFLAGS += -std=c++14

SOURCES += main.cpp \
        mainwindow.cpp \
    ObjectTracker.cpp \
    SaladDetector.cpp \
    q_debugstream.cpp \
    visionthread.cpp

HEADERS  += mainwindow.h \
    ObjectTracker.h \
    SaladDetector.h \
    q_debugstream.h \
    visionthread.h

FORMS    += mainwindow.ui


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/lib/release/ -lrobbie
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/lib/debug/ -lrobbie
else:unix: LIBS += -L$$PWD/../libraries/lib/ -lrobbie

INCLUDEPATH += $$PWD/../libraries/include/robbie
DEPENDPATH += $$PWD/../libraries/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../libraries/lib/release/ -lvitals
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../libraries/lib/debug/ -lvitals
else:unix: LIBS += -L$$PWD/../libraries/lib/ -lvitals

INCLUDEPATH += $$PWD/../libraries/include/vitals
DEPENDPATH += $$PWD/../libraries/include
