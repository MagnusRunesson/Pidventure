//
//  World.h
//  pidventure
//
//  Created by Magnus Runesson on 2016-10-26.
//  Copyright © 2016 Magnus Runesson. All rights reserved.
//

#ifndef World_h
#define World_h

class CDoor;
class CPlayer;

void worldInit();
void worldExit();

void worldUpdate();
void worldInteract();

CDoor* worldGetDoorAt();

#endif /* World_h */
