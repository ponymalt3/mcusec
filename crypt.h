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

void sha1_compress(uint32_t state[5], const uint8_t block[64]);

class xorshift64s
{
public:
  xorshift64s(uint64_t seed=0xAE5BB37D89368DEULL)
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

class KeyManager
{
public:
  typedef uint8_t Key[47];
  
	KeyManager(const Key *keys,uint32_t numKeys)
	{
		m_keys=keys;
		m_numKeys=numKeys;
	}
	
	const Key& getKey(uint32_t i) const
	{
		return m_keys[i];
	}
	
protected:
  const Key *m_keys;
  uint32_t m_numKeys;
};
 
class Crypt
{
public:
	typedef uint32_t Hash[5];
	
	Crypt(const KeyManager &keyManager):m_keyManager(keyManager)
 	{
 		m_block[55]=0x80;
 		m_block[56]=0x00;
 		m_block[57]=0x00;
 		m_block[58]=0x00;
 		m_block[59]=0x00;
 		m_block[60]=0x00;
 		m_block[61]=0x00;
 		m_block[62]=((8*55)>>8)&0xFF;
 		m_block[63]=(8*55)&0xFF;
 	}
 
	void sign(uint32_t key,uint64_t random,Hash &hash)
	{
		hash[0]=0x67452301;
		hash[1]=0xEFCDAB89;
		hash[2]=0x98BADCFE;
		hash[3]=0x10325476;
		hash[4]=0xC3D2E1F0;
		
		for(uint32_t i=0;i<sizeof(KeyManager::Key);++i)
		{
			m_block[i]=m_keyManager.getKey(key)[i];
		}
		
		for(uint32_t i=0;i<sizeof(random);++i)
		{
			m_block[sizeof(KeyManager::Key)+i]=random&0xFF;
			random>>=8;
		}	
		
		sha1_compress(hash,m_block);
	}
	
	bool verify(const Hash &hash,uint32_t key,uint64_t random)
	{
		Hash hashVerify;
		sign(key,random,hashVerify);
		
		for(uint32_t i=0;i<(sizeof(hash)/sizeof(hash[0]));++i)
		{
			if(hash[i] != hashVerify[i])
			{
				return false;
			}
		}
		
		return true;
	}
	
	uint64_t getPseudoRandom() { return m_prng.getPseudoRandom(); }
	
protected:	
	xorshift64s m_prng;
	const KeyManager &m_keyManager; 
	uint8_t m_block[64];
};


