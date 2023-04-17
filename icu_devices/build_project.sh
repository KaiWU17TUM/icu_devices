#!/bin/sh

FILEPATH=$( dirname -- "$0"; )

read -p 'Build mode [debug/release]: ' MODE
read -p 'Folder name [default = ""]: ' FOLDERNAME

BUILDFOLDER=${FILEPATH}/build/${MODE}${FOLDERNAME}

echo "Binaries will be built in : ${BUILDFOLDER}"

if [ ${MODE} != "debug" ] && [ ${MODE} != "release" ]; then
	echo "Please choose either debug or release modes please..."
	exit 1
fi

mkdir -p ${BUILDFOLDER}

cd ${BUILDFOLDER}

qmake "CONFIG+=${1}" ../../icu_devices.pro

make -j$(($(nproc)-1))
