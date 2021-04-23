#! /bin/bash
echo "STEP 2 : Unbanding available devices..."
DEVICE_ARRAY=$(sudo usbip list -l|cut -d " " -f 4|grep [0-9])
#echo "Devices connected are"
for DEVICE in $DEVICE_ARRAY
do
        echo $DEVICE
        sudo usbip unbind -b $DEVICE
done

sudo killall -9 usbipd
