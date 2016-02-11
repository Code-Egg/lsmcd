/*
 * Copyright 2002 Lite Speed Technologies Inc, All Rights Reserved.
 * LITE SPEED PROPRIETARY/CONFIDENTIAL.
 */


#include <util/base64.h>

static const unsigned char s_decodeTable[128] =
{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 00-0F */
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, /* 10-1F */
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63, /* 20-2F */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255, /* 30-3F */
    255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,          /* 40-4F */
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255, /* 50-5F */
    255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, /* 60-6F */
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255 /* 70-7F */
};

static const unsigned char s_encodeTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

Base64::Base64()
{
}
Base64::~Base64()
{
}

const unsigned char *Base64::getDecodeTable()
{   return s_decodeTable;       }

int Base64::decode(const char *encoded, int size, char *decoded)
{
    register const char *pEncoded = encoded;
    register unsigned char e1, prev_e = 0;
    register char           phase = 0;
    register unsigned char *pDecoded = (unsigned char *)decoded;
    register const char *pEnd = encoded + size ;

    while (pEncoded < pEnd)
    {
        register int ch = *pEncoded++;
        if (ch < 0)
            continue;
        e1 = s_decodeTable[ ch ];
        if (e1 != 255)
        {
            switch (phase)
            {
            case 0:
                break;
            case 1:
                *pDecoded++ = ((prev_e << 2) | ((e1 & 0x30) >> 4));
                break;
            case 2:
                *pDecoded++ = (((prev_e & 0xf) << 4) | ((e1 & 0x3c) >> 2));
                break;
            case 3:
                *pDecoded++ = (((prev_e & 0x03) << 6) | e1);
                phase = -1;
                break;
            }
            phase++;
            prev_e = e1;
        }
    }
    *pDecoded = 0;
    return pDecoded - (unsigned char *)decoded;
}

int Base64::encode(const char *decoded, int size, char *encoded)
{
    register const unsigned char *pDecoded = (const unsigned char *)decoded;
    register const unsigned char *pEnd = (const unsigned char *)decoded + size
                                         ;
    register char *pEncoded = encoded;
    register unsigned char ch;

    size %= 3;
    pEnd -= size;

    while (pEnd > pDecoded)
    {
        *pEncoded++ = s_encodeTable[((ch = *pDecoded++) >> 2) & 0x3f];
        *pEncoded++ = s_encodeTable[((ch & 0x03) << 4) | (*pDecoded >> 4)];
        ch = *pDecoded++;
        *pEncoded++ = s_encodeTable[((ch & 0x0f) << 2) | (*pDecoded >> 6)];
        *pEncoded++ = s_encodeTable[ *pDecoded++ & 0x3f];
    }

    if (size > 0)
    {
        *pEncoded++ = s_encodeTable[((ch = *pDecoded++) >> 2) & 0x3f];

        if (size == 1)
        {
            *pEncoded++ = s_encodeTable[(ch & 0x03) << 4];
            *pEncoded++ = '=';

        }
        else
        {
            *pEncoded++ = s_encodeTable[((ch & 0x03) << 4) | (*pDecoded >> 4)];
            *pEncoded++ = s_encodeTable[(*pDecoded & 0x0f) << 2];
        }
        *pEncoded++ = '=';
    }

    return pEncoded - encoded;
}

