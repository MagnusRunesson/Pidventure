INCPATHS=-I/opt/vc/include \
         -I/opt/vc/include/interface/vcos/pthreads \
         -I/opt/vc/include/interface/vmcs_host/linux \
         -I/home/pi/pidventure

#CXXFLAGS=-std=c++11 $(INCPATHS) -DENGINE_TARGET_RPI
CXXFLAGS=$(INCPATHS) -DENGINE_TARGET_RPI
CXXFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi

LDFLAGS=
LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lGLESv2 -lEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm -L../libs/ilclient -L../libs/vgfont

LIBS=-L/opt/vc/lib -L/usr/local/lib -lEGL -lGLESv2
#  -lbcm_host -lpng -lz

# Music
#ifdef WITH_MUSIC
#	CXXFLAGS+=$(shell libmikmod-config --cflags)
#	CXXFLAGS+=-DMUSIC
#	LDFLAGS+=$(shell libmikmod-config --libs)
#endif

SRCS=\
	Engine/Audio/AudioData.cpp \
	Engine/Audio/AudioHandler.cpp \
	Engine/Audio/AudioMixer.cpp \
	Engine/Audio/AudioSource.cpp \
	Engine/Audio/AudioStream.cpp \
	Engine/Core/BitHelpers.cpp \
	Engine/Core/Debug.cpp \
	Engine/Core/Timer.cpp \
	Engine/Graphics/Animation.cpp \
	Engine/Graphics/Image.cpp \
	Engine/Graphics/Screen.cpp \
	Engine/Graphics/Sprite.cpp \
	Engine/Graphics/SpriteRenderer.cpp \
	Engine/Graphics/TileBank.cpp \
	Engine/Graphics/TileMap.cpp \
	Engine/Graphics/TileRenderer.cpp \
	Engine/IO/Joypad.cpp \
	Engine/Platform/RPi/file_rpi.cpp \
	Engine/Platform/RPi/main.cpp \
	Engine/Scene/Camera.cpp \
	Engine/Scene/GameObject.cpp \
	Engine/Scene/GameObjectManager.cpp \
	Engine/Scene/GameObjectTemplate.cpp \
	Pidventure/background.cpp \
	Pidventure/CameraController.cpp \
	Pidventure/Physics.cpp \
	Pidventure/Pidventure.cpp \
	Pidventure/Player/Player.cpp \
	Pidventure/Player/PlayerAvatar.cpp \
	Pidventure/Player/PlayerInventory.cpp \
	Pidventure/Scene.cpp \
	Pidventure/DataManual/Data.cpp \
	Pidventure/DataManual/Animations.cpp

OBJS:=$(addprefix build/,$(SRCS:.cpp=.o))
# SRCS:=$(addprefix src/,$(SRCS))

OUT=runme

all: $(OUT)

$(OUT): $(OBJS)
	@g++ -o $(OUT) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(OBJS)

build/%.o: %.cpp
	@mkdir -p build
	g++ -c -o $@ $(CXXFLAGS) $<

clean:
	rm -f $(OBJS) $(OUT)

#pngtest:
#	g++ -o test src/test_png.cpp src/textureloader.cpp  -lpng -lz
#	./test font.png

 # pngtest
.PHONY: all
