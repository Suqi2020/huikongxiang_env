#include "board.h"


static union {
  float flo;
  char  buf[4];
} HexFloat;

uint32_t write_float_to_hex(float number)
{
    HexFloat.flo = number;
    uint16_t dat0 = (HexFloat.buf[1] << 8) + HexFloat.buf[0];
    uint16_t dat1 = (HexFloat.buf[3] << 8) + HexFloat.buf[2];
    uint32_t ndata = dat0 << 16 | dat1;
    printf("dat0 : %02X ; dat1 : %02X ; ndata : %08X\n", dat0, dat1, ndata);
    return ndata;
}

float write_hex_to_float(uint32_t number)
{
    uint16_t dat0 = number & 0x0000FFFF;
    uint16_t dat1 = (number >> 16) & 0x0000FFFF;
    HexFloat.buf[0] = dat0 & 0xFF;
    HexFloat.buf[1] = (dat0 >> 8) & 0xFF;
    HexFloat.buf[2] = dat1 & 0xFF;
    HexFloat.buf[3] = (dat1 >> 8) & 0xFF;
    float fdata = HexFloat.flo;
   // printf("ndata:%08X; dat0:%02X; dat1:%02X; fdata:%f\n",\
           number, dat0, dat1, fdata);
    return fdata;
}