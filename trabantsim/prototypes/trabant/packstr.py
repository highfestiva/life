#!/usr/bin/env python3

from sys import byteorder
from zlib import compress,decompress


def stringify(b):
    i = int().from_bytes(b, byteorder)
    s = ''
    while i:
        j = i%95
        s += chr(32+j)
        i //= 95
    return s

def unstringify(s):
    i = 0
    for c in reversed(s):
        if ord(c) < 32:
            continue
        i *= 95
        i += ord(c)-32
    b = i.to_bytes((i.bit_length()+7)//8, byteorder)
    return b

def chrfix_single(c):
    return c if c not in "'\\" else '\\'+c

def chrfix_triple(c):
    return c if c != "\\" else '\\\\'

def pack(d):
    b = compress(d if type(d)==bytes else str(d).encode())
    return stringify(b)

def packr(d):
    '''Converts to single quote string.'''
    return "'" + ''.join(chrfix_single(c) for c in pack(d)) + "'"

def packrr(d):
    '''Converts to triple quote string.'''
    return "'''" + ''.join(chrfix_triple(c) for c in pack(d)) + "'''"

def unpack(s):
    b = unstringify(s)
    return decompress(b).decode()


if __name__ == '__main__':
    assert unstringify(stringify(b'something')) == b'something'
    assert unpack(pack('packed2bits')) == 'packed2bits'
    print('small string ratio:', len(pack('a very tiny string'))/len('a very tiny string'))
    print(packr('var = [3,4,92]'))
