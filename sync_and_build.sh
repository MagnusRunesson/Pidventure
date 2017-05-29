#!/bin/bash

HOST=192.168.1.12
PROJECT_FOLDER=projects/pidventure
BUILD_FOLDER=Pidventure

#HOST=192.168.2.65
#HOST=m3pi
#HOST=m3pi.local

rsync -av -e ssh ./ --exclude='.git' --exclude='build' --exclude='Tools' --exclude='pidventure.xcodeproj' pi@$HOST:~/$PROJECT_FOLDER

if [ ! -z "$1" ]; then

	if [ $1 -eq "0" ]; then
		ssh pi@$HOST << END
		sudo killall runme
		cd $PROJECT_FOLDER && make 2>&1
END
	elif [ $1 -eq "1" ]; then
		ssh pi@$HOST << END
		sudo killall runme
		cd $PROJECT_FOLDER && make 2>&1 && cd $BUILD_FOLDER && sudo ./runme
END
	fi
fi
