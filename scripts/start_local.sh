GREEN='\033[1;32m'
BLUE='\033[0;36m'
NC='\033[0m'


IP_RASP1='10.42.0.67'
IP_RASP2='10.42.1.46'
RASP1='pi@'$IP_RASP1
RASP2='pi@'$IP_RASP2


echo "${GREEN}Start shell for auto setup hardwares in mocked ICU room${NC}"

echo "${BLUE}STEP 1 : Copy and run commands in Raspberry pi ...  $IP_RASP1 ${NC}"
scp -i ~/.ssh/id_rsa start_remote.sh $RASP1:/home/pi/
scp -i ~/.ssh/id_rsa start_remote.sh $RASP2:/home/pi/

ssh -i ~/.ssh/id_rsa $RASP1 'sh start_remote.sh  > /dev/null 2>&1 &' 
ssh -i ~/.ssh/id_rsa $RASP2 'sh start_remote.sh  > /dev/null 2>&1 &' 

sleep 2

echo "${GREEN}Running commands in localmachine ${NC}"
echo "${BLUE}STEP 2 : Mounting necessary driver for usb-ip ... ${NC}"
sudo modprobe vhci-hcd

echo "${BLUE}STEP 3 : Connecting existing devices from ip address ... $IP_RASP1 ${NC}"
DEVICES_AVAILABLE=$(sudo /usr/lib/linux-tools/4.15.0-135-generic/usbip list -r $IP_RASP1|grep vendor|cut -d ":" -f 1)

for DEVICE in $DEVICES_AVAILABLE
do
	echo $DEVICE
	sudo /usr/lib/linux-tools/4.15.0-135-generic/usbip attach -r $IP_RASP1 -b $DEVICE
done

DEVICES_AVAILABLE=$(sudo /usr/lib/linux-tools/4.15.0-135-generic/usbip list -r $IP_RASP2|grep vendor|cut -d ":" -f 1)

echo "Look Here > ${DEVICES_AVAILABLE} ${IP_RASP2}"

for DEVICE in $DEVICES_AVAILABLE
do
	echo $DEVICE
	sudo /usr/lib/linux-tools/4.15.0-135-generic/usbip attach -r $IP_RASP2 -b $DEVICE
done



echo "${GREEN}Finish initialization ${NC}"
