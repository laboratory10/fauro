import os
import sys
import zlib

FSW_IMAGE_BINARY_FILEPATH = (os.path.dirname(sys.path[0]) 
                             + '/fauro/build-fprime-automatic-featherM0/bin/featherM0/Fauro.bin')

if os.path.exists(FSW_IMAGE_BINARY_FILEPATH):
    file = open(FSW_IMAGE_BINARY_FILEPATH, 'rb')
    image = file.readlines()
    file.close()
    checksum = 0
    for line in image:
        checksum = zlib.crc32(line, checksum)
    #Complete, return after flipping bits and turning into a hex string
    print('0x%08x' % (checksum & 0xFFFFFFFF))
    print(os.path.getsize(FSW_IMAGE_BINARY_FILEPATH))
else:
    print('file not found: %s' % FSW_IMAGE_BINARY_FILEPATH)
