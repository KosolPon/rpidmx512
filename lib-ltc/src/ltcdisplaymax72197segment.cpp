/**
 * @file ltcdisplaymax72197segment.cpp
 *
 */
/* Copyright (C) 2019-2020 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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
#include <assert.h>

#include "ltcdisplaymax72197segment.h"

#include "d8x7segment.h"

#include "max7219.h"
#include "max7219_spi.h"

LtcDisplayMax72197Segment *LtcDisplayMax72197Segment::s_pThis = 0;

LtcDisplayMax72197Segment::LtcDisplayMax72197Segment(void) {
	s_pThis = this;

	m_DeviceInfo.chip_select = SPI_CS0;
	m_DeviceInfo.speed_hz = 0;
}

LtcDisplayMax72197Segment::~LtcDisplayMax72197Segment(void) {
}

void LtcDisplayMax72197Segment::Init(uint8_t nIntensity) {
	d8x7segment_init(&m_DeviceInfo, nIntensity);
	d8x7segment_cls(&m_DeviceInfo);

	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT6, 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT4, 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT2, 0x80);
}

void LtcDisplayMax72197Segment::Show(const char *pTimecode) {
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT7, (pTimecode[0] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT6, (pTimecode[1] - '0') | 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT5, (pTimecode[3] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT4, (pTimecode[4] - '0') | 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT3, (pTimecode[6] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT2, (pTimecode[7] - '0') | 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT1, (pTimecode[9] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT0, (pTimecode[10] - '0'));
}

void LtcDisplayMax72197Segment::ShowSysTime(const char *pSystemTime) {
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT7, MAX7219_CHAR_BLANK);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT6, (pSystemTime[0] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT5, (pSystemTime[1] - '0') | 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT4, (pSystemTime[3] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT3, (pSystemTime[4] - '0') | 0x80);
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT2, (pSystemTime[6] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT1, (pSystemTime[7] - '0'));
	max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT0, MAX7219_CHAR_BLANK);
}

void LtcDisplayMax72197Segment::WriteChar(uint8_t nChar, uint8_t nPos) {
	if (nPos < 8) {
		max7219_spi_write_reg(&m_DeviceInfo, MAX7219_REG_DIGIT0 + nPos, nChar);
	}
}
