BLUE='\033[0;36m'
GREEN='\033[1;32m'
NC='\033[0m'

echo "${GREEN}Start shell for detach all hardwares in mocked ICU room${NC}"

IP_RASP1='10.42.0.67'
IP_RASP2='10.42.1.46'
RASP1='pi@'$IP_RASP1
RASP2='pi@'$IP_RASP2


echo "${BLUE}Running commands in localmachine ${NC}"

echo "${BLUE}Detach all devices from ip addressi $IP_RASP1 ${NC}"
sudo /usr/lib/linux-tools/4.15.0-135-generic/usbip detach -p 00

echo "${BLUE}Running commands in Raspberry pi $IP_RASP1 and $IP_RASP2 ${NC}"
ssh -i ~/.ssh/id_rsa $RASP1 'bash -s'<cancel_remote.sh
ssh -i ~/.ssh/id_rsa $RASP2 'bash -s'<cancel_remote.sh


