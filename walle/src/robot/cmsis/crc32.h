#ifndef __CRC32_H__
#define __CRC32_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void gen_crc_table(void);
uint32_t update_crc(uint32_t crc_accum, byte *data_blk_ptr, uint32_t data_blk_size);

#ifdef __cplusplus
}

#endif

