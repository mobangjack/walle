/**
 * Copyright (c) 2011-2016, Mobangjack 莫帮杰 (mobangjack@foxmail.com).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
/***************************************/
/************ Control Bus **************/
/***************************************/

#ifndef __CBUS_H__ 
#define __CBUS_H__

#ifdef __cplusplus 
extern "C" {         
#endif

#include "stdint.h"

#define MSG_HEADER  0xff
#define MSG_ID_PAN  0xfe
#define MSG_ID_TILT 0xfd
#define MSG_ID_ZOOM 0xfc
#define MSG_ID_FIRE 0xfb

#define CBUS_BOUND  (uint8_t)0xff // CBUS bound
#define CBUS_CH_MIN (uint8_t)0x00 // CBUS channel min value (0)
#define CBUS_CH_MAX (uint8_t)0xfe // CBUS channel max value (254)
#define CBUS_CH_OFS (uint8_t)0x7f // CBUS channel min value (127)
#define CBUS_BUF_LEN    10    // CBUS buffer length

typedef struct CBus
{
	uint8_t x; // horizontal (pan)
	uint8_t y; // vertical (tilt)
	uint8_t z; // depth (zoom)
	uint8_t f; // functional bits (fire)
}CBus;

uint8_t CBus_PackUp(const CBus* cbus, uint8_t* cbuf);
uint8_t CBus_Unpack(CBus* cbus, const uint8_t* cbuf);

#ifdef __cplusplus
}
#endif

#endif

