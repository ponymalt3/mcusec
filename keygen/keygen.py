#!/usr/bin/python3

import re
import sys
import getopt
import os
import io


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

# if already keys available generate file with selected ones otherwise
# generate new inputFile


def main(args):
    args, keys = getopt.getopt(args, 'hi:o:')

    print(keys)

    inputFile = ''
    outputFile = ''

    for arg, value in args:
        if arg in ('-i', '--input'):
            inputFile = value
        elif arg in ('-o', '--output'):
            outputFile = value

    keyList = []

    if outputFile == '':
        print('no output file specified!')
        sys.exit(1)
    elif inputFile != '':
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
