/**
 * @file ws28xxstatic.cpp
 *
 */
/* Copyright (C) 2019 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "ws28xx.h"
#include "ws28xxconst.h"

const char* WS28xx::GetLedTypeString(TWS28XXType tType) {
	assert(tType < WS28XX_UNDEFINED);

	return WS28xxConst::TYPES[tType];
}

TWS28XXType WS28xx::GetLedTypeString(const char *pVale) {
	assert(pVale != 0);

	for (uint32_t i = 0; i < WS28XX_UNDEFINED; i++) {
		if (strcasecmp(pVale, WS28xxConst::TYPES[i]) == 0) {
			return (TWS28XXType) i;
		}
	}

	return WS28XX_UNDEFINED;
}
