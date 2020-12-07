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


#include <iostream>
#include <stdint.h>

#include "crypt.h"

int main()
{
	KeyManager::Key key1="1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJ";
	key1[46]='K';
	
	KeyManager km(&key1,1);
	
	Crypt crypt(km);
	
	uint64_t rnd=crypt.getPseudoRandom();
	rnd=(uint64_t('SRQP')<<32)+'ONML';
	
	Crypt::Hash hash;
	crypt.sign(0,rnd,hash);
	
	std::cout<<"hash: ";
	for(uint32_t i=0;i<5;++i)
	{
		std::cout<<std::hex<<((hash[i]>>24)&0xFF);
		std::cout<<std::hex<<((hash[i]>>16)&0xFF);
		std::cout<<std::hex<<((hash[i]>>8)&0xFF);
		std::cout<<std::hex<<((hash[i]>>0)&0xFF);
	}
	
	std::cout<<std::dec<<" (should be b7d41243314081ed66be7ff92c61521b525ab85d)\n";
	
	bool verified=crypt.verify(hash,0,rnd);
	
	std::cout<<"verified: "<<(verified?"true":"false")<<"\n";
	
	return 0;
}
