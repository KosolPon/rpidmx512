/**
 * @file ws28xxmatrix.cpp
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
#include <stdbool.h>
#include <assert.h>

#include "ws28xxmatrix.h"

#if defined(USE_SPI_DMA)
 #include "h3/ws28xxdma.h"
#else
 #include "ws28xx.h"
#endif

#include "../lib-bob/src/font_cp437.h"

#include "debug.h"

// FIXME Currently working for single row only
// TODO Set RGB colour
// TODO Create colon methods

WS28xxMatrix::WS28xxMatrix(uint32_t nColumns, uint32_t nRows):
	m_nColumns(nColumns),
	m_nRows(nRows),
	m_nOffset((nRows - FONT_CP437_CHAR_H) * 2),
	m_nMaxLeds(nColumns * nRows),
	m_nMaxPosition(nColumns / FONT_CP437_CHAR_W),
	m_nMaxLine(nRows / FONT_CP437_CHAR_H),
	m_pWS28xx(0),
	m_bUpdateNeeded(false),
	m_nPosition(0),
	m_nLine(0)
{
	assert(nColumns % FONT_CP437_CHAR_W == 0);
	assert(nRows % FONT_CP437_CHAR_H == 0);

	DEBUG_PRINTF("m_nColumns=%u, m_nRows=%u, m_nOffset=%u, m_nMaxPosition=%u, m_nMaxLine=%u", m_nColumns, m_nRows, m_nOffset, m_nMaxPosition, m_nMaxLine);
}

WS28xxMatrix::~WS28xxMatrix(void) {
}

void WS28xxMatrix::Init(TWS28XXType tLedType) {
	assert(m_pWS28xx == 0);
#if defined(USE_SPI_DMA)
	m_pWS28xx = new WS28xxDMA(tLedType, m_nMaxLeds);
#else
	m_pWS28xx = new WS28xx(tLedType, m_nMaxLeds);
#endif
	assert(m_pWS28xx != 0);

	m_pWS28xx->Initialize();
}

void WS28xxMatrix::PutChar(uint8_t nChar) {
	if (nChar >= (sizeof(cp437_font)/sizeof(cp437_font[0]))) {
		nChar = ' ';
	}

	while (m_pWS28xx->IsUpdating()) {
		// wait for completion
	}

	uint32_t nOffset = (FONT_CP437_CHAR_W * FONT_CP437_CHAR_H) * m_nPosition;

	const uint8_t nRed = 0x10;
	const uint8_t nGreen = 0x10;
	const uint8_t nBlue = 0x10;

	for (uint32_t nWidth = 0; nWidth < FONT_CP437_CHAR_W; nWidth++) {
		for (uint32_t nHeight = 0; nHeight < FONT_CP437_CHAR_H; nHeight++) {
			uint8_t nByte = cp437_font[nChar][nWidth];

			if ((nWidth & 0x1) != 0) {
				nByte = ReverseBits(nByte);
			}

			if (nByte & (1 << nHeight)) {
				m_pWS28xx->SetLED(nOffset, nRed, nGreen, nBlue);
			} else {
				m_pWS28xx->SetLED(nOffset, 0, 0, 0);
			}

			nOffset++;
		}
	}

	m_nPosition++;

	if (m_nPosition == m_nMaxPosition ) {
		m_nPosition = 0;
		m_nLine++;

		if (m_nLine == m_nMaxLine) {
			m_nLine = 0;
		}
	}

	m_bUpdateNeeded = true;
}

void WS28xxMatrix::PutString(const char *pString) {
	uint8_t nChar;

	while ((nChar = *pString++) != 0) {
		PutChar(nChar);
	}
}

void WS28xxMatrix::Text(const char *pText, uint8_t nLength) {
	if (nLength > m_nMaxPosition) {
		nLength = m_nMaxPosition;
	}

	for (uint32_t i = 0; i < nLength; i++) {
		PutChar(pText[i]);
	}
}

/*
 * 1 is top line
 */
void WS28xxMatrix::TextLine(uint8_t nLine, const char *pText, uint8_t nLength) {
	if ((nLine == 0) || (nLine > m_nMaxLine)) {
		return;
	}

	SetCursorPos(0, nLine - 1);
	Text(pText, nLength);
}

/*
 * 1 is top line
 */
void WS28xxMatrix::ClearLine(uint8_t nLine) {
	if ((nLine == 0) || (nLine > m_nMaxLine)) {
		return;
	}

	for (uint32_t i = 0; i < m_nMaxLeds; i++) {
		m_pWS28xx->SetLED(i, 0, 0, 0); // FIXME Currently working for single row only
	}

	SetCursorPos(0, nLine - 1);
}

/**
 * 0,0 is top left
 */
void WS28xxMatrix::SetCursorPos(uint8_t nCol, uint8_t nRow) {
	if ((nCol >= m_nMaxPosition) || (nRow >= m_nMaxLine)) {
		return;
	}

	m_nPosition = nCol;
	m_nLine = nRow;
}

void WS28xxMatrix::Cls(void) {
	while (m_pWS28xx->IsUpdating()) {
		// wait for completion
	}
	m_pWS28xx->Blackout();
}

void WS28xxMatrix::Show(void) {
	if (m_bUpdateNeeded) {
		m_bUpdateNeeded = false;
		m_pWS28xx->Update();
	}
}

uint8_t WS28xxMatrix::ReverseBits(uint8_t nBits) {
	const uint32_t input = (uint32_t) nBits;
	uint32_t output;
	asm("rbit %0, %1" : "=r"(output) : "r"(input));
	return (uint8_t) (output >> 24);
}
