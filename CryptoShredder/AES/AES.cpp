#include "AES.h"


//set default values of Nk and Nr to AES-128
size_t AES::Nk = 4; //number of 32-bit words in the key
size_t AES::Nr = 10; //number of rounds (AES-128 has 10 rounds, AES-192 has 12 rounds, AES-256 has 14 rounds)


/**
 * @brief • Represents the SBOX table of AES encryption.
 */
const unsigned char AES::SBOX[16][16] = {
  //  _0    _1    _2    _3    _4    _5    _6    _7    _8    _9    _A    _B    _C    _D    _E    _F    //
    {0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76}, //_0
    {0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0}, //_1
    {0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15}, //_2
    {0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75}, //_3
    {0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84}, //_4
    {0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF}, //_5
    {0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8}, //_6
    {0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2}, //_7
    {0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73}, //_8
    {0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB}, //_9
    {0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79}, //_A
    {0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08}, //_B
    {0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A}, //_C
    {0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E}, //_D
    {0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF}, //_E
    {0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16}  //_F
};


/**
 * @brief • Represents the inverse SBOX table of AES encryption.
 */
const unsigned char AES::INVSBOX[16][16] = {
  //  _0    _1    _2    _3    _4    _5    _6    _7    _8    _9    _A    _B    _C    _D    _E    _F    //
    {0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB}, //_0
    {0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB}, //_1
    {0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E}, //_2
    {0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25}, //_3
    {0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92}, //_4
    {0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84}, //_5
    {0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06}, //_6
    {0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B}, //_7
    {0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73}, //_8
    {0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E}, //_9
    {0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B}, //_A
    {0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4}, //_B
    {0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F}, //_C
    {0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF}, //_D
    {0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61}, //_E
    {0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D}  //_F
};


/**
 * @brief • Galois multiplication tables for AES MixColumns function.
 */
const unsigned char AES::GaloisMult[15][256] = {
    {}, {},

    {0x00, 0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E,
     0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C, 0x3E,
     0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5A, 0x5C, 0x5E,
     0x60, 0x62, 0x64, 0x66, 0x68, 0x6A, 0x6C, 0x6E, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7A, 0x7C, 0x7E,
     0x80, 0x82, 0x84, 0x86, 0x88, 0x8A, 0x8C, 0x8E, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9A, 0x9C, 0x9E,
     0xA0, 0xA2, 0xA4, 0xA6, 0xA8, 0xAA, 0xAC, 0xAE, 0xB0, 0xB2, 0xB4, 0xB6, 0xB8, 0xBA, 0xBC, 0xBE,
     0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2, 0xD4, 0xD6, 0xD8, 0xDA, 0xDC, 0xDE,
     0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEC, 0xEE, 0xF0, 0xF2, 0xF4, 0xF6, 0xF8, 0xFA, 0xFC, 0xFE,
     0x1B, 0x19, 0x1F, 0x1D, 0x13, 0x11, 0x17, 0x15, 0x0B, 0x09, 0x0F, 0x0D, 0x03, 0x01, 0x07, 0x05,
     0x3B, 0x39, 0x3F, 0x3D, 0x33, 0x31, 0x37, 0x35, 0x2B, 0x29, 0x2F, 0x2D, 0x23, 0x21, 0x27, 0x25,
     0x5B, 0x59, 0x5F, 0x5D, 0x53, 0x51, 0x57, 0x55, 0x4B, 0x49, 0x4F, 0x4D, 0x43, 0x41, 0x47, 0x45,
     0x7B, 0x79, 0x7F, 0x7D, 0x73, 0x71, 0x77, 0x75, 0x6B, 0x69, 0x6F, 0x6D, 0x63, 0x61, 0x67, 0x65,
     0x9B, 0x99, 0x9F, 0x9D, 0x93, 0x91, 0x97, 0x95, 0x8B, 0x89, 0x8F, 0x8D, 0x83, 0x81, 0x87, 0x85,
     0xBB, 0xB9, 0xBF, 0xBD, 0xB3, 0xB1, 0xB7, 0xB5, 0xAB, 0xA9, 0xAF, 0xAD, 0xA3, 0xA1, 0xA7, 0xA5,
     0xDB, 0xD9, 0xDF, 0xDD, 0xD3, 0xD1, 0xD7, 0xD5, 0xCB, 0xC9, 0xCF, 0xCD, 0xC3, 0xC1, 0xC7, 0xC5,
     0xFB, 0xF9, 0xFF, 0xFD, 0xF3, 0xF1, 0xF7, 0xF5, 0xEB, 0xE9, 0xEF, 0xED, 0xE3, 0xE1, 0xE7, 0xE5},

    {0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09, 0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11,
     0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39, 0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21,
     0x60, 0x63, 0x66, 0x65, 0x6C, 0x6F, 0x6A, 0x69, 0x78, 0x7B, 0x7E, 0x7D, 0x74, 0x77, 0x72, 0x71,
     0x50, 0x53, 0x56, 0x55, 0x5C, 0x5F, 0x5A, 0x59, 0x48, 0x4B, 0x4E, 0x4D, 0x44, 0x47, 0x42, 0x41,
     0xC0, 0xC3, 0xC6, 0xC5, 0xCC, 0xCF, 0xCA, 0xC9, 0xD8, 0xDB, 0xDE, 0xDD, 0xD4, 0xD7, 0xD2, 0xD1,
     0xF0, 0xF3, 0xF6, 0xF5, 0xFC, 0xFF, 0xFA, 0xF9, 0xE8, 0xEB, 0xEE, 0xED, 0xE4, 0xE7, 0xE2, 0xE1,
     0xA0, 0xA3, 0xA6, 0xA5, 0xAC, 0xAF, 0xAA, 0xA9, 0xB8, 0xBB, 0xBE, 0xBD, 0xB4, 0xB7, 0xB2, 0xB1,
     0x90, 0x93, 0x96, 0x95, 0x9C, 0x9F, 0x9A, 0x99, 0x88, 0x8B, 0x8E, 0x8D, 0x84, 0x87, 0x82, 0x81,
     0x9B, 0x98, 0x9D, 0x9E, 0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8F, 0x8C, 0x89, 0x8A,
     0xAB, 0xA8, 0xAD, 0xAE, 0xA7, 0xA4, 0xA1, 0xA2, 0xB3, 0xB0, 0xB5, 0xB6, 0xBF, 0xBC, 0xB9, 0xBA,
     0xFB, 0xF8, 0xFD, 0xFE, 0xF7, 0xF4, 0xF1, 0xF2, 0xE3, 0xE0, 0xE5, 0xE6, 0xEF, 0xEC, 0xE9, 0xEA,
     0xCB, 0xC8, 0xCD, 0xCE, 0xC7, 0xC4, 0xC1, 0xC2, 0xD3, 0xD0, 0xD5, 0xD6, 0xDF, 0xDC, 0xD9, 0xDA,
     0x5B, 0x58, 0x5D, 0x5E, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46, 0x4F, 0x4C, 0x49, 0x4A,
     0x6B, 0x68, 0x6D, 0x6E, 0x67, 0x64, 0x61, 0x62, 0x73, 0x70, 0x75, 0x76, 0x7F, 0x7C, 0x79, 0x7A,
     0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A,
     0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A},

    {}, {}, {}, {}, {},

    {0x00, 0x09, 0x12, 0x1B, 0x24, 0x2D, 0x36, 0x3F, 0x48, 0x41, 0x5A, 0x53, 0x6C, 0x65, 0x7E, 0x77,
     0x90, 0x99, 0x82, 0x8B, 0xB4, 0xBD, 0xA6, 0xAF, 0xD8, 0xD1, 0xCA, 0xC3, 0xFC, 0xF5, 0xEE, 0xE7,
     0x3B, 0x32, 0x29, 0x20, 0x1F, 0x16, 0x0D, 0x04, 0x73, 0x7A, 0x61, 0x68, 0x57, 0x5E, 0x45, 0x4C,
     0xAB, 0xA2, 0xB9, 0xB0, 0x8F, 0x86, 0x9D, 0x94, 0xE3, 0xEA, 0xF1, 0xF8, 0xC7, 0xCE, 0xD5, 0xDC,
     0x76, 0x7F, 0x64, 0x6D, 0x52, 0x5B, 0x40, 0x49, 0x3E, 0x37, 0x2C, 0x25, 0x1A, 0x13, 0x08, 0x01,
     0xE6, 0xEF, 0xF4, 0xFD, 0xC2, 0xCB, 0xD0, 0xD9, 0xAE, 0xA7, 0xBC, 0xB5, 0x8A, 0x83, 0x98, 0x91,
     0x4D, 0x44, 0x5F, 0x56, 0x69, 0x60, 0x7B, 0x72, 0x05, 0x0C, 0x17, 0x1E, 0x21, 0x28, 0x33, 0x3A,
     0xDD, 0xD4, 0xCF, 0xC6, 0xF9, 0xF0, 0xEB, 0xE2, 0x95, 0x9C, 0x87, 0x8E, 0xB1, 0xB8, 0xA3, 0xAA,
     0xEC, 0xE5, 0xFE, 0xF7, 0xC8, 0xC1, 0xDA, 0xD3, 0xA4, 0xAD, 0xB6, 0xBF, 0x80, 0x89, 0x92, 0x9B,
     0x7C, 0x75, 0x6E, 0x67, 0x58, 0x51, 0x4A, 0x43, 0x34, 0x3D, 0x26, 0x2F, 0x10, 0x19, 0x02, 0x0B,
     0xD7, 0xdE, 0xC5, 0xCC, 0xF3, 0xfA, 0xE1, 0xE8, 0x9F, 0x96, 0x8D, 0x84, 0xBB, 0xB2, 0xA9, 0xA0,
     0x47, 0x4E, 0x55, 0x5C, 0x63, 0x6A, 0x71, 0x78, 0x0F, 0x06, 0x1D, 0x14, 0x2B, 0x22, 0x39, 0x30,
     0x9A, 0x93, 0x88, 0x81, 0xBE, 0xB7, 0xAC, 0xA5, 0xD2, 0xDB, 0xC0, 0xC9, 0xF6, 0xFF, 0xE4, 0xED,
     0x0A, 0x03, 0x18, 0x11, 0x2E, 0x27, 0x3C, 0x35, 0x42, 0x4B, 0x50, 0x59, 0x66, 0x6F, 0x74, 0x7D,
     0xA1, 0xA8, 0xB3, 0xBA, 0x85, 0x8C, 0x97, 0x9E, 0xE9, 0xE0, 0xFB, 0xF2, 0xCD, 0xC4, 0xDF, 0xD6,
     0x31, 0x38, 0x23, 0x2A, 0x15, 0x1C, 0x07, 0x0E, 0x79, 0x70, 0x6B, 0x62, 0x5D, 0x54, 0x4F, 0x46},

    {},

    {0x00, 0x0B, 0x16, 0x1D, 0x2C, 0x27, 0x3A, 0x31, 0x58, 0x53, 0x4E, 0x45, 0x74, 0x7F, 0x62, 0x69,
     0xB0, 0xBB, 0xA6, 0xAD, 0x9C, 0x97, 0x8A, 0x81, 0xE8, 0xE3, 0xFE, 0xF5, 0xC4, 0xCF, 0xD2, 0xD9,
     0x7B, 0x70, 0x6D, 0x66, 0x57, 0x5C, 0x41, 0x4A, 0x23, 0x28, 0x35, 0x3E, 0x0F, 0x04, 0x19, 0x12,
     0xCB, 0xC0, 0xDD, 0xD6, 0xE7, 0xEC, 0xF1, 0xFA, 0x93, 0x98, 0x85, 0x8E, 0xBF, 0xB4, 0xA9, 0xA2,
     0xF6, 0xFD, 0xE0, 0xEB, 0xDA, 0xD1, 0xCC, 0xC7, 0xAE, 0xA5, 0xB8, 0xB3, 0x82, 0x89, 0x94, 0x9F,
     0x46, 0x4D, 0x50, 0x5B, 0x6A, 0x61, 0x7C, 0x77, 0x1E, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2F,
     0x8D, 0x86, 0x9B, 0x90, 0xA1, 0xAA, 0xB7, 0xBC, 0xD5, 0xDE, 0xC3, 0xC8, 0xF9, 0xF2, 0xEF, 0xE4,
     0x3D, 0x36, 0x2B, 0x20, 0x11, 0x1A, 0x07, 0x0C, 0x65, 0x6E, 0x73, 0x78, 0x49, 0x42, 0x5F, 0x54,
     0xF7, 0xFC, 0xE1, 0xEA, 0xDB, 0xD0, 0xCD, 0xC6, 0xAF, 0xA4, 0xB9, 0xB2, 0x83, 0x88, 0x95, 0x9E,
     0x47, 0x4C, 0x51, 0x5A, 0x6B, 0x60, 0x7D, 0x76, 0x1F, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2E,
     0x8C, 0x87, 0x9A, 0x91, 0xA0, 0xAB, 0xB6, 0xBD, 0xD4, 0xDF, 0xC2, 0xC9, 0xF8, 0xF3, 0xEE, 0xE5,
     0x3C, 0x37, 0x2A, 0x21, 0x10, 0x1B, 0x06, 0x0D, 0x64, 0x6F, 0x72, 0x79, 0x48, 0x43, 0x5E, 0x55,
     0x01, 0x0A, 0x17, 0x1C, 0x2D, 0x26, 0x3B, 0x30, 0x59, 0x52, 0x4F, 0x44, 0x75, 0x7E, 0x63, 0x68,
     0xB1, 0xBA, 0xA7, 0xAC, 0x9D, 0x96, 0x8B, 0x80, 0xE9, 0xE2, 0xFF, 0xF4, 0xC5, 0xCE, 0xD3, 0xD8,
     0x7A, 0x71, 0x6C, 0x67, 0x56, 0x5D, 0x40, 0x4B, 0x22, 0x29, 0x34, 0x3F, 0x0E, 0x05, 0x18, 0x13,
     0xCA, 0xC1, 0xDC, 0xD7, 0xE6, 0xED, 0xF0, 0xFB, 0x92, 0x99, 0x84, 0x8F, 0xBE, 0xB5, 0xA8, 0xA3},

    {},

    {0x00, 0x0D, 0x1A, 0x17, 0x34, 0x39, 0x2E, 0x23, 0x68, 0x65, 0x72, 0x7F, 0x5C, 0x51, 0x46, 0x4B,
     0xD0, 0xDD, 0xCA, 0xC7, 0xE4, 0xE9, 0xFE, 0xF3, 0xB8, 0xB5, 0xA2, 0xAF, 0x8C, 0x81, 0x96, 0x9B,
     0xBB, 0xB6, 0xA1, 0xAC, 0x8F, 0x82, 0x95, 0x98, 0xD3, 0xDE, 0xC9, 0xC4, 0xE7, 0xEA, 0xFD, 0xF0,
     0x6B, 0x66, 0x71, 0x7C, 0x5F, 0x52, 0x45, 0x48, 0x03, 0x0E, 0x19, 0x14, 0x37, 0x3A, 0x2D, 0x20,
     0x6D, 0x60, 0x77, 0x7A, 0x59, 0x54, 0x43, 0x4E, 0x05, 0x08, 0x1F, 0x12, 0x31, 0x3C, 0x2B, 0x26,
     0xBD, 0xB0, 0xA7, 0xAA, 0x89, 0x84, 0x93, 0x9E, 0xD5, 0xD8, 0xCF, 0xC2, 0xE1, 0xEC, 0xFB, 0xF6,
     0xD6, 0xDB, 0xCC, 0xC1, 0xE2, 0xEF, 0xF8, 0xF5, 0xBE, 0xB3, 0xA4, 0xA9, 0x8A, 0x87, 0x90, 0x9D,
     0x06, 0x0B, 0x1C, 0x11, 0x32, 0x3F, 0x28, 0x25, 0x6E, 0x63, 0x74, 0x79, 0x5A, 0x57, 0x40, 0x4D,
     0xDA, 0xD7, 0xC0, 0xCD, 0xEE, 0xE3, 0xF4, 0xF9, 0xB2, 0xBF, 0xA8, 0xA5, 0x86, 0x8B, 0x9C, 0x91,
     0x0A, 0x07, 0x10, 0x1D, 0x3E, 0x33, 0x24, 0x29, 0x62, 0x6F, 0x78, 0x75, 0x56, 0x5B, 0x4C, 0x41,
     0x61, 0x6C, 0x7B, 0x76, 0x55, 0x58, 0x4F, 0x42, 0x09, 0x04, 0x13, 0x1E, 0x3D, 0x30, 0x27, 0x2A,
     0xB1, 0xBC, 0xAB, 0xA6, 0x85, 0x88, 0x9F, 0x92, 0xD9, 0xD4, 0xC3, 0xCE, 0xED, 0xE0, 0xF7, 0xFA,
     0xB7, 0xBA, 0xAD, 0xA0, 0x83, 0x8E, 0x99, 0x94, 0xDF, 0xD2, 0xC5, 0xC8, 0xEB, 0xE6, 0xF1, 0xFC,
     0x67, 0x6A, 0x7D, 0x70, 0x53, 0x5E, 0x49, 0x44, 0x0F, 0x02, 0x15, 0x18, 0x3B, 0x36, 0x21, 0x2C,
     0x0C, 0x01, 0x16, 0x1B, 0x38, 0x35, 0x22, 0x2F, 0x64, 0x69, 0x7E, 0x73, 0x50, 0x5D, 0x4A, 0x47,
     0xDC, 0xD1, 0xC6, 0xCB, 0xE8, 0xE5, 0xF2, 0xFF, 0xB4, 0xB9, 0xAE, 0xA3, 0x80, 0x8D, 0x9A, 0x97},
    
    {0x00, 0x0E, 0x1C, 0x12, 0x38, 0x36, 0x24, 0x2A, 0x70, 0x7E, 0x6C, 0x62, 0x48, 0x46, 0x54, 0x5A,
     0xE0, 0xEE, 0xFC, 0xF2, 0xD8, 0xD6, 0xC4, 0xCA, 0x90, 0x9E, 0x8C, 0x82, 0xA8, 0xA6, 0xB4, 0xBA,
     0xDB, 0xD5, 0xC7, 0xC9, 0xE3, 0xED, 0xFF, 0xF1, 0xAB, 0xA5, 0xB7, 0xB9, 0x93, 0x9D, 0x8F, 0x81,
     0x3B, 0x35, 0x27, 0x29, 0x03, 0x0D, 0x1F, 0x11, 0x4B, 0x45, 0x57, 0x59, 0x73, 0x7D, 0x6F, 0x61,
     0xAD, 0xA3, 0xB1, 0xBF, 0x95, 0x9B, 0x89, 0x87, 0xDD, 0xD3, 0xC1, 0xCF, 0xE5, 0xEB, 0xF9, 0xF7,
     0x4D, 0x43, 0x51, 0x5F, 0x75, 0x7B, 0x69, 0x67, 0x3D, 0x33, 0x21, 0x2F, 0x05, 0x0B, 0x19, 0x17,
     0x76, 0x78, 0x6A, 0x64, 0x4E, 0x40, 0x52, 0x5C, 0x06, 0x08, 0x1A, 0x14, 0x3E, 0x30, 0x22, 0x2C,
     0x96, 0x98, 0x8A, 0x84, 0xAE, 0xA0, 0xB2, 0xBC, 0xE6, 0xE8, 0xFA, 0xF4, 0xDE, 0xD0, 0xC2, 0xCC,
     0x41, 0x4F, 0x5D, 0x53, 0x79, 0x77, 0x65, 0x6B, 0x31, 0x3F, 0x2D, 0x23, 0x09, 0x07, 0x15, 0x1B,
     0xA1, 0xAF, 0xBD, 0xB3, 0x99, 0x97, 0x85, 0x8B, 0xD1, 0xDF, 0xCD, 0xC3, 0xE9, 0xE7, 0xF5, 0xFB,
     0x9A, 0x94, 0x86, 0x88, 0xA2, 0xAC, 0xBE, 0xB0, 0xEA, 0xE4, 0xF6, 0xF8, 0xD2, 0xDC, 0xCE, 0xC0,
     0x7A, 0x74, 0x66, 0x68, 0x42, 0x4C, 0x5E, 0x50, 0x0A, 0x04, 0x16, 0x18, 0x32, 0x3C, 0x2E, 0x20,
     0xEC, 0xE2, 0xF0, 0xFE, 0xD4, 0xDA, 0xC8, 0xC6, 0x9C, 0x92, 0x80, 0x8E, 0xA4, 0xAA, 0xB8, 0xB6,
     0x0C, 0x02, 0x10, 0x1E, 0x34, 0x3A, 0x28, 0x26, 0x7C, 0x72, 0x60, 0x6E, 0x44, 0x4A, 0x58, 0x56,
     0x37, 0x39, 0x2B, 0x25, 0x0F, 0x01, 0x13, 0x1D, 0x47, 0x49, 0x5B, 0x55, 0x7F, 0x71, 0x63, 0x6D,
     0xD7, 0xD9, 0xCB, 0xC5, 0xEF, 0xE1, 0xF3, 0xFD, 0xA7, 0xA9, 0xBB, 0xB5, 0x9F, 0x91, 0x83, 0x8D}
};


/**
 * @brief • Function that handles the operation mode of AES encryption.
 * @param • size_t keySize
 * @throws • invalid_argument thrown if given keySize is invalid.
 */
void AES::SetOperationMode(const size_t keySize) {
    if (keySize == 16) { //if keySize is 16 bytes (128 bits)
        Nk = 4; //number of 32-bit words in the key (AES-128)
        Nr = 10; //number of rounds (AES-128 has 10 rounds)
    }
    else if (keySize == 24) { //if keySize is 24 bytes (192 bits)
        Nk = 6; //number of 32-bit words in the key (AES-192)
        Nr = 12; //number of rounds (AES-192 has 12 rounds)
    }
    else if (keySize == 32) { //if keySize is 32 bytes (256 bits)
        Nk = 8; //number of 32-bit words in the key (AES-256)
        Nr = 14; //number of rounds (AES-256 has 14 rounds)
    }
    else //else keySize is invalid
        throw invalid_argument("Invalid mode of operation, please provide valid key that matches AES requirements."); //we throw invalid argument 
}


/**
 * @brief • Function for creating a vector of random bytes.
 * @param • size_t vecSize
 * @return • vector<unsigned char> vec
 */
vector<unsigned char> AES::Create_Vector(const size_t vecSize) {
	random_device randomDevice; //create random device for generating random bytes
    vector<unsigned char> vec(vecSize); //create vector of desired size
    for (size_t i = 0; i < vecSize; i++) //iterate over the vector
        vec[i] = (unsigned char)(randomDevice() & 0xFF); //insert random byte to vector using random device
    return vec; //return the vector
}


/**
 * @brief • Function for creating an AES key.
 * @brief • Supports AES-128, AES-192 and AES-256 keys.
 * @brief • Returns default AES-128 key if no arguments given.
 * @param • size_t keySize
 * @return • vector<unsigned char> key
 */
vector<unsigned char> AES::Create_Key(const size_t keySize) {
    vector<unsigned char> key; //create key vector
    if (keySize == 128 || keySize == 192 || keySize == 256) //if keySize is valid AES key size
        key = Create_Vector(keySize / (Nb * 2)); //we create key vector with keySize
    else //else keySize isn't valid AES key size
        key = Create_Vector(128 / (Nb * 2)); //we create key vector with AES-128 key size
	return key; //return key vector
}


/**
 * @brief • Function for creating an initialization vector.
 * @brief • Supports zero padding for various IV sizes.
 * @brief • Returns default AES IV if no arguments given.
 * @param • size_t ivSize
 * @return • vector<unsigned char> iv
 */
vector<unsigned char> AES::Create_IV(const size_t ivSize) {
	vector<unsigned char> iv; //create IV vector
	if (ivSize <= BlockSize) //if ivSize is valid AES IV size
		iv = Create_Vector(ivSize); //we create IV vector with given ivSize
	else //else ivSize isn't valid AES IV size
        iv = Create_Vector(BlockSize); //we create IV vector with BlockSize
    iv.resize(BlockSize, 0x00); //resize IV vector to BlockSize and add zero padding if necessary
	return iv; //return IV vector
}


/**
 * @brief • Function for converting vector of bytes to hexadecimal string representation.
 * @param • vector<unsigned char> vec
 * @return • string hex
 */
string AES::VectorToHex(const vector<unsigned char>& vec) {
    const string hexChars = "0123456789abcdef"; //hex characters for conversion
	string result; //create result string
    result.reserve(vec.size() * 2); //reserve memory for result string
	for (const unsigned char& byte : vec) { //iterate through each byte in the vector
		result.push_back(hexChars[byte >> 4]); //append the left 4 bits as a hex character
		result.push_back(hexChars[byte & 0x0F]); //append the right 4 bits as a hex character
    }
	return result; //return the result string
}


/**
 * @brief • Function for converting hexadecimal string to vector of bytes representation.
 * @param • string hex
 * @return • vector<unsigned char> vec
 * @throws • invalid_argument thrown if given hex is invalid.
 */
vector<unsigned char> AES::HexToVector(const string& hex) {
	if (hex.size() % 2 != 0) //if hex string has odd length
        throw invalid_argument("Hex string must have even length."); //throw invalid argument
	vector<unsigned char> result; //create result vector
	result.reserve(hex.size() / 2); //reserve memory for result vector
    for (size_t i = 0; i < hex.size(); i += 2) { //iterate through each hex string two characters at a time
        unsigned char byte = (unsigned char)stoi(hex.substr(i, 2), NULL, 16); //convert substring of two chars to a byte
        result.push_back(byte); //append the byte to result vector
    }
	return result; //return the result vector
}


/**
 * @brief • Function for clearing a vector securly.
 * @param • vector<unsigned char> vec
 */
void AES::ClearVector(vector<unsigned char>& vec) {
    fill(vec.begin(), vec.end(), 0x00); //fill vector with zeros 
    vec.clear(); //clear vector for added security after finishing operations
}


/**
 * @brief • Function for clearing a vector of vectors securly.
 * @param • vector<vector<unsigned char>> vec
 */
void AES::ClearVector(vector<vector<unsigned char>>& vec) {
    for (vector<unsigned char>& innerVec : vec) //iterate through each innerVec in vector
        ClearVector(innerVec); //clear each innerVec 
    vec.clear(); //clear vector for added security after finishing operations
}


/**
 * @brief • Function for printing a vector of bytes as characters.
 * @param • vector<unsigned char> vec
 */
void AES::PrintVector(const vector<unsigned char>& vec) {
    for (const unsigned char& byte : vec) //iterate through each byte in vector
        cout << byte; //print byte as character
    cout << endl; //new line
}


/**
 * @brief • Function for printing a vector of vectors of bytes as characters.
 * @param • vector<vector<unsigned char>> vec
 */
void AES::PrintVector(const vector<vector<unsigned char>>& vec) {
    for (const vector<unsigned char>& innerVec : vec) //iterate through each innerVec in vector
        PrintVector(innerVec); //print innerVec as characters
    cout << endl; //new line
}


/**
 * @brief • Function that rotates a vector element (byte) to the left.
 * @param • unsigned char* word
 * @return • unsigned char* rotatedWord
 */
unsigned char* AES::RotWord(unsigned char* word) {
    if (word != NULL) { //if word not null
        unsigned char temp = word[0]; //save first value of word
        //shift the word to the left by one position
        word[0] = word[1];
        word[1] = word[2];
        word[2] = word[3];
        word[3] = temp;
    }
    return word; //return new rotated word
}


/**
 * @brief • Function that substitutes each byte in a word using the SBOX.
 * @param • unsigned char* word
 * @return • unsigned char* subWord
 */
unsigned char* AES::SubWord(unsigned char* word) {
    if (word != NULL) { //if word not null
        for (size_t i = 0; i < Nb; i++) { //iterate over the word
            //we set the value from SBOX with right rotating by 4 to extract the left value and OR with 0x0F for extracting the right value
            word[i] = SBOX[word[i] >> 4][word[i] & 0x0F]; //set the value from the SBOX
        }
    }
    return word; //return new word with SBOX values
}


/**
 * @brief • Function that returns the Rcon value given original value for key schedule.
 * @param • unsigned char value
 * @return • unsigned char rconValue
 */
unsigned char AES::Rcon(unsigned char value) {
	if (value == 0) return 0x00; //if value is 0 we return 0x00
    unsigned char rconValue = 0x01; //initialize with 0x01 (first round constant)
    for (size_t i = 1; i < value; i++) {
        if (rconValue & 0x80) //if the leftmost bit (0x80) is set
            rconValue = (rconValue << 1) ^ 0x1B; //XOR with 0x1B after left shifting
        else //else we left shift 
            rconValue = rconValue << 1; //left shift by one
    }
    return rconValue; //return rcon value
}


/**
 * @brief • Function for substitute bytes in AES encryption, both for encryption and decryption.
 * @param • unsigned char* state
 * @param • bool inverse
 * @return • unsigned char* subState
 */
unsigned char* AES::SubBytes(unsigned char* state, const bool inverse) {
    if (state != NULL) { //if state not null
        if (!inverse) { //perform substitute bytes for encryption
            for (size_t i = 0; i < BlockSize; i++) //iterate over state array
                state[i] = SBOX[state[i] >> 4][state[i] & 0x0F]; //set correct value from SBOX
        }
        else { //perform substitute bytes for decryption
            for (size_t i = 0; i < BlockSize; i++) //iterate over state array
                state[i] = INVSBOX[state[i] >> 4][state[i] & 0x0F]; //set correct value from INVSBOX
        }
    }
    return state; //return new state array after substitute bytes
}


/**
 * @brief • Function for shifting rows in AES encryption, both for encryption and decryption.
 * @param • unsigned char* state
 * @param • bool inverse
 * @return • unsigned char* shiftedState
 */
unsigned char* AES::ShiftRows(unsigned char* state, const bool inverse) {
    if (state != NULL) { //if state not null
        if (!inverse) { //perform shift rows for encryption
            //swap elements in second row
            for (size_t i = 1; i <= 9; i += Nb) //iterate on second row
                swap(state[i], state[i + Nb]); //swap elements
            //swap elements in third row
            for (size_t i = 2; i <= 6; i += Nb) //iterate on third row
                swap(state[i], state[i + (Nb * 2)]); //swap elements
            //swap elements in fourth row
            for (size_t i = 15; i >= 7; i -= Nb) //iterate on fourth row
                swap(state[i], state[i - Nb]); //swap elements
        }
        else { //perform inverse shift rows for decryption
            //swap elements in second row
            for (size_t i = 13; i >= 5; i -= Nb) //iterate on second row
                swap(state[i], state[i - Nb]); // swap elements
            //swap elements in third row
            for (size_t i = 2; i <= 6; i += Nb) //iterate on third row
                swap(state[i], state[i + (Nb * 2)]); //swap elements
            //swap elements in fourth row
            for (size_t i = 3; i <= 11; i += Nb) //iterate on fourth row
                swap(state[i], state[i + Nb]); //swap elements
        }
    }
    return state; //return shifted state 
}


/**
 * @brief • Function for mixing columns using GaloisMult tables for AES encryption, both for encryption and decryption.
 * @param • unsigned char* state
 * @param • bool inverse
 * @return • unsigned char* mixedState
 */
unsigned char* AES::MixColumns(unsigned char* state, const bool inverse) {
    if (state != NULL) { //if state not null
        unsigned char temp[Nb]{}; //temp array represents current column values 
        if (!inverse) { //perform mix columns for encryption
            for (size_t i = 0; i < BlockSize; i += Nb) { //iterate over state 
                copy(state + i, state + i + Nb, temp); //copy the columns values to temp array

                //apply Galois field equations for each byte in state array
                state[i + 0] = (unsigned char)(GaloisMult[2][temp[0]] ^ GaloisMult[3][temp[1]] ^ temp[2] ^ temp[3]);
                state[i + 1] = (unsigned char)(temp[0] ^ GaloisMult[2][temp[1]] ^ GaloisMult[3][temp[2]] ^ temp[3]);
                state[i + 2] = (unsigned char)(temp[0] ^ temp[1] ^ GaloisMult[2][temp[2]] ^ GaloisMult[3][temp[3]]);
                state[i + 3] = (unsigned char)(GaloisMult[3][temp[0]] ^ temp[1] ^ temp[2] ^ GaloisMult[2][temp[3]]);
            }
        }
        else { //perform mix columns for decryption
            for (size_t i = 0; i < BlockSize; i += Nb) { //iterate over state array
                copy(state + i, state + i + Nb, temp); //copy the columns values to temp array

                //apply Galois field equations for each byte in state array
                state[i + 0] = (unsigned char)(GaloisMult[14][temp[0]] ^ GaloisMult[11][temp[1]] ^ GaloisMult[13][temp[2]] ^ GaloisMult[9][temp[3]]);
                state[i + 1] = (unsigned char)(GaloisMult[9][temp[0]] ^ GaloisMult[14][temp[1]] ^ GaloisMult[11][temp[2]] ^ GaloisMult[13][temp[3]]);
                state[i + 2] = (unsigned char)(GaloisMult[13][temp[0]] ^ GaloisMult[9][temp[1]] ^ GaloisMult[14][temp[2]] ^ GaloisMult[11][temp[3]]);
                state[i + 3] = (unsigned char)(GaloisMult[11][temp[0]] ^ GaloisMult[13][temp[1]] ^ GaloisMult[9][temp[2]] ^ GaloisMult[14][temp[3]]);
            }
        }
    }
    return state; //return state after mix columns
}


/**
 * @brief • Function for XOR operation between two arrays in same size, modifies first array with XOR result.
 * @param • unsigned char* first
 * @param • unsigned char* second
 * @return • unsigned char* xorValue
 */
unsigned char* AES::XOR(unsigned char* first, const unsigned char* second) {
    if (first != NULL && second != NULL) { //if both arrays not null
        for (size_t i = 0; i < BlockSize; i++) //iterate over the arrays
            first[i] ^= second[i]; //perform XOR on arrays elements
    }
    return first; //return first array with XOR value
}


/**
 * @brief • Function for generating round keys for AES encryption, supports AES-128, AES-192 and AES-256.
 * @param • vector<unsigned char> key
 * @return • vector<vector<unsigned char>> roundKeys
 */
vector<vector<unsigned char>> AES::KeySchedule(const vector<unsigned char>& key) {
    vector<vector<unsigned char>> roundKeysMatrix; //represents round keys as matrix of vectors (each represented as a vector of unsigned char)
    vector<unsigned char> roundKeysVector(BlockSize * (Nr + 1)); //represents round keys as vector
    unsigned char temp[Nb]{}; //represents temporary keyword for key schedule operations
    roundKeysMatrix.reserve(Nr + 1); //reserve memory for our keys in advance for better performance

    for (size_t i = 0; i < Nb * Nk; i++) //add initial key to roundKeyVector
        roundKeysVector[i] = key[i]; //set each value for word

    //iterate over the round keys vector to generate round keys
    for (size_t i = Nb * Nk; i < BlockSize * (Nr + 1); i += Nb) {
        for (size_t j = 0; j < Nb; j++) //copy the last word from the previous round key to temp
            temp[j] = roundKeysVector[i - Nb + j]; //save each word's value in temp

        if (i / Nb % Nk == 0) {  //if we are at the beginning of a new set of Nk words, we apply RotWord, SubWord and XOR with Rcon value
            RotWord(temp); //apply RotWord operation on current word 
            SubWord(temp); //apply SubWord operation on current word 
            temp[0] ^= Rcon((unsigned char)(i / (Nb * Nk))); //XOR current word with Rcon value
        }
        else if (Nk > 6 && i / Nb % Nk == Nb) //for AES-256 we need to apply SubWord again for added security half way of the generation
            SubWord(temp); //apply the SubWord operation again for AES-256

        for (size_t j = 0; j < Nb; j++) //combine the previous round key with the transformed word to generate the new round key
            roundKeysVector[i + j] = roundKeysVector[i - Nb * Nk + j] ^ temp[j]; //XOR current word temp with word from the previous round key
    }

    for (size_t i = 0; i < BlockSize * (Nr + 1); i += BlockSize) //iterate over roundKeysVector and initialize roundKeyMatrix
        roundKeysMatrix.emplace_back(roundKeysVector.begin() + i, roundKeysVector.begin() + i + BlockSize); //add each key into roundKeysMatrix
    ClearVector(roundKeysVector); //clear roundKeysVector for added security after we finish operations

    return roundKeysMatrix; //return our roundKeysMatrix for AES operation
}


/**
 * @brief • Function that performs AES encryption on given text using specified round keys, supports AES-128, AES-192 and AES-256.
 * @brief • This function performs AES encryption with fixed block size of 16 bytes (128-bit).
 * @param • unsigned char* text
 * @param • vector<vector<unsigned char>> roundKeys
 * @return • unsigned char* cipherText
 */
unsigned char* AES::EncryptBlock(unsigned char* text, const vector<vector<unsigned char>>& roundKeys) {
    if (text != NULL) { //if text not null
        //apply initial round key
        XOR(text, roundKeys[0].data()); //perform first AddRoundKey operation on text
        //apply AES operations of SubByte, ShiftRows, MixColumns and AddRoundKey
        for (size_t i = 1; i < Nr; i++) { //iterate over roundKeys and apply AES operations
            SubBytes(text, false); //perform SubBytes operation on text
            ShiftRows(text, false); //perform ShiftRows operation on text
            MixColumns(text, false); //perform MixColumns operation on text
            XOR(text, roundKeys[i].data()); //perform AddRoundKey operation on text
        }
        //apply AES final round operations SubBytes, ShiftRows and AddRoundKey
        SubBytes(text, false); //perform SubBytes operation on text
        ShiftRows(text, false); //perform ShiftRows operation on text
        XOR(text, roundKeys[Nr].data()); //perform AddRoundKey operation on text 
    }
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption on given text using specified round keys, supports AES-128, AES-192 and AES-256.
 * @brief • This function performs AES decryption with fixed block size of 16 bytes (128-bit).
 * @param • unsigned char* text
 * @param • vector<vector<unsigned char>> roundKeys
 * @return • unsigned char* decipherText
 */
unsigned char* AES::DecryptBlock(unsigned char* text, const vector<vector<unsigned char>>& roundKeys) {
    if (text != NULL) { //if text not null
        //apply AES final round operations in reverse order of AddRoundKey, ShiftRows and SubBytes
        XOR(text, roundKeys[Nr].data()); //perform AddRoundKey operation on text
        ShiftRows(text, true); //perform ShiftRows operation on text
        SubBytes(text, true); //perform SubBytes operation on text
        //apply AES operations in reverse order of AddRoundKey, MixColumns, ShiftRows and SubBytes
        for (size_t i = Nr - 1; i >= 1; i--) { //iterate over roundKeys in reverse order and apply AES operations
            XOR(text, roundKeys[i].data()); //perform AddRoundKey operation on text
            MixColumns(text, true); //perform MixColumns operation on text
            ShiftRows(text, true); //perform ShiftRows operation on text
            SubBytes(text, true); //perform SubBytes operation on text
        }
        //apply initial round key
        XOR(text, roundKeys[0].data()); //perform AddRoundKey operation on text
    }
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption on given text using specified key, supports AES-128, AES-192 and AES-256.
 * @brief • This function performs AES encryption with fixed block size of 16 bytes (128-bit).
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 */
vector<unsigned char>& AES::Encrypt(vector<unsigned char>& text, const vector<unsigned char>& key) {
    if (text.size() != BlockSize) //if plaintext isn't valid we throw invalid argument
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES requirements."); //throw invalid argument
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    EncryptBlock(text.data(), roundKeys); //call our AES EncryptBlock function for encrypting text using round keys
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption on given text using specified key, supports AES-128, AES-192 and AES-256.
 * @brief • This function performs AES decryption with fixed block size of 16 bytes (128-bit).
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 */
vector<unsigned char>& AES::Decrypt(vector<unsigned char>& text, const vector<unsigned char>& key) {
    if (text.size() != BlockSize) //if plaintext isn't valid we throw invalid argument
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES requirements."); //throw invalid argument
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    DecryptBlock(text.data(), roundKeys); //call our AES DecryptBlock function for decrypting text using round keys
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption in ECB mode on given text using specified key.
 * @brief • ECB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports PKCS7 padding.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 */
vector<unsigned char>& AES::Encrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES ECB requirements."); //throw invalid argument
    if (text.size() % BlockSize != 0) { //if text size isn't multiply of 16 bytes we add padding
        unsigned char padding = BlockSize - (text.size() % BlockSize); //calculate the number of padding bytes needed
        text.insert(text.end(), padding, padding); //append the padding bytes to the text
    }
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        EncryptBlock(text.data() + i, roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations 
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption in ECB mode on given text using specified key.
 * @brief • ECB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports PKCS7 padding.
 * @brief • ECB decryption mode requires text to be a multiple of 16 bytes in length.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 */
vector<unsigned char>& AES::Decrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty() || text.size() % BlockSize != 0) //if plaintext is empty or plaintext size isn't multiply of 16 bytes
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES ECB requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        DecryptBlock(text.data() + i, roundKeys); //decrypt the block using our AES DecryptBlock function using round keys
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations 
    unsigned char padding = text.back(); //get the value of the last byte, which indicates the padding size
    if (padding > 0 && padding <= BlockSize && padding <= text.size()) { //if true we have padding bytes to remove from text
        for (size_t i = text.size(); i-- > text.size() - padding;) //check if last bytes match padding value
            if (text[i] != padding) return text; //if byte doesn't match padding value we return deciphered text
        text.resize(text.size() - padding); //remove the padding bytes from the text
    }
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption in CBC mode on given text using specified key and initialization vector.
 * @brief • CBC mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports PKCS7 padding.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Encrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CBC requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CBC requirements."); //throw invalid argument
    if (text.size() % BlockSize != 0) { //if text size isn't multiply of 16 bytes we add padding
        unsigned char padding = BlockSize - (text.size() % BlockSize); //calculate the number of padding bytes needed
        text.insert(text.end(), padding, padding); //append the padding bytes to the text
    }
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> currentCipher = iv; //initialize currentCipher vector with IV vector
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        XOR(text.data() + i, currentCipher.data()); //XOR with currentCipher block
        EncryptBlock(text.data() + i, roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
        copy(text.begin() + i, text.begin() + i + BlockSize, currentCipher.begin()); //update currentCipher block with previous block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption in CBC mode on given text using specified key and initialization vector.
 * @brief • CBC mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports PKCS7 padding.
 * @brief • CBC decryption mode requires text to be a multiple of 16 bytes in length.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Decrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty() || text.size() % BlockSize != 0) //if plaintext is empty or plaintext size isn't multiply of 16 bytes
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CBC requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CBC requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> currentCipher = iv; //initialize currentCipher vector with IV vector
    vector<unsigned char> previousCipher(BlockSize); //initialize previousCipher vector
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        copy(text.begin() + i, text.begin() + i + BlockSize, previousCipher.begin()); //save current block in previousCipher 
        DecryptBlock(text.data() + i, roundKeys); //decrypt the block using our AES DecryptBlock function using round keys
        XOR(text.data() + i, currentCipher.data()); //XOR with currentCipher block
        currentCipher = previousCipher; //update currentCipher block with previousCipher block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    unsigned char padding = text.back(); //get the value of the last byte, which indicates the padding size
    if (padding > 0 && padding <= BlockSize && padding <= text.size()) { //if true we have padding bytes to remove from text
        for (size_t i = text.size(); i-- > text.size() - padding;) //check if last bytes match padding value
            if (text[i] != padding) return text; //if byte doesn't match padding value we return deciphered text
        text.resize(text.size() - padding); //remove the padding bytes from the text
    }
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption in CFB mode on given text using specified key and initialization vector.
 * @brief • CFB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Encrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CFB requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CFB requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    vector<unsigned char> currentCipher(BlockSize); //initialize currentCipher vector
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentCipher 
            currentCipher = previousCipher; //set currentCipher vector to previousCipher block for encryption
            EncryptBlock(currentCipher.data(), roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for currentCipher back to zero to perform XOR operation 
        }
        text[i] ^= currentCipher[j]; //perform byte XOR between text and currentCipher block
        previousCipher[j] = text[i]; //update previousCipher with the new ciphered text
        j++; //increase j index for currentCipher block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption in CFB mode on given text using specified key and initialization vector.
 * @brief • CFB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Decrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CFB requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CFB requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    vector<unsigned char> currentCipher(BlockSize); //initialize currentCipher vector 
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentCipher 
            currentCipher = previousCipher; //set currentCipher vector to previousCipher block for decryption
            EncryptBlock(currentCipher.data(), roundKeys); //decrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for currentCipher back to zero to perform XOR operation 
        }
        previousCipher[j] = text[i]; //update previousCipher with the new deciphered text
        text[i] ^= currentCipher[j]; //perform byte XOR between text and currentCipher block
        j++; //increase j index for currentCipher block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption in OFB mode on given text using specified key and initialization vector.
 * @brief • OFB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Encrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES OFB requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES OFB requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> currentCipher = iv; //initialize currentCipher vector with IV vector
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentCipher 
            EncryptBlock(currentCipher.data(), roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for currentCipher back to zero to perform XOR operation 
        }
        text[i] ^= currentCipher[j]; //perform byte XOR between text and currentCipher block
        j++; //increase j index for currentCipher block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption in OFB mode on given text using specified key and initialization vector.
 * @brief • OFB mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Decrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES OFB requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES OFB requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> currentCipher = iv; //initialize currentCipher vector with IV vector
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentCipher 
            EncryptBlock(currentCipher.data(), roundKeys); //decrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for currentCipher back to zero to perform XOR operation 
        }
        text[i] ^= currentCipher[j]; //perform byte XOR between text and currentCipher block
        j++; //increase j index for currentCipher block
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return deciphered text
}


/**
 * @brief • Function that performs AES encryption in CTR mode on given text using specified key and initialization vector.
 * @brief • CTR mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> cipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Encrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CTR requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CTR requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousIV = iv; //initialize previousIV vector with IV vector
    vector<unsigned char> currentIV(BlockSize); //initialize currentIV vector
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentIV
            currentIV = previousIV; //set currentIV vector to previousIV vector for encryption
            EncryptBlock(currentIV.data(), roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for IV vector back to zero to perform XOR operation 
        }
        text[i] ^= currentIV[j]; //perform byte XOR between text and IV vector
        j++; //increase j index for IV vector
        //here we increase the counter for IV vector
        if (j == BlockSize) { //if j equals to block size 
            for (size_t k = BlockSize; k-- > BlockSize / 2;) //iterate over previousIV from end to start
                if (++previousIV[k]) break; //increment previousIV[k] and break if it's not zero
        }
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return ciphered text
}


/**
 * @brief • Function that performs AES decryption in CTR mode on given text using specified key and initialization vector.
 * @brief • CTR mode supports AES-128, AES-192 and AES-256.
 * @brief • Supports text in any size.
 * @param • vector<unsigned char> text
 * @param • vector<unsigned char> key
 * @param • vector<unsigned char> iv
 * @return • vector<unsigned char> decipherText
 * @throws • invalid_argument thrown if given text is invalid.
 * @throws • invalid_argument thrown if given key is invalid.
 * @throws • invalid_argument thrown if given iv is invalid.
 */
vector<unsigned char>& AES::Decrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws invalid argument if key invalid
    if (text.empty()) //if plaintext is empty
        throw invalid_argument("Invalid mode of operation, please provide valid plaintext that matches AES CTR requirements."); //throw invalid argument
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw invalid_argument("Invalid mode of operation, please provide valid initialization vector that matches AES CTR requirements."); //throw invalid argument
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousIV = iv; //initialize previousIV vector with IV vector
    vector<unsigned char> currentIV(BlockSize); //initialize currentIV vector
    for (size_t i = 0, j = 0; i < text.size(); i++) { //iterate over text
        if (j % BlockSize == 0) { //if we are in new cipher block we encrypt currentIV
            currentIV = previousIV; //set currentIV vector to previousIV vector for decryption
            EncryptBlock(currentIV.data(), roundKeys); //decrypt the block using our AES EncryptBlock function using round keys
            j = 0; //set the index for IV vector back to zero to perform XOR operation 
        }
        text[i] ^= currentIV[j]; //perform byte XOR between text and IV vector
        j++; //increase j index for IV vector
        //here we increase the counter for IV vector
        if (j == BlockSize) { //if j equals to block size 
            for (size_t k = BlockSize; k-- > BlockSize / 2;) //iterate over previousIV from end to start
                if (++previousIV[k]) break; //increment previousIV[k] and break if it's not zero
        }
    }
    ClearVector(roundKeys); //clear our roundKeys for added security after we finish operations
    return text; //return deciphered text
}