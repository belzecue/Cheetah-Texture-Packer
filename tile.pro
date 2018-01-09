#-------------------------------------------------
#
# Project created by QtCreator 2012-04-29T22:23:49
#
#-------------------------------------------------

QT += core gui

TARGET = cheetah-texture-packer

QT_VERSION=$$[QT_VERSION]

contains(QT_VERSION, "^5.*") {
  QT += widgets
} else {
}

TEMPLATE = app
INCLUDEPATH += src

SOURCES += src/main.cpp\
        src/mainwindow.cpp \
        src/widgets/view.cpp \
        src/imagepacker.cpp \
        src/imagecrop.cpp \
        src/maxrects.cpp \
    src/support.cpp \
    src/parsearguments.cpp \
    src/widgets/qclickablelabel.cpp \
    src/settingspanel.cpp \
    src/preferences/rotationsettings.cpp \
    src/preferences/sortsettings.cpp \
    src/preferences/heuristicsettings.cpp \
    src/superxbrscaler.cpp \
    src/imagescaler.cpp \
    src/widgets/qimageview.cpp \
    src/rendering/brushes.cpp \
    src/preferences/sizeconstraintsettings.cpp \
    src/preferences/scalesettings.cpp \
    src/inputimage.cpp \
    src/rc_crc32.c \
    src/preferences/pixelformatsettings.cpp

HEADERS  += src/mainwindow.h \
        src/widgets/view.h \
        src/imagepacker.h \
        src/maxrects.h \
    src/support.h \
    src/parsearguments.h \
    src/widgets/qclickablelabel.h \
    src/settingspanel.h \
    src/preferences/rotationsettings.h \
    src/preferences/sortsettings.h \
    src/preferences/heuristicsettings.h \
    src/superxbrscaler.h \
    src/imagescaler.h \
    src/widgets/qimageview.h \
    src/rendering/brushes.h \
    src/preferences/sizeconstraintsettings.h \
    src/widgets/qconstrainedspinbox.hpp \
    src/preferences/scalesettings.h \
    src/inputimage.h \
    src/qimageptr.hpp \
    src/rc_crc32.h \
    src/preferences/pixelformatsettings.h

FORMS    += src/mainwindow.ui

TRANSLATIONS += src/tile_ru.ts
