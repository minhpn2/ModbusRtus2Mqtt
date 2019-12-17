QT -= gui
QT +=  mqtt core concurrent serialbus

qtConfig(modbus-serialport): QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += "$$PWD/../../../raspi/sysroot/" \
               "$$PWD/../../../raspi/sysroot/lib" \
               "$$PWD/../../../raspi/sysroot/opt" \
               "$$PWD/../../../raspi/sysroot/usr" \
               "$$PWD/../../../raspi/sysroot/usr/include" \
               "$$PWD/../../../raspi/sysroot/usr/include/arm-linux-gnueabihf"

LIBS +=    "-L$$PWD/../../../raspi/sysroot/" \
           "-L$$PWD/../../../raspi/sysroot/opt" \
           "-L$$PWD/../../../raspi/sysroot/lib" \
           "-L$$PWD/../../../raspi/sysroot/usr" \
           "-L$$PWD/../../../raspi/sysroot/usr/include" \
           "-L$$PWD/../../../raspi/sysroot/usr/include/arm-linux-gnueabihf"


SOURCES += \
        main.cpp \
    maincontroller.cpp \
    configsetting.cpp \
    modbusconfig.cpp \
    mqttconfig.cpp \
    recordlatencynetworkexcelfile.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /usr/local/qt5pi/vpos
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    maincontroller.h \
    configsetting.h \
    modbusconfig.h \
    mqttconfig.h \
    recordlatencynetworkexcelfile.h

