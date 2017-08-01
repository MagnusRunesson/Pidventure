//
//  GameObject.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef GameObject_hpp
#define GameObject_hpp

#include "Engine/Graphics/Animation.h"

#define GO_SYSTEMFLAG_ENABLED		(1<<0)

class Sprite;
class Image;
class Animation;
class AnimationSequenceDefinition;

class GameObject
{
	//
	//
	//
public:
	GameObject();
	void Reboot();
	void Create( const Image* _image );
	void Create( const AnimationSequenceDefinition* _animation );
	void Destroy();
	
	//
	void SetEnabled( bool _enabled );
	bool IsEnabled();
	
	//
	void Update();
	void Render();

	void SetHotspot( float _x, float _y );
	
	void SetWorldPosition( float _x, float _y );
	float GetWorldPositionX();
	float GetWorldPositionY();
	
	bool IsUsed();
	Sprite* GetSprite();
	void SetImage( const Image* _pImage );
	const AnimationSequenceDefinition* GetAnimationSequenceDefinition();
	Animation* GetAnimation();

	void* m_customObject;
	void(*m_customUpdate)(void*);
	void(*m_customPreRender)(void*);
	void(*m_customPostRender)(void*);
	
	int m_flags;
	uint32 m_systemFlags;
	
	//
	//
	//
protected:
	//
	float m_worldPositionX;
	float m_worldPositionY;

	//
	//Image* m_image;
	Sprite* m_sprite;
	float m_imageHotspotX;
	float m_imageHotspotY;
	
	//
	Animation m_animation;
	const AnimationSequenceDefinition* m_animationSequenceDefinition;
	
	//
	bool m_drawAlpha;
};

#endif /* GameObject_hpp */
