# icu_devices
This project contains code and scripts for data retrieving from ICU devices (GE_Monitor, Braun perfusor and Evita 4 ventilator) and realsense camera.

```
Icu_devices_project
│   README.md
│   LICENSE    
│
└───document : 
|   |   imgs
|   |   docu [contains official documents for devices]
|   |   hardware_list.md
|   |   ...
|
└───src : [Code for data retrieving]
│   │   bbraun_const.h
│   │   bbraun.cpp
│   │   ...
│   
└───scripts : [Scripts for connecting and disconnecting hardwares]
|   │   cancel_local.sh
|   │   cancel_remote.sh
|   │   start_local.sh
|   │   start_remote.sh
|
└───data : [Data retrieved from devices]
    │   XXXX.csv
    │   ...

```
The whole hardware connection setup is shown in the following graph, more detailed description about connections can be found in the file *hardware_list.md*
<img src="./documents/imgs/Connection.png">
<br>

## Folder **scripts**
Inside this folder you will see 4 bash scripts. They are used to automatically connect hardwares (ICU devices and cameras) to station via usbip service. More detail is found in the Readme.md file.
\
<br>


## Folder **src**
In this folder you will see the Qt project to retrieve data from medical devices. 
\
\
<img src='./documents/imgs/UML.png'>
\
\
In general, since all medical devices use serial protocal to do the communication, they all inherited from the Device class who contains a Qtserial port for serial communicating. 

```
GE_Monitor
│   ge_monitor.cpp
│   ge_monitor.hpp
│   datex.h    

BBraun
│   bbraun.cpp
│   bbraun.hpp
│   bbraun_const.h

Evita4_vent
│   evita4_vent.cpp
│   evita4_vent.hpp
│   draeger.h  

```

There is a function called **start()** for all ICU_devices classes.

This is the main working loop for the device. You can change the request and period. More detailed descriptions are found in documents folder.