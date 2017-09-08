//
//  Math.h
//  JRaz
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef __Math_h
#define __Math_h

inline int round( float _value )
{
	_value += 10000.5f;
	int ret = (int)_value;
	ret -= 10000;
	return ret;
}

#endif /* __Math_h */
