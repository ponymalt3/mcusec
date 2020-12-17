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

#include "KeyManager.h"

void sha1_compress(uint32_t state[5], const uint8_t block[64]);

class Crypt
{
public:
	struct Hash
	{
		Hash() { clear(); }
		Hash(uint32_t a,uint32_t b,uint32_t c,uint32_t d,uint32_t e):data{a,b,c,d,e} {}
		
		bool operator==(const Hash &rhs) const
		{
			return data[0] == rhs.data[0] &&
			       data[1] == rhs.data[1] &&
			       data[2] == rhs.data[2] &&
			       data[3] == rhs.data[3] &&
			       data[4] == rhs.data[4];
		}
		
		void clear()
		{
			data[0]=0;
			data[1]=0;
			data[2]=0;
			data[3]=0;
			data[4]=0;			
		}
		
		uint32_t data[5];
	};
	
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
		hash={0x67452301,0xEFCDAB89,0x98BADCFE,0x10325476,0xC3D2E1F0};
		
		for(uint32_t i=0;i<sizeof(KeyManager::Key);++i)
		{
			m_block[i]=m_keyManager.getKey(key)[i];
		}
		
		for(uint32_t i=0;i<sizeof(random);++i)
		{
			m_block[sizeof(KeyManager::Key)+i]=random&0xFF;
			random>>=8;
		}	
		
		sha1_compress(hash.data,m_block);
	}
	
	bool verify(const Hash &hash,uint32_t key,uint64_t random)
	{
		Hash hashVerify;
		sign(key,random,hashVerify);
		
		if(hash == hashVerify)
		{
			return true;
		}
		
		return false;
	}
	
protected:	
	const KeyManager &m_keyManager; 
	uint8_t m_block[64];
};

class AuthenicatedCallProvider
{
public:
  struct _PendingCall
  {
  	Crypt::Hash hash;
  	uint32_t callId;
  	uint32_t parameter;
  };
  
  struct Function
  {
  	std::function<void(uint32_t)> callback;
  	uint32_t keyId;
  };
  
	AuthenicatedCallProvider(KeyManager &keyManager)
	: m_keyManager(keyManager)
	, m_crypt(keyManager)
	, m_leastRecentlyCall(0)
	{
	}
	
	void registerFunction(uint32_t id,uint32_t useKeyId,const std::function<void(uint32_t)> &callback)
	{
		m_functions.insert(std::make_pair(id,Function{callback,useKeyId}));
	}
	
	uint64_t requestCallToken(uint32_t id,uint32_t parameter)
	{
		auto it=m_functions.find(id);
		
		if(it == m_functions.end())
		{
			return 0ULL;
		}
		
		uint64_t token=m_prng.getPseudoRandom()^id;
		
		shiftPendingCall();
		
		m_crypt.sign(it->second.keyId,token,m_pendingCalls[m_leastRecentlyCall].hash);
		m_pendingCalls[m_leastRecentlyCall].callId=id;
		m_pendingCalls[m_leastRecentlyCall].parameter=parameter;
		
		return token;		
	}
	
	bool executeCall(const Crypt::Hash &hash)
	{
		for(uint32_t i=0;i<4;++i)
		{
			if(hash == m_pendingCalls[i].hash)
			{
				m_pendingCalls[i].hash.clear();
				auto x=m_functions.find(m_pendingCalls[i].callId);
				x->second.callback(m_pendingCalls[i].parameter);
				m_pendingCalls[i].callId=0;
				m_pendingCalls[i].parameter=0;
				return true;
			}
		}
		
		return false;
	}
	
	void shiftPendingCall()
	{
		m_leastRecentlyCall=(m_leastRecentlyCall+1)%4;
		m_pendingCalls[m_leastRecentlyCall].hash.clear();
	}
	
protected:
  	xorshift64s m_prng;
  	KeyManager &m_keyManager;
  	Crypt m_crypt;
  	std::map<uint32_t,Function> m_functions;
  	_PendingCall m_pendingCalls[4];
  	uint32_t m_leastRecentlyCall;
};


