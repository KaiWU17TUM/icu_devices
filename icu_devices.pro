QT += serialport

CONFIG += qt c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/bbraun.cpp \
    src/datex_ohmeda.cpp \
    src/device.cpp \
    src/evita4_vent.cpp \
    src/ge_monitor.cpp \
    src/logger.cpp \
    src/main.cpp \
    src/myserialport.cpp \
    src/protocol.cpp

HEADERS += \
    src/bbraun.h \
    src/bbraun_const.h \
    src/datex.h \
    src/datex_ohmeda.h \
    src/device.h \
    src/evita4_vent.h \
    src/ge_monitor.h \
    src/logger.h \
    src/myserialport.h \
    src/draeger.h \
    src/protocol.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
