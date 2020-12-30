#!/usr/bin/python3

import re
import sys
import getopt
import os


def parseKeyFile(filename):
    with open(filename, 'r') as f:
        m = re.findall(r'  \{(.*)\}, //(.*)\n', f.read())

    keyMap = {}
    i = 0
    for mo in m:
        print('mo: ' + mo[1])
        keyMap[mo[1]] = (bytes([int(x, 16) for x in mo[0].split(', ')]), i)
        i += 1

    return keyMap


def main(args):
    """Generates key lists in c++11 header format.\nSupporting two different functions:

    1. Generates a new list of keys

    2. If an existing key list is specified as input file,
       another header file is generated which contains the selected keys as well as,
       a key index mapping table.
    """

    args, keys = getopt.getopt(args, 'hi:o:', ['help', 'input=', 'output='])

    for arg, value in args:
        if arg in ('-i', '--input'):
            inputFile = value
        elif arg in ('-o', '--output'):
            outputFile = value
        elif arg in ('-h', '--help'):
            print(main.__doc__)
            print('Usage: ./keygen.py [-i <inputfile>] -o <outputfile> keyname1 ...\n')
            sys.exit(0)

    keyList = []

    if 'outputFile' not in locals():
        print('no output file specified!')
        sys.exit(1)
    elif 'inputFile' in locals():
        availableKeys = parseKeyFile(inputFile)
        keyIdList = []

        for key in keys:
            try:
                data, id = availableKeys[key]
                keyList.append((key, data))
                keyIdList.append(id)
            except KeyError:
                print("key '" + key + "' not included in '" + inputFile + "'")
                sys.exit(2)
    else:
        for key in keys:
            keyList.append((key, os.urandom(47)))

    with open(outputFile, 'w') as f:
        f.write('#pragma once\n\n#include "KeyManager.h"\n\n'
                '#ifndef KEY_ATTR\n#  define KEY_ATTR\n#endif\n\n'
                'constexpr KeyManager::Key keys[] KEY_ATTR = {\n')

        for name, data in keyList:
            line = '  {'

            for byte in data:
                line += hex(byte) + ', '

            line = line[:-2] + '}, //' + name + '\n'
            f.write(line)

        f.write('};\n\n')

        if 'keyIdList' in locals():
            f.write('#ifndef KEYID_ATTR\n#  define KEYID_ATTR\n#endif\n\n'
                    'constexpr uint32_t keyIdMap[] KEYID_ATTR = {\n')
            for keyId in keyIdList:
                f.write('  ' + hex(keyId) + ',\n')

            f.write('};')


if __name__ == "__main__":
    main(sys.argv[1:])
