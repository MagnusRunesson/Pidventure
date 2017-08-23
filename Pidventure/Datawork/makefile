CONV_MAP=tmx2scene
CONV_SPRITE=png2le
CONV_BANK=png2petb
CONV_SFX=bin2c
counter=0

MAP_FILES=$(wildcard scene_*.tmx)
SPRITE_FILES=$(wildcard sprite_*.png)
BANK_FILES=$(wildcard tilebank_*.png)
SFX_FILES=$(wildcard audiosamples_*.raw)

CONVERTED_MAP_FILES=$(MAP_FILES:.tmx=.petm)
CONVERTED_SPRITE_FILES=$(SPRITE_FILES:.png=.pei)
CONVERTED_BANK_FILES=$(BANK_FILES:.png=.petb)
CONVERTED_SFX_FILES=$(SFX_FILES:.raw=.cpp)

ALL_CONVERTED_FILES=$(CONVERTED_MAP_FILES) $(CONVERTED_BANK_FILES) $(CONVERTED_SPRITE_FILES) $(CONVERTED_SFX_FILES)

# $1: original name
define crunch_name
	@$(eval counter=$(shell echo $$(($(counter)+1))))
	@cp ../data/$1 ../data/$(basename $(shell printf '%08d' $(counter))).dat
	@$(shell echo $1:$(basename $(shell printf '%08d' $(counter))).dat >> filelist.txt)
endef

build: clean_file_list $(ALL_CONVERTED_FILES) move_file_list

clean_file_list:
	$(shell rm filelist.txt)

move_file_list:
	@echo Moving file list
	$(shell mv filelist.txt ../data/filelist.txt)

%.petm: %.tmx
	@echo Converting scene $^
	@$(CONV_MAP) $^ ../data/$(subst .tmx,,$^) > /dev/null
	$(call crunch_name,$(subst .tmx,,$^).petm)
	$(call crunch_name,$(subst .tmx,,$^).peso)

sprite_%.pei: sprite_%.png
	@echo Converting sprite $^
	@$(CONV_SPRITE) $^ ../data/$(subst .png,,$^) > /dev/null
	$(call crunch_name,$(subst .png,,$^).pei)

tilebank_%.petb: tilebank_%.png
	@echo Converting tilebank $^ with tiles at 4x4 pixels
	@$(CONV_BANK) $^ ../data/$(subst .png,,$^) 4 4 > /dev/null
	$(call crunch_name,$(subst .png,,$^).petb)

audiosamples_%.cpp: audiosamples_%.raw
	@echo Converting audio samples $^
	@$(CONV_SFX) $^ ../data/$(subst .raw,,$^) $(subst .raw,,$^) > /dev/null
