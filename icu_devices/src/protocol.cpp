#include "protocol.h"
#include "device.h"

Protocol::Protocol(std::string config_file, Device *device)
{
    this->device = device;
    // load_protocol_config(config_file);
}
