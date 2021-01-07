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

#include "AuthenicatedCallProvider.h"

namespace Provider {
#include "AllKeys.h"
}

namespace Client {
#include "ClientKey1.h"
}

int main()
{
	KeyManager kmServer(Provider::keys,sizeof(Provider::keys)/sizeof(KeyManager::Key));
	AuthenticatedCallProvider acp(kmServer);

  //using key 'key1' (index: 2)
	acp.registerFunction(99,2,[&](uint32_t data){
		std::cout<<"function called with param = "<<(data)<<"\n";
	});

	KeyManager kmClient(Client::keys,sizeof(Client::keys)/sizeof(KeyManager::Key),Client::keyIdMap);
	Crypt crypt(kmClient);

	uint64_t token=acp.requestCallToken(99,123456789);

	Crypt::Hash authFunctionCall;
	crypt.sign(2,token,authFunctionCall);

	acp.executeCall(authFunctionCall);

	return 0;
}
