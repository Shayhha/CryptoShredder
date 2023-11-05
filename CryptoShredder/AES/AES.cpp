#include "AES.h"


//set default values of Nk and Nr to AES-128
size_t AES::Nk = 4; //number of 32-bit words in the key (AES-128)
size_t AES::Nr = 10; //number of rounds (AES-128 has 10 rounds)


/// <summary>
/// Represents the SBOX table of AES encryption.
/// </summary>
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


/// <summary>
/// Represents the inverse SBOX table of AES encryption.
/// </summary>
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


/// <summary>
///  Galois multiplication tables for AES MixColumns function.
/// </summary>
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


/// <summary>
/// Function that handles the operation mode of AES encryption, this function throws runtime error if given key size isn't valid.
/// </summary>
/// <param name="size_t keySize"></param>
void AES::SetOperationMode(const size_t keySize) {
    if (keySize == 16) { //if keySize is 16 bytes (128 bits)
        AES::Nk = 4; //number of 32-bit words in the key (AES-128)
        AES::Nr = 10; //number of rounds (AES-128 has 10 rounds)
    }
    else if (keySize == 24) { //if keySize is 24 bytes (192 bits)
        AES::Nk = 6; //number of 32-bit words in the key (AES-192)
        AES::Nr = 12; //number of rounds (AES-192 has 12 rounds)
    }
    else if (keySize == 32) { //if keySize is 32 bytes (256 bits)
        AES::Nk = 8; //number of 32-bit words in the key (AES-256)
        AES::Nr = 14; //number of rounds (AES-256 has 14 rounds)
    }
    else //else keySize isn't valid
        throw runtime_error("Invalid mode of operation, please provide valid key that matches AES requirements."); //we throw a runtime error
}


/// <summary>
/// Function for printing a vector array.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; vector"></param>
void AES::PrintVector(const vector<unsigned char>& vector) {
    for (unsigned char element : vector) // iterate through each element
        cout << hex << setw(2) << setfill('0') << (int)element << " "; //set the output stream to hexadecimal mode
    cout << endl; //new line
}


/// <summary>
/// Function for printing a matrix represented as vector of vectors.
/// </summary>
/// <param name="vector&lt;vector&lt;unsigned char&gt;&gt; matrix"></param>
void AES::PrintMatrix(const vector<vector<unsigned char>>& matrix) {
    for (const vector<unsigned char>& row : matrix) {
        for (unsigned char element : row) //iterate through the inner vector (row)
            cout << hex << setw(2) << setfill('0') << (int)element << " "; //set the output stream to hexadecimal mode
        cout << dec << endl; //restore the output stream to decimal mode after each row
    }
    cout << endl; //new line
}


/// <summary>
/// Function that rotates a vector element(byte) to the left.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; word"></param>
/// <returns>vector&lt;unsigned char&gt; rotatedWord</returns>
const vector<unsigned char> AES::RotWord(const vector<unsigned char>& word) {
    vector<unsigned char> result(Nb); //initialize a vector of size Nb
    //shift the word to the left by one position
    result[0] = word[1];
    result[1] = word[2];
    result[2] = word[3];
    result[3] = word[0];
    return result; //return result
}


/// <summary>
/// Function that substitutes each byte in a word using the SBOX.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; word"></param>
/// <returns>vector&lt;unsigned char&gt; subWord</returns>
const vector<unsigned char> AES::SubWord(const vector<unsigned char>& word) {
    vector<unsigned char> result(Nb); //initialize a vector of size Nb
    for (int i = 0; i < Nb; i++) { //iterate over the word
        //we set the value from SBOX with right rotating by 4 to extract the left value and OR with 0x0F for extracting the right value
        result[i] = SBOX[word[i] >> 4][word[i] & 0x0F]; //set the value from the SBOX
    }
    return result; //return new word with SBOX values
}


/// <summary>
/// Function that returns the Rcon value given original value for key schedule.
/// </summary>
/// <param name=" unsigned char value"></param>
/// <returns>unsigned char RconValue</returns>
const unsigned char AES::Rcon(const unsigned char& value) {
    unsigned char result = 0x01; //initialize with 0x01 (first round constant)
    for (int i = 1; i < value; i++) {
        if (result & 0x80) //if the leftmost bit (0x80) is set
            result = (result << 1) ^ 0x11B; //XOR with 0x11B after left shifting
        else //else we left shift 
            result = result << 1; //left shift the result by one
    }
    return result; //return rcon value
}


/// <summary>
/// Function for substitute bytes in AES encryption, both for encryption and decryption.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; state"></param>
/// <param name="bool inverse"></param>
/// <returns>vector&lt;unsigned char&gt; subState</returns>
const vector<unsigned char> AES::SubBytes(vector<unsigned char>& state, const bool inverse) {
    if (!inverse) { //perform substitute bytes for encryption
        for (size_t i = 0; i < state.size(); i++) //iterate over state vector
            state[i] = SBOX[state[i] >> 4][state[i] & 0x0F]; //set correct value from SBOX
    }
    else { //perform substitute bytes for decryption
        for (size_t i = 0; i < state.size(); i++) //iterate over state vector
            state[i] = INVSBOX[state[i] >> 4][state[i] & 0x0F]; //set correct value from INVSBOX
    }
    return state; //return new state vector after substitute bytes
}


/// <summary>
/// Function for shifting rows in AES encryption, both for encryption and decryption.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; state"></param>
/// <param name="bool inverse"></param>
/// <returns>vector&lt;unsigned char&gt; shiftedState</returns>
const vector<unsigned char> AES::ShiftRows(vector<unsigned char>& state, const bool inverse) {
    if (!inverse) { //perform shift rows for encryption
        //swap elements in second row
        for (int i = 1; i <= 9; i += Nb) //iterate on second row
            swap(state[i], state[i + Nb]); //swap elements
        //swap elements in third row
        for (int i = 2; i <= 6; i += Nb) //iterate on third row
            swap(state[i], state[i + (Nb * 2)]); //swap elements
        //swap elements in fourth row
        for (int i = 15; i >= 7; i -= Nb) //iterate on fourth row
            swap(state[i], state[i - Nb]); //swap elements
    }
    else { //perform inverse shift rows for decryption
        //swap elements in second row
        for (int i = 13; i >= 5; i -= Nb) //iterate on second row
            swap(state[i], state[i - Nb]); // swap elements
        //swap elements in third row
        for (int i = 2; i <= 6; i += Nb) //iterate on third row
            swap(state[i], state[i + (Nb * 2)]); //swap elements
        //swap elements in fourth row
        for (int i = 3; i <= 11; i += Nb) //iterate on fourth row
            swap(state[i], state[i + Nb]); //swap elements
    }
    return state; //return shifted state vector
}


/// <summary>
/// Function for mixing columns using GaloisMult tables for AES encryption, both for encryption and decryption.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; state"></param>
/// <param name="bool inverse"></param>
/// <returns>vector&lt;unsigned char&gt; mixedState</returns>
const vector<unsigned char> AES::MixColumns(vector<unsigned char>& state, const bool inverse) {
    vector<unsigned char> temp(Nb); //temp vector represents current column values 
    if (!inverse) { //perform mix columns for encryption
        for (size_t i = 0; i < BlockSize; i += Nb) { //iterate over state vector
            copy(state.begin() + i, state.begin() + i + Nb, temp.begin()); //copy the columns values to temp vector

            //apply Galois field equations for each byte in state vector
            state[i + 0] = (unsigned char)(GaloisMult[2][temp[0]] ^ GaloisMult[3][temp[1]] ^ temp[2] ^ temp[3]);
            state[i + 1] = (unsigned char)(temp[0] ^ GaloisMult[2][temp[1]] ^ GaloisMult[3][temp[2]] ^ temp[3]);
            state[i + 2] = (unsigned char)(temp[0] ^ temp[1] ^ GaloisMult[2][temp[2]] ^ GaloisMult[3][temp[3]]);
            state[i + 3] = (unsigned char)(GaloisMult[3][temp[0]] ^ temp[1] ^ temp[2] ^ GaloisMult[2][temp[3]]);
        }
    }
    else { //perform mix columns for decryption
        for (size_t i = 0; i < BlockSize; i += Nb) { //iterate over state vector
            copy(state.begin() + i, state.begin() + i + Nb, temp.begin()); //copy the columns values to temp vector

            //apply Galois field equations for each byte in state vector
            state[i + 0] = (unsigned char)(GaloisMult[14][temp[0]] ^ GaloisMult[11][temp[1]] ^ GaloisMult[13][temp[2]] ^ GaloisMult[9][temp[3]]);
            state[i + 1] = (unsigned char)(GaloisMult[9][temp[0]] ^ GaloisMult[14][temp[1]] ^ GaloisMult[11][temp[2]] ^ GaloisMult[13][temp[3]]);
            state[i + 2] = (unsigned char)(GaloisMult[13][temp[0]] ^ GaloisMult[9][temp[1]] ^ GaloisMult[14][temp[2]] ^ GaloisMult[11][temp[3]]);
            state[i + 3] = (unsigned char)(GaloisMult[11][temp[0]] ^ GaloisMult[13][temp[1]] ^ GaloisMult[9][temp[2]] ^ GaloisMult[14][temp[3]]);
        }
    }
    return state; //return state after mix columns
}


/// <summary>
/// Function for XOR operation between two vectors in same size, vectors immutable if flag is set. 
/// </summary>
/// <param name="vector&lt;unsigned char&gt; first"></param>
/// <param name="vector&lt;unsigned char&gt; second"></param>
/// <param name="bool immutable"></param>
/// <returns>vector&lt;unsigned char&gt; xor</returns>
const vector<unsigned char> AES::XOR(vector<unsigned char>& first, const vector<unsigned char>& second, const bool immutable) {
    if (first.size() != second.size()) //if not same size we exit the function
        return first; //return first vector as indication for failure

    if (immutable) { //perform XOR operation with new vector, keep given vectors unchanged
        vector<unsigned char> result; //create new vector for XOR result
        result.reserve(first.size()); //reserve memory for vector
        for (size_t i = 0; i < first.size(); i++) { //iterate over the vectors
            result.push_back(first[i] ^ second[i]); //push to new vector the new XOR'ed elements 
        }
        return result; //return XOR result
    }
    else { //perform XOR operation in place with first vector
        for (size_t i = 0; i < first.size(); i++) //iterate over the vectors
            first[i] ^= second[i]; //perform XOR on vectors elements
        return first; //return first vector with XOR result
    }
}


/// <summary>
/// Function that splits given vector into AES blocks (4 bytes).
/// </summary>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;vector&lt;unsigned char&gt;&gt; splitKey</returns>
const vector<vector<unsigned char>> AES::SplitIntoKeyWords(const vector<unsigned char>& key) {
    vector<vector<unsigned char>> KeyWordArray; //initialize new vector
    KeyWordArray.reserve(key.size() / Nb); //reserve memory for blocks 
    for (size_t i = 0; i < key.size(); i += Nb) { //iterate over the given key vector
        KeyWordArray.emplace_back(key.begin() + i, key.begin() + i + Nb); //split the vector into blocks and insert into KeyWordArray
    }
    return KeyWordArray; //return the new vector
}


/// <summary>
/// Function for generating round keys for AES encryption, supports AES-128, AES-192 and AES-256.
/// </summary>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;vector&lt;unsigned char&gt;&gt; roundKeys</returns>
const vector<vector<unsigned char>> AES::KeySchedule(const vector<unsigned char>& key) {
    vector<vector<unsigned char>> roundKeysMatrix; //represents the matrix of round keys (each represented as a vector of unsigned char)
    vector<unsigned char> roundKeysVector; //represents new round keys vector
    vector<unsigned char> previousKey(Nb * Nk); //represents previous round key
    vector<vector<unsigned char>> currentWord(Nk, vector<unsigned char>(Nb)); //represents current key in progress
    vector<unsigned char> temp(Nb); //represents temporary vector for key schedule operations
    roundKeysMatrix.reserve(Nr + 1); //reserve memory for our keys in advance for better performance
    copy(key.begin(), key.end(), back_inserter(roundKeysVector)); //add initial key to roundKeyVector

    //iterate over the round keys vector in the specified number of rounds and generate round keys
    for (int i = 1; i <= Nr; i++) {
        copy(roundKeysVector.end() - (Nb * Nk), roundKeysVector.end(), previousKey.begin()); //retrieve previous key from roundKeysVector
        currentWord = SplitIntoKeyWords(previousKey); //split key into 32-bit keywords for ease of use
        temp = SubWord(RotWord(currentWord[Nk - 1])); //apply SubWord and RotWord operation on current word and save it in temp vector
        temp[0] ^= Rcon(i); //XOR temp vector with Rcon value 
        //now we need to iterate over Nk (number of keywords) to generate the key
        for (int j = 0; j < Nk; j++) {
            temp = XOR(temp, currentWord[j]); //call our XOR function to apply XOR operation on temp and currentWord vectors
            copy(temp.begin(), temp.end(), back_inserter(roundKeysVector)); //add our word into the round keys vector
            if (Nk > 6 && j == 3) //for AES-256 we need to apply SubWord again for added security half way of the generation
                temp = SubWord(temp); //apply the SubWord function again for AES-256
        }
    }
    roundKeysVector.erase(roundKeysVector.begin() + (Nr + 1) * BlockSize, roundKeysVector.end()); //remove the extra bytes if present (on AES-192 and AES-256)

    //finally we add roundKeysVector keys to roundKeysMatrix for later use in AES encryption
    for (size_t i = 0; i < roundKeysVector.size(); i += BlockSize) //iterate over roundKeysVector
        roundKeysMatrix.emplace_back(roundKeysVector.begin() + i, roundKeysVector.begin() + min(roundKeysVector.size(), i + BlockSize)); //getting each key (128-bit) from beginning of vector and adding it to roundKeysMatrix

    return roundKeysMatrix; //return our roundKeysMatrix for AES operation
}


/// <summary>
/// Function that performs AES encryption on given text using specified round keys, supports AES-128, AES-192 and AES-256. 
/// <para>This function performs AES encryption with fixed block size of 16 bytes (128-bit).</para>
/// <para>This function throws runtime error if given text is invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;vector&lt;unsigned char&gt;&gt; roundKeys"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::EncryptBlock(vector<unsigned char>& text, const vector<vector<unsigned char>>& roundKeys) {
    if(text.size() != BlockSize) //if plaintext isn't valid we throw runtime error
        throw runtime_error("Invalid mode of operation, please provide valid plaintext that matches AES requirements."); //throw a runtime error
    //apply initial round key
    XOR(text, roundKeys[0]); //perform first AddRoundKey operation on text
    //apply AES operations of SubByte, ShiftRows, MixColumns and AddRoundKey
    for (int i = 1; i < Nr; i++) { //iterate over roundKeys and apply AES operations
        SubBytes(text, false); //perform SubBytes operation on text
        ShiftRows(text, false); //perform ShiftRows operation on text
        MixColumns(text, false); //perform MixColumns operation on text
        XOR(text, roundKeys[i]); //perform AddRoundKey operation on text
    }
    //apply AES final round operations SubBytes, ShiftRows and AddRoundKey
    SubBytes(text, false); //perform SubBytes operation on text
    ShiftRows(text, false); //perform ShiftRows operation on text
    XOR(text, roundKeys[Nr]); //perform AddRoundKey operation on text 
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption on given text using specified round keys, supports AES-128, AES-192 and AES-256. 
/// <para>This function performs AES decryption with fixed block size of 16 bytes (128-bit).</para>
/// <para>This function throws runtime error if given text is invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;vector&lt;unsigned char&gt;&gt; roundKeys"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::DecryptBlock(vector<unsigned char>& text, const vector<vector<unsigned char>>& roundKeys) {
    if (text.size() != BlockSize) //if plaintext isn't valid we throw runtime error
        throw runtime_error("Invalid mode of operation, please provide valid plaintext that matches AES requirements."); //throw a runtime error
    //apply AES final round operations in reverse order of AddRoundKey, ShiftRows and SubBytes
    XOR(text, roundKeys[Nr]); //perform AddRoundKey operation on text
    ShiftRows(text, true); //perform ShiftRows operation on text
    SubBytes(text, true); //perform SubBytes operation on text
    //apply AES operations in reverse order of AddRoundKey, MixColumns, ShiftRows and SubBytes
    for (int i = Nr - 1; i >= 1; i--) { //iterate over roundKeys in reverse order and apply AES operations
        XOR(text, roundKeys[i]); //perform AddRoundKey operation on text
        MixColumns(text, true); //perform MixColumns operation on text
        ShiftRows(text, true); //perform ShiftRows operation on text
        SubBytes(text, true); //perform SubBytes operation on text
    }
    //apply initial round key
    XOR(text, roundKeys[0]); //perform AddRoundKey operation on text
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption on given text using specified key, supports AES-128, AES-192 and AES-256. 
/// <para>This function performs AES encryption with fixed block size of 16 bytes (128-bit).</para>
/// <para>This function throws runtime error if given text and key are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    EncryptBlock(text, roundKeys); //call our AES EncryptBlock function for encrypting text using round keys
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption on given text using specified key, supports AES-128, AES-192 and AES-256. 
/// <para>This function performs AES decryption with fixed block size of 16 bytes (128-bit).</para>
/// <para>This function throws runtime error if given text and key are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    DecryptBlock(text, roundKeys); //call our AES DecryptBlock function for decrypting text using round keys
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption in ECB mode on given text using specified key. 
/// <para>ECB mode supports AES-128, AES-192 and AES-256, includes PKCS7 padding.</para> 
/// <para>This function throws runtime error if given text and key are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp(BlockSize); //represents temp vector for ECB operation
    if (text.size() % BlockSize != 0) { //if text size isn't multiply of 16 bytes we add padding
        size_t padding = BlockSize - (text.size() % BlockSize); //calculate the number of padding bytes needed
        text.insert(text.end(), padding, (unsigned char)padding); //append the padding bytes to the text
    }
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        copy(text.begin() + i, text.begin() + i + BlockSize, temp.begin()); //extract block from the input
        EncryptBlock(temp, roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
        copy(temp.begin(), temp.end(), text.begin() + i); //replace the original block in the input text with the encrypted block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption in ECB mode on given text using specified key.
/// <para>ECB mode supports AES-128, AES-192 and AES-256, includes PKCS7 padding.</para> 
/// <para>This function throws runtime error if given text and key are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt_ECB(vector<unsigned char>& text, const vector<unsigned char>& key) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp(BlockSize); //represents temp vector for ECB operation
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        copy(text.begin() + i, text.begin() + i + BlockSize, temp.begin()); //extract block from the input
        DecryptBlock(temp, roundKeys); //decrypt the block using our AES DecryptBlock function using round keys
        copy(temp.begin(), temp.end(), text.begin() + i); //replace the original block in the input text with the decrypted block
    }
    size_t padding = text.back(); //get the value of the last byte, which indicates the padding size
    if (padding > 0 && padding <= BlockSize) //if true we have padding bytes to remove for text
        text.resize(text.size() - padding); //remove the padding bytes from the text
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption in CBC mode on given text using specified key and initialization vector. 
/// <para>CBC mode supports AES-128, AES-192 and AES-256, includes PKCS7 padding.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CBC requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp(BlockSize); //represents temp vector for CBC operation
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    if (text.size() % BlockSize != 0) { //if text size isn't multiply of 16 bytes we add padding
        size_t padding = BlockSize - (text.size() % BlockSize); //calculate the number of padding bytes needed
        text.insert(text.end(), padding, (unsigned char)padding); //append the padding bytes to the text
    }
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        copy(text.begin() + i, text.begin() + i + BlockSize, temp.begin()); //extract block from the input
        XOR(temp, previousCipher); //XOR with previous cipher block
        EncryptBlock(temp, roundKeys); //encrypt the block using our AES EncryptBlock function using round keys
        copy(temp.begin(), temp.end(), text.begin() + i); //replace the original block in the input text with the encrypted block
        previousCipher = temp; //update previous cipher block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption in CBC mode on given text using specified key and initialization vector. 
/// <para>CBC mode supports AES-128, AES-192 and AES-256, includes PKCS7 padding.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt_CBC(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CBC requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp(BlockSize); //represents temp vector for CBC operation
    vector<unsigned char> cipherBlock(BlockSize); //represents current cipher block for decryption process
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    for (size_t i = 0; i < text.size(); i += BlockSize) { //iterate over text
        copy(text.begin() + i, text.begin() + i + BlockSize, temp.begin()); //extract block from the input
        cipherBlock = temp; //save the current cipher block
        DecryptBlock(temp, roundKeys); //decrypt the block using our AES DecryptBlock function using round keys
        XOR(temp, previousCipher); //XOR with previous cipher block
        copy(temp.begin(), temp.end(), text.begin() + i); //replace the original block in the input text with the decrypted block
        previousCipher = cipherBlock; //update previous cipher block with current cipher block
    }
    size_t padding = text.back(); //get the value of the last byte, which indicates the padding size
    if (padding > 0 && padding <= BlockSize) //if true we have padding bytes to remove for text
        text.resize(text.size() - padding); //remove the padding bytes from the text
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations 
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption in CFB mode on given text using specified key and initialization vector. 
/// <para>CFB mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CFB requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp = iv; //initialize temp vector for cipher block with IV vector
    vector<unsigned char> previousCipher(BlockSize); //initialize previousCipher vector
    size_t j = 0; //represents the size of previousCipher block, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            previousCipher = temp; //set temp vector to previousCipher block for encryption
            EncryptBlock(previousCipher, roundKeys); //encrypt the previous cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for previousCipher back to zero to perform XOR operation 
        }
        text[i] ^= previousCipher[j]; //perform byte XOR between text and previousCipher block
        temp[j] = text[i]; //update temp with the new ciphered text
        j++; //increase j index for previousCipher block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption in CFB mode on given text using specified key and initialization vector. 
/// <para>CFB mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt_CFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CFB requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> temp = iv; //initialize temp vector for cipher block with IV vector
    vector<unsigned char> previousCipher(BlockSize); //initialize previousCipher vector 
    size_t j = 0; //represents the size of previousCipher block, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            previousCipher = temp; //set temp vector to previousCipher block for decryption
            EncryptBlock(previousCipher, roundKeys); //decrypt the previous cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for previousCipher back to zero to perform XOR operation 
        }
        temp[j] = text[i]; //update temp with the new deciphered text
        text[i] ^= previousCipher[j]; //perform byte XOR between text and previousCipher block
        j++; //increase j index for previousCipher block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption in OFB mode on given text using specified key and initialization vector. 
/// <para>OFB mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES OFB requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    size_t j = 0; //represents the size of previousCipher block, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            EncryptBlock(previousCipher, roundKeys); //encrypt the previous cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for previousCipher back to zero to perform XOR operation 
        }
        text[i] ^= previousCipher[j]; //perform byte XOR between text and previousCipher block
        j++; //increase j index for previousCipher block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption in OFB mode on given text using specified key and initialization vector. 
/// <para>OFB mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt_OFB(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES OFB requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> previousCipher = iv; //initialize previousCipher vector with IV vector
    size_t j = 0; //represents the size of previousCipher block, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            EncryptBlock(previousCipher, roundKeys); //decrypt the previous cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for previousCipher back to zero to perform XOR operation 
        }
        text[i] ^= previousCipher[j]; //perform byte XOR between text and previousCipher block
        j++; //increase j index for previousCipher block
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return deciphered text
}


/// <summary>
/// Function that performs AES encryption in CTR mode on given text using specified key and initialization vector. 
/// <para>CTR mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; cipherText</returns>
const vector<unsigned char> AES::Encrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CTR requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> tempIV = iv; //initialize tempIV vector for cipher block with IV vector
    vector<unsigned char> currentIV(BlockSize); //initialize currentIV vector
    size_t j = 0; //represents the size of IV vector, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            currentIV = tempIV; //set tempIV vector to currentIV vector for encryption
            EncryptBlock(currentIV, roundKeys); //encrypt the cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for IV vector back to zero to perform XOR operation 
        }
        text[i] ^= currentIV[j]; //perform byte XOR between text and IV vector
        j++; //increase j index for IV vector
        //here we increase the counter for IV vector
        if (j == BlockSize) { //if j equals to block size (16 bytes)
            for (int k = BlockSize - 1; k >= 0; k--) //iterate over tempIV from end to start
                if (tempIV[k]++) break; //increment temp[k] and break if it's not zero
        }
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return ciphered text
}


/// <summary>
/// Function that performs AES decryption in CTR mode on given text using specified key and initialization vector. 
/// <para>CTR mode supports AES-128, AES-192 and AES-256, supports text in any size.</para> 
/// <para>This function throws runtime error if given text, key and iv are invalid.</para>
/// </summary>
/// <param name="vector&lt;unsigned char&gt; text"></param>
/// <param name="vector&lt;unsigned char&gt; key"></param>
/// <param name="vector&lt;unsigned char&gt; iv"></param>
/// <returns>vector&lt;unsigned char&gt; decipherText</returns>
const vector<unsigned char> AES::Decrypt_CTR(vector<unsigned char>& text, const vector<unsigned char>& key, const vector<unsigned char>& iv) {
    SetOperationMode(key.size()); //call our SetOperationMode function to check the key and set correct AES mode, throws runtime error if key invalid
    if (iv.size() != BlockSize) //if IV vector isn't in correct size
        throw runtime_error("Invalid mode of operation, please provide valid initialization vector that matches AES CTR requirements."); //throw runtime error
    vector<vector<unsigned char>> roundKeys = KeySchedule(key); //call our KeySchedule function for generating round keys
    vector<unsigned char> tempIV = iv; //initialize tempIV vector for cipher block with IV vector
    vector<unsigned char> currentIV(BlockSize); //initialize currentIV vector
    size_t j = 0; //represents the size of IV vector, we use it as an index for performing XOR with each cipher block
    for (size_t i = 0; i < text.size(); i++) { //iterate over text
        if (i == 0 || j == BlockSize) { //if we are in first iteration or when j equals to block size (16 bytes)
            currentIV = tempIV; //set tempIV vector to currentIV vector for decryption
            EncryptBlock(currentIV, roundKeys); //decrypt the cipher block using our AES EncryptBlock function using round keys
            j = 0; //set the index for IV vector back to zero to perform XOR operation 
        }
        text[i] ^= currentIV[j]; //perform byte XOR between text and IV vector
        j++; //increase j index for IV vector
        //here we increase the counter for IV vector
        if (j == BlockSize) { //if j equals to block size (16 bytes)
            for (int k = BlockSize - 1; k >= 0; k--) //iterate over tempIV from end to start
                if (tempIV[k]++) break; //increment temp[k] and break if it's not zero
        }
    }
    roundKeys.clear(); //clear our roundKeys matrix for added security after we finish operations
    return text; //return deciphered text
}



//int main() {
//    ///test key schedule///
//    //vector<unsigned char> key1(16, 0x00);
//    ////AES::SetOperationMode(16, key.size());
//    //vector<vector<unsigned char>> keys = AES::KeySchedule(key1);
//    //AES::PrintMatrix(keys);
//
//    ///test AES encryption and decryption///
//    string plaintext = "TheKingOfNewYork";
//    string key = "PopSmokeTheWoo55";
//    string iv = "PopSmokeTheWoo55";
//    vector<unsigned char> plaintextVec(plaintext.begin(), plaintext.end());
//    vector<unsigned char> keyVec(key.begin(), key.end());
//    vector<unsigned char> ivVec(iv.begin(), iv.end());
//    cout << "Plaintext:" << endl;
//    AES::PrintVector(plaintextVec);
//    try {
//        //plaintextVec = AES::Encrypt_ECB(plaintextVec, keyVec);
//        //plaintextVec = AES::Encrypt_CBC(plaintextVec, keyVec, ivVec);
//        //plaintextVec = AES::Encrypt_CFB(plaintextVec, keyVec, ivVec);
//        //plaintextVec = AES::Encrypt_OFB(plaintextVec, keyVec, ivVec);
//        plaintextVec = AES::Encrypt_CTR(plaintextVec, keyVec, ivVec);
//        cout << "Cipher:" << endl;
//        AES::PrintVector(plaintextVec);
//        cout << "Original Text:" << endl;
//        //plaintextVec = AES::Decrypt_ECB(plaintextVec, keyVec);
//        //plaintextVec = AES::Decrypt_CBC(plaintextVec, keyVec, ivVec);
//        //plaintextVec = AES::Decrypt_CFB(plaintextVec, keyVec, ivVec);
//        //plaintextVec = AES::Decrypt_OFB(plaintextVec, keyVec, ivVec);
//        plaintextVec = AES::Decrypt_CTR(plaintextVec, keyVec, ivVec);
//        AES::PrintVector(plaintextVec);
//        string str(plaintextVec.begin(), plaintextVec.end());
//        cout << str << endl;
//    }
//    catch (const runtime_error& e) {
//        cerr << e.what() << endl;
//        return 1;
//    }
//
//    return 0;
//}