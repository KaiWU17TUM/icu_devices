# icu_devices

# Folder scripts
* Inside this folder you will see 4 bash scripts. They are used to automatically connect hardwares (ICU devices and cameras) to station via usbip service.

## Connect everything
* To connect everything, just run "sudo sh start_local.sh", it will perform the following tasks

### STEP 1 : Copy and run commands in Raspberry pi 
* Copy start_remote.sh file to two raspberry pi and run the file there. It will mount necessary driver, start usbip service and bind all devices to the usbip service.

### STEP 2 : Mounting necessary driver for usb-ip
* Mount vhci-hcd driver to linux kernel

### STEP 3 : Connecting existing devices from ip address
* List all devices bind to the two raspberry pi and filter out the bus-id of devices
* Attach the devices with their bus-ids

### STEP 4 : Changing access permission of USB devices
* Giving software the access permission to serial ports

## Disconnect everything
* To disconnect everything, just run "sudo sh cancel_local.sh", it will perform the following tasks

### STEP1 :  Stop usbip locally
### STEP2 :  Copy and run commands in Raspberry pi 
* Copy cancel_remote.sh file to two raspberry pi and run the file there. It will unbind devices and kill the usbip service.
