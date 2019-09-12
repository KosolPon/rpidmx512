/**
 * @file dump.cpp
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
#include <stdio.h>
#include <assert.h>

#include "llrpdevice.h"

#include "e133.h"
#include "rdm_e120.h"

#include "rdmmessage.h"

#include "debug.h"

void LLRPDevice::DumpCommon(void) {
#ifndef NDEBUG
	const struct TLLRPCommonPacket *packet = (struct TLLRPCommonPacket *) &(m_tLLRP.LLRPPacket.Common);
	printf("RootLayerPreAmble.PreAmbleSize=0x%.04x\n", (int) __builtin_bswap16(packet->RootLayerPreAmble.PreAmbleSize));
	printf("RootLayerPreAmble.PostAmbleSize=0x%.04x\n", (int) __builtin_bswap16(packet->RootLayerPreAmble.PostAmbleSize));
	printf("RootLayerPreAmble.ACNPacketIdentifier=[%s]\n", packet->RootLayerPreAmble.ACNPacketIdentifier);

	uint8_t *pdu_buf = (uint8_t *)packet->RootLayerPDU.FlagsLength;
	uint32_t nLength = ((uint32_t)((pdu_buf[0] & 0x0fu) << 16) | (uint32_t)(pdu_buf[1] << 8) | (uint32_t)pdu_buf[2]);

	printf("RootLayerPDU PDU length=%d, High 4 bits=%.1x\n", (int) nLength, (int) packet->RootLayerPDU.FlagsLength[0] >> 4);
	printf("RootLayerPDU.Vector=0x%.8x\n", (int) __builtin_bswap32(packet->RootLayerPDU.Vector));
	printf("RootLayerPDU.SenderCid ");

	for (uint32_t i = 0; i < sizeof(packet->RootLayerPDU.SenderCid); i++) {
			printf("%.02x", packet->RootLayerPDU.SenderCid[i]);
	}
	printf("\n");

	pdu_buf = (uint8_t *)packet->LlrpPDU.FlagsLength;
	nLength = ((uint32_t)((pdu_buf[0] & 0x0fu) << 16) | (uint32_t)(pdu_buf[1] << 8) | (uint32_t)pdu_buf[2]);

	printf("LlrpPDU PDU length=%d, High 4 bits=%.1x\n", (int) nLength, (int) packet->LlrpPDU.FlagsLength[0] >> 4);
	printf("LlrpPDU.Vector=0x%.8x\n", (int) __builtin_bswap32(packet->LlrpPDU.Vector));
	printf("LlrpPDU.DestinationCid ");

	for (uint32_t i = 0; i < sizeof(packet->LlrpPDU.DestinationCid); i++) {
			printf("%.02x", packet->LlrpPDU.DestinationCid[i]);
	}
	printf("\n");

	printf("LlrpPDU.TransactionNumber=0x%.4x\n", (int) __builtin_bswap32(packet->LlrpPDU.TransactionNumber));

	switch (__builtin_bswap32(packet->LlrpPDU.Vector)) {
	case VECTOR_LLRP_PROBE_REQUEST:
		{
			const struct TProbeRequestPDUPacket *pRequest = (struct TProbeRequestPDUPacket *) &(m_tLLRP.LLRPPacket.Request);
			pdu_buf = (uint8_t *)pRequest->ProbeRequestPDU.FlagsLength;
			nLength = ((uint32_t)((pdu_buf[0] & 0x0fu) << 16) | (uint32_t)(pdu_buf[1] << 8) | (uint32_t)pdu_buf[2]);

			printf("Probe Request PDU length=%d, High 4 bits=%.1x\n", (int) nLength, (int) pRequest->ProbeRequestPDU.FlagsLength[0] >> 4);
			printf("ProbeRequestPDU.Vector=0x%.2x\n", (int) pRequest->ProbeRequestPDU.Vector);

			printf("ProbeRequestPDU.Filter=0x%.4x\n", (int) __builtin_bswap16(pRequest->ProbeRequestPDU.Filter));
		}
		break;
	case VECTOR_LLRP_PROBE_REPLY:
		DEBUG_PUTS("VECTOR_LLRP_PROBE_REPLY");
		break;
	case VECTOR_LLRP_RDM_CMD:
		{
			struct LTRDMCommandPDUPacket *pRDMCommand = (struct LTRDMCommandPDUPacket *) &(m_tLLRP.LLRPPacket.Request);
			pdu_buf = (uint8_t *)pRDMCommand->RDMCommandPDU.FlagsLength;
			nLength = ((uint32_t)((pdu_buf[0] & 0x0fu) << 16) | (uint32_t)(pdu_buf[1] << 8) | (uint32_t)pdu_buf[2]);

			printf("Probe Request PDU length=%d, High 4 bits=%.1x\n", (int) nLength, (int) pRDMCommand->RDMCommandPDU.FlagsLength[0] >> 4);
			printf("ProbeRequestPDU.Vector=0x%.2x\n", (int) pRDMCommand->RDMCommandPDU.Vector);

			pRDMCommand->RDMCommandPDU.Vector = E120_SC_RDM;; // QuickAndDirty
			RDMMessage::Print(&pRDMCommand->RDMCommandPDU.Vector);
		}
		DEBUG_PUTS("VECTOR_LLRP_RDM_CMD");
		break;
	default:
		break;
	}
#endif
}

void LLRPDevice::DumpLLRP(void) {
	const struct TLLRPCommonPacket *packet = (struct TLLRPCommonPacket*) &(m_tLLRP.LLRPPacket.Common);

	printf("SenderCID: ");

	for (uint32_t i = 0; i < sizeof(packet->RootLayerPDU.SenderCid); i++) {
		printf("%.02X", packet->RootLayerPDU.SenderCid[i]);
	}

	printf(" DestinationCID: ");

	for (uint32_t i = 0; i < sizeof(packet->LlrpPDU.DestinationCid); i++) {
		printf("%.02X", packet->LlrpPDU.DestinationCid[i]);
	}
	printf("\n");
}

void LLRPDevice::DumpRdmMessageIn(void) {
	const struct LTRDMCommandPDUPacket *pRDMCommand = (struct LTRDMCommandPDUPacket *) &(m_tLLRP.LLRPPacket.Request);
	const struct TRdmMessage *pRdmDataIn = (struct TRdmMessage *)pRDMCommand->RDMCommandPDU.RDMData;

	uint8_t dummy[512];
	dummy[0] = E120_SC_RDM;
	memcpy(&dummy[1], pRdmDataIn, pRdmDataIn->message_length);

	RDMMessage::Print(dummy);
}