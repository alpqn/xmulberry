QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++23
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DESTDIR = bin
UI_DIR = build/ui
OBJECTS_DIR = build/obj
RCC_DIR = build
MOC_DIR = build/moc
INCLUDEPATH = include

SOURCES += \
           src/main.cpp \
           src/MimeApp.cpp \
           src/Mulberry.cpp \
           src/Setting.cpp \
           src/Utils.cpp

HEADERS += \
           include/MimeApp.hpp \
           include/Mulberry.hpp \
           include/Setting.hpp \
           include/Utils.hpp

FORMS +=   ui/Mulberry.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
