#pragma once

typedef unsigned int uint;

#define one_block_size_bytes 64 // количество байт в блоке
#define one_block_size_uints 16 // количество 4байтовых  в блоке
#define block_expend_size_uints 80 // количество 4байтовых в дополненном блоке
#define SHA1HASHLENGTHBYTES 20
#define SHA1HASHLENGTHUINTS 5

typedef uint* Block;
typedef uint ExpendBlock[block_expend_size_uints];

const uint H[5] = { // константы, согласно стандарту
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
    0xC3D2E1F0
}; 

uint cycle_shift_left(uint val, int bit_count);
uint bring_to_human_view(uint val);

uint* sha1(const char* message, uint msize_bytes); // отданный массив нужно удалить вручную
