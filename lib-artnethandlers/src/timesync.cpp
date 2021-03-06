/**
 * @file timesync.cpp
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
#include <time.h>

#include "timesync.h"

#include "artnettimesync.h"

#include "hardware.h"

#include "debug.h"

TimeSync::TimeSync(void){
	DEBUG_ENTRY

	DEBUG_EXIT
}

TimeSync::~TimeSync(void) {
	DEBUG_ENTRY

	DEBUG_EXIT
}

void TimeSync::Handler(const struct TArtNetTimeSync *pArtNetTimeSync) {
	DEBUG_ENTRY

	struct THardwareTime hwTime;

	hwTime.tm_sec = pArtNetTimeSync->tm_sec;
	hwTime.tm_min = pArtNetTimeSync->tm_min;
	hwTime.tm_hour = pArtNetTimeSync->tm_hour;
	hwTime.tm_mday = pArtNetTimeSync->tm_mday;
	hwTime.tm_mon = pArtNetTimeSync->tm_mon;
	hwTime.tm_year = ((uint16_t) (pArtNetTimeSync->tm_year_hi) << 8) + pArtNetTimeSync->tm_year_lo;

	Hardware::Get()->SetTime(hwTime);

	DEBUG_PRINTF("%.4d/%.2d/%.2d %.2d:%.2d:%.2d", 1900 + hwTime.tm_year, 1 + hwTime.tm_mon, hwTime.tm_mday, hwTime.tm_hour, hwTime.tm_min, hwTime.tm_sec);
	DEBUG_EXIT
}
