#
# Makefile
#

SRCS=\
	Pidventure/Pidventure.cpp \
	Pidventure/background.cpp \
	Pidventure/Physics.cpp \
	Pidventure/CameraController.cpp \
	Pidventure/Content/World.cpp \
	Pidventure/Gameplay/Door.cpp \
	Pidventure/Player/Player.cpp \
	Pidventure/Player/PlayerAvatar.cpp \
	Pidventure/Player/PlayerInventory.cpp \
	Pidventure/DataManual/Animations.cpp

include makefile_gameidentifier.include

OUTPUT_BASE_NAME=$(GAME_IDENTIFIER)

include Engine/Build/RPi/Makefile.Include
