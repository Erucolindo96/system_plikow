TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    dysk.c \
    dysk_private.c

HEADERS += \
    dysk.h \
    dysk_private.h

