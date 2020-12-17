/* 
 * MIT License
 * 
 * Copyright (c) 2020 Malte Graeper
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * - The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 * - The Software is provided "as is", without warranty of any kind, express or
 *   implied, including but not limited to the warranties of merchantability,
 *   fitness for a particular purpose and noninfringement. In no event shall the
 *   authors or copyright holders be liable for any claim, damages or other
 *   liability, whether in an action of contract, tort or otherwise, arising from,
 *   out of or in connection with the Software or the use or other dealings in the
 *   Software.
 */
 
#pragma once

#include <stdint.h>

class Xorshift64s
{
public:
  Xorshift64s(uint64_t seed=0xAE5BB37D89368DEULL)
  {
  	m_state=seed;
  }
  
  uint64_t getPseudoRandom()
  {
    m_state^=m_state>>12;
    m_state^=m_state>>25;
    m_state^=m_state>>27;
    
    return m_state*0x2545F4914F6CDD1DULL;
  }

protected:
  uint64_t m_state;
};
