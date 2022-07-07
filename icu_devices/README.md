# Raspberrypi code
Code for reading data from medical devices. Compile using the build_project script and update the cfg files in the build folder when running.

# Files in folder [src](./src/) 
Code to read data from various medical devices.

BBraun Perfusors:
- bbraun_const.h (data definition)
- bcc.h
- bcc.cpp

GE Monitor:
- datex.h (data definition)
- datex_ohmeda.h
- datex_ohmeda.cpp

Draeger Evita:
- draeger.h (data definition)
- medibus.h
- medibus.cpp

Device - It is a base class for the above devices that is responsible to connect the serial ports & sends the protocols:
- device.h
- device.cpp
- myserialport.h
- myserialport.cpp
- protocol.h
- protocol.cpp

Misc:
- helper.h
- helper.cpp
- logger.h
- logger.cpp
