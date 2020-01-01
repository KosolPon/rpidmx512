#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <ws28xxdisplaymatrix.h>

#include "hardware.h"
#include "networkh3emac.h"
#include "ledblink.h"

#include "display.h"

#include "spiflashinstall.h"
#include "spiflashstore.h"
#include "remoteconfig.h"
#include "remoteconfigparams.h"
#include "storeremoteconfig.h"

#include "mdns.h"
#include "mdnsservices.h"

#include "firmwareversion.h"

#include "h3.h"
#include "h3_gpio.h"
#include "h3_board.h"

#include "debug.h"

#include "h3/ws28xxdma.h"

#include "ltcdisplayws28xx.h"


//#define LENGTH (170 * 4 * 3 * 8)	//	16320 -> 16 kB
// (128 * 4 * 4 * 8) // 16384 -> 16 kB

#define LED_COUNT 	(600) //(32 * 8)

static const char SOFTWARE_VERSION[] = "0.0";

extern "C" {

void notmain(void) {
	Hardware hw;
	NetworkH3emac nw;
	LedBlink lb;
	Display oled(0,4);
	FirmwareVersion fw(SOFTWARE_VERSION, __DATE__, __TIME__);

	SpiFlashInstall spiFlashInstall;
	SpiFlashStore spiFlashStore;

	fw.Print();

	nw.Init((NetworkParamsStore *)spiFlashStore.GetStoreNetwork());
	nw.Print();

	MDNS mDns;

	mDns.Start();
	mDns.AddServiceRecord(0, MDNS_SERVICE_CONFIG, 0x2905);
	mDns.AddServiceRecord(0, MDNS_SERVICE_TFTP, 69);
	mDns.Print();

	RemoteConfig remoteConfig(REMOTE_CONFIG_LTC, REMOTE_CONFIG_MODE_TIMECODE, 0);

	StoreRemoteConfig storeRemoteConfig;
	RemoteConfigParams remoteConfigParams(&storeRemoteConfig);

	if (remoteConfigParams.Load()) {
		remoteConfigParams.Set(&remoteConfig);
		remoteConfigParams.Dump();
	}

	while (spiFlashStore.Flash())
		;

	/*
	 ********************************************
	 */

	h3_gpio_fsel(3, GPIO_FSEL_OUTPUT);
	h3_gpio_clr(3);
	h3_gpio_fsel(6, GPIO_FSEL_OUTPUT);
	h3_gpio_clr(6);
	h3_gpio_fsel(20, GPIO_FSEL_OUTPUT);
	h3_gpio_clr(20);

	/*
	 ********************************************
	 */

//	WS28xxDMA ws28xx(WS2812B, LED_COUNT);
//	ws28xx.Initialize();

//	WS28xxDisplayMatrix matrix(32, 8);
//	matrix.Init();

	//LtcDisplayWS28xx ltcDisplayWS28xx(LTCDISPLAYWS28XX_TYPE_MATRIX);
	LtcDisplayWS28xx ltcDisplayWS28xx(LTCDISPLAYWS28XX_TYPE_7SEGMENT);
	ltcDisplayWS28xx.Init(WS2812B, 0xFF);
	ltcDisplayWS28xx.Print();

	/*
	 ********************************************
	 */

//	uint8_t nRed = 0, nGreen = 0, nBlue = 0;
//	uint32_t nColour = 0;

//	time_t ltime;
//	struct tm *local_time;

	uint32_t frames_prev = 60;
	const uint32_t nTimeStart = hw.Millis();

	for (;;) {
		nw.Run();
		mDns.Run();
		remoteConfig.Run();
		spiFlashStore.Flash();
		lb.Run();

		/*
		 ********************************************
		 */

		ltcDisplayWS28xx.Run();

//		ltime = time(NULL);
//		local_time = localtime(&ltime);

		uint32_t nTime = hw.Millis() - nTimeStart;
		const uint32_t hours = nTime / (uint32_t) 3600000;
		nTime -= hours * (uint32_t) 3600000;
		const uint32_t minutes = nTime / (uint32_t) 60000;
		nTime -= minutes * (uint32_t) 60000;
		const uint32_t seconds = nTime / (uint32_t) 1000;
		const uint32_t millis = nTime - seconds * (uint32_t) 1000;
		const uint32_t frames = (float) millis / ( 1000 / 25);

		char stime[16];
		sprintf(stime, "%.2d:%.2d:%.2d.%.2d", hours, minutes, seconds, frames);

		if (frames_prev != frames) {
			frames_prev = frames;
			ltcDisplayWS28xx.Show(stime);
		}

		//matrix.PutString((const char *)stime);

//		matrix.TextLine(1, (const char *)stime, 4);

		//matrix.PutString("8800");

//		matrix.Show();

		/*
		 ********************************************
		 */

//		if (!ws28xx.IsUpdating()) {
//			h3_gpio_set(6);
//
//			//udelay(500);
//
//			switch (nColour) {
//				case 0:
//					nRed++;
//					break;
//				case 1:
//					nGreen++;
//					break;
//				case 2:
//					nBlue++;
//					break;
//				case 3:
//					nRed++;
//					nGreen++;
//					nBlue++;
//					break;
//				default:
//				break;
//			}
//
//			for (uint32_t nIndex = 0; nIndex < LED_COUNT; nIndex++) {
////				ws28xx.SetLED(nIndex, nRed, nGreen, nBlue);
//			}
//
//			ltime = time(NULL);
//			local_time = localtime(&ltime);
//
//			char stime[5];
//			sprintf(stime, "%.2d%.2d", local_time->tm_min, local_time->tm_sec);
//
//			matrix.PutString((const char *)stime);
//			//matrix.PutString("Film");
//
//			ws28xx.Update();
//
//			switch (nColour) {
//			case 0:
//				if (nRed == 0xFF) {
//					nColour = 1;
//					nRed = 0;
//				}
//				break;
//			case 1:
//				if (nGreen == 0xFF) {
//					nColour = 2;
//					nGreen = 0;
//				}
//				break;
//			case 2:
//				if (nBlue == 0xFF) {
//					nColour = 0;
//					nBlue = 0;
//				}
//				break;
//			case 3:
//				if (nRed == 0xFF) {
//					nColour = 0;
//					nRed = 0;
//					nGreen = 0;
//					nBlue = 0;
//				}
//				break;
//			default:
//				break;
//			}
//
//			h3_gpio_clr(6);
//		}

		/*
		 ********************************************
		 */
	}
}

}
