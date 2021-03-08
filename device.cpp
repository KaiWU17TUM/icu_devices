#include "device.h"


Device::Device()
{

}

void Device::try_to_open_port(){
    local_serial_port->open_serial_port();
}

