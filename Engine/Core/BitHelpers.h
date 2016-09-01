//
//  BitHelpers.hpp
//  TinyArcade
//
//  Created by Magnus Runesson on 2015-12-03.
//  Copyright © 2015 Magnus Runesson. All rights reserved.
//

#ifndef BitHelpers_hpp
#define BitHelpers_hpp

#define SetBit( value, bitmask ) (value |= bitmask)
#define ClrBit( value, bitmask ) (value &= ~bitmask)
#define HasBit( value, bitmask ) ((value & bitmask) == bitmask)

#endif /* BitHelpers_hpp */
