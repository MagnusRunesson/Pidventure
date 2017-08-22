#
# Makefile
#

CIRCLEHOME = /Users/magnusrunesson/Projects/ROS/lib-circle
OUTPUT_DIR	= build/

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
	Pidventure/DataManual/Animations.cpp \
	Engine/Audio/AudioData.cpp \
	Engine/Audio/AudioHandler.cpp \
	Engine/Audio/AudioMixer.cpp \
	Engine/Audio/AudioSource.cpp \
	Engine/Audio/AudioStream.cpp \
	Engine/Core/BitHelpers.cpp \
	Engine/Core/Debug.cpp \
	Engine/Core/Timer.cpp \
	Engine/Core/Memory.cpp \
	Engine/Util/String.cpp \
	Engine/Util/Memory.cpp \
	Engine/Graphics/Animation.cpp \
	Engine/Graphics/Image.cpp \
	Engine/Graphics/Screen.cpp \
	Engine/Graphics/Sprite.cpp \
	Engine/Graphics/SpriteRenderer.cpp \
	Engine/Graphics/TileBank.cpp \
	Engine/Graphics/TileMap.cpp \
	Engine/Graphics/TileRenderer.cpp \
	Engine/Graphics/Font.cpp \
	Engine/Graphics/TextRenderer.cpp \
	Engine/IO/Joypad.cpp \
	Engine/IO/FileCache.cpp \
	Engine/IO/Data.cpp \
	Engine/Platform/RPi_Circle/file_rpi_circle.cpp \
	Engine/Platform/RPi_Circle/main.cpp \
	Engine/Scene/Camera.cpp \
	Engine/Scene/GameObject.cpp \
	Engine/Scene/GameObjectManager.cpp \
	Engine/Scene/GameObjectTemplate.cpp \
	Engine/Scene/Scene.cpp

INCLUDE = -I .

CFLAGS	= -DENGINE_TARGET_RPI_CIRCLE -fpermissive
LIBS	= $(CIRCLEHOME)/lib/libcircle.a

include Rules.mk
