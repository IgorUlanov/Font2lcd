#-------------------------------------------------
#
# Project created by QtCreator 2012-08-31T16:44:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Font2lcd
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
		paint.cpp \
    parametr.cpp

HEADERS  += mainwindow.h \
    paint.h \
    parametr.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    setting.txt \
    symbol.csv \
    font2lcd.rc

RESOURCES +=  my.qrc

TRANSLATIONS    = font2lcd_ru.ts

RC_FILE = font2lcd.rc
