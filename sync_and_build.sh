#!/bin/bash

HOST=192.168.1.12
TARGET_FOLDER=pidventure
#HOST=192.168.2.65
#HOST=m3pi
#HOST=m3pi.local

rsync -av -e ssh ./ --exclude='.git' --exclude='build' --exclude='Tools' --exclude='pidventure.xcodeproj' pi@$HOST:~/$TARGET_FOLDER

if [ ! -z "$1" ]; then

	if [ $1 -eq "0" ]; then
		ssh pi@$HOST << END
		sudo killall runme
		cd $TARGET_FOLDER && make 2>&1
END
	elif [ $1 -eq "1" ]; then
		ssh pi@$HOST << END
		sudo killall runme
		cd $TARGET_FOLDER && make 2>&1 && sudo ./runme
END
	fi
fi
