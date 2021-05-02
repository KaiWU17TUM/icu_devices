# Evita4 ventilator

MEDIBUS is a software protocol intended to be used by
two medical devices for exchanging data and control
functions via their RS 232 interfaces.

The MEDIBUS protocol distinguishes two basic types of
messages:
– commands
– responses.
A command is transmitted by one device to request data
from the other device or to control its function.
A response is transmitted by one device upon receipt of
a command from the other device. Responses may
contain embedded commands.

* In our code, the program periodically sends commands to the machine to get data, the period can be changed in the start() funciton. There you will see a timer which calls function request_device_settings() periodically.

> connect(timer_cp1, SIGNAL(timeout()), this, SLOT(request_device_settings()));


You can change it to 
    
    1. request_device_settings()
    2. request_measurement_cp1()
    3. request_measurement_cp2()
    4. request_text_messages()

The response corresponds to 
    
    1. The current values of all device settings applicable with the responding device.
    2. Current measured Data (codepage 1)
    3. Current measured Data (codepage 2)
    4. All the text messages the requested device currently holds for user information, along with the text code, text length and an end-of-text marker.

A more detailed description for measured data is found in page 17 of ducument *evita4_data*