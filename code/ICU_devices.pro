QT += serialport

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        bbraun.cpp \
        device.cpp \
        evita4_vent.cpp \
        ge_monitor.cpp \
        main.cpp \
        myserialport.cpp

HEADERS += \
    bbraun.h \
    bbraun_const.h \
    datex.h \
    device.h \
    evita4_vent.h \
    ge_monitor.h \
    myserialport.h \
    draeger.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target