/**
 * @file tcnetsendtimecode.cpp
 *
 */
/* Copyright (C) 2020 by Arjan van Vught mailto:info@raspberrypi-dmx.nl
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

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "tcnet.h"
#include "tcnettimecode.h"

#include "tcnetpackets.h"

#include "hardware.h"
#include "network.h"

#include "debug.h"

void TCNet::SendTimeCode(const struct TTCNetTimeCode *ptTCNetTimeCode) {
	if ((TTCNetNodeType) m_tOptIn.ManagementHeader.NodeType != TCNET_TYPE_MASTER) {
		return;
	}

	assert(ptTCNetTimeCode != 0);

	memcpy((void *)&m_TTCNet.TCNetPacket.Time.ManagementHeader, (void *)&m_tOptIn.ManagementHeader, sizeof(struct TTCNetPacketManagementHeader));
	m_TTCNet.TCNetPacket.Time.ManagementHeader.MessageType = TCNET_MESSAGE_TYPE_TIME;
	m_TTCNet.TCNetPacket.Time.ManagementHeader.SEQ = m_nSeqTimeMessage++;
	m_TTCNet.TCNetPacket.Time.ManagementHeader.TimeStamp = Hardware::Get()->Micros();

	if (m_tLayer != TCNET_LAYER_UNDEFINED) {
		uint32_t nMillis = (float) ptTCNetTimeCode->nFrames * m_fTypeDivider;
		nMillis += ptTCNetTimeCode->nSeconds * 1000;
		nMillis += ptTCNetTimeCode->nMinutes * 60000;
		nMillis += ptTCNetTimeCode->nHours * 3600000;

		*m_pLTime = nMillis;
	} else {
		m_TTCNet.TCNetPacket.Time.LMTimeCode.Frames = ptTCNetTimeCode->nFrames;
		m_TTCNet.TCNetPacket.Time.LMTimeCode.Seconds = ptTCNetTimeCode->nSeconds;
		m_TTCNet.TCNetPacket.Time.LMTimeCode.Minutes = ptTCNetTimeCode->nMinutes;
		m_TTCNet.TCNetPacket.Time.LMTimeCode.Hours = ptTCNetTimeCode->nHours;

		switch (ptTCNetTimeCode->nType) {
			case TCNET_TIMECODE_TYPE_FILM:
				m_TTCNet.TCNetPacket.Time.LMTimeCode.SMPTEMode = 24;
				break;
			case TCNET_TIMECODE_TYPE_EBU_25FPS:
				m_TTCNet.TCNetPacket.Time.LMTimeCode.SMPTEMode = 25;
				break;
			case TCNET_TIMECODE_TYPE_DF:
				m_TTCNet.TCNetPacket.Time.LMTimeCode.SMPTEMode = 29;
				break;
			case TCNET_TIMECODE_TYPE_SMPTE_30FPS:
				m_TTCNet.TCNetPacket.Time.LMTimeCode.SMPTEMode = 30;
				break;
			default:
				break;
		}
	}

	Network::Get()->SendTo(m_aHandles[1], (const uint8_t *) &m_TTCNet.TCNetPacket.Time, (uint16_t) sizeof(struct TTCNetPacketTime), m_tNode.nIPAddressBroadcast, TCNET_BROADCAST_PORT_1);

}
