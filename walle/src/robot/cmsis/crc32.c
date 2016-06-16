#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc32.h"

#define POLYNOMIAL 0x04c11db7L      // Standard CRC-32 ppolynomial
#define BUFFER_LEN       4096L      // Length of buffer

static uint32_t crc_table[256];       // Table of 8-bit remainders

crc32 g_crc32;

void gen_crc_table(void)
{
	register uint16_t i, j;
	register uint32_t crc_accum;

	for (i=0;  i<256;  i++)
	{
		crc_accum = ( (uint32_t) i << 24 );
		for ( j = 0;  j < 8;  j++ )
		{
			if ( crc_accum & 0x80000000L )
				crc_accum = (crc_accum << 1) ^ POLYNOMIAL;
			else
				crc_accum = (crc_accum << 1);
		}
		crc_table[i] = crc_accum;
	}
}

uint32_t update_crc(uint32_t crc_accum, byte *data_blk_ptr, uint32_t data_blk_size)
{
	if ( data_blk_size < sizeof(uint32_t) )
	{
		return 0;
	}

	register uint32_t i, j;

	for (j=0; j<data_blk_size; j++)
	{
		i = ((int) (crc_accum >> 24) ^ *data_blk_ptr++) & 0xFF;
		crc_accum = (crc_accum << 8) ^ crc_table[i];
	}
	crc_accum = ~crc_accum;
	memcpy( data_blk_ptr + data_blk_size - sizeof(uint32_t), &crc_accum, sizeof(uint32_t) );

	return crc_accum;
}
