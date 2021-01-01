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

class KeyManager
{
public:
  typedef uint8_t Key[47];

	KeyManager(const Key *keys,uint32_t numKeys,const uint32_t *keyIdMap=nullptr)
	{
		m_keys=keys;
		m_numKeys=numKeys;
    m_keyIdMap=keyIdMap;
	}

	const Key& getKey(uint32_t id) const
	{
    if(m_keyIdMap)
    {
      for(uint32_t i=0;i<m_numKeys;++i)
      {
        if(m_keyIdMap[i] == id)
        {
          return m_keys[i];
        }
      }
    }

		return m_keys[id];
	}

protected:
  const Key *m_keys;
  uint32_t m_numKeys;
  const uint32_t *m_keyIdMap;
};
