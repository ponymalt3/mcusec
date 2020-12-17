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
#include <map>
#include <functional>

#include "Crypt.h"
#include "KeyManager.h"
#include "Xorshift.h"

class AuthenicatedCallProvider
{
public:
  struct PendingCall
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
  	Xorshift64s m_prng;
  	KeyManager &m_keyManager;
  	Crypt m_crypt;
  	std::map<uint32_t,Function> m_functions;
  	PendingCall m_pendingCalls[4];
  	uint32_t m_leastRecentlyCall;
};


