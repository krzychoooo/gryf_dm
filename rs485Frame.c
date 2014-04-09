/*
 * rs485Frame.c
 *
 *  Created on: 7 kwi 2014
 *      Author: krzysztofklimas@interia.pl
 */

#include <util/crc16.h>
#include "rs485Frame.h"
#include "radioFrame.h"
#include "USART/usartd0.h"

TModuleConfig moduleConfig;
TModuleConfig eeModuleConfig EEMEM;
uint8_t state;

void copyModuleConfigEEpromToRam() {
	eeprom_read_block(&moduleConfig, &eeModuleConfig, sizeof(moduleConfig));
}

void copyModuleConfigRamToEEprom() {
	eeprom_write_block(&moduleConfig, &eeModuleConfig, sizeof(moduleConfig));
}

void sendAlarmFrame() {
	uint16_t crc16;

	crc16 = 0xffff;
	putchare0(0x00);							//MCA
	crc16 = _crc16_update(crc16, 0);
	putchare0(inFrameBufferWrIndex);		//size
	crc16 = _crc16_update(crc16, inFrameBufferWrIndex);
	putchare0(moduleConfig.myAddress);		//MDA
	crc16 = _crc16_update(crc16, moduleConfig.myAddress);
	putchare0(state);						//MD Dat  stan modu³u
	crc16 = _crc16_update(crc16, state);
	for (inFrameBufferRdIndex = 0; inFrameBufferRdIndex != inFrameBufferWrIndex; inFrameBufferRdIndex++) {
		putchare0(inFrameBuffer[inFrameBufferRdIndex]);
		crc16 = _crc16_update(crc16, inFrameBuffer[inFrameBufferRdIndex]);
	}
	putchare0((uint8_t) crc16);
	putchare0((uint8_t) crc16 >> 8);
	inFrameBufferRdIndex = 0;
	inFrameBufferWrIndex = 0;
}

uint8_t getFrameFromMc() {
	uint16_t iData;
	uint8_t myAddress;
	uint8_t frameType;
	uint8_t i;
	uint16_t crc16;

	if (rx_counter_usarte0) {
		crc16 = 0xffff;
		iData = getchare0time(0x0a);		// 10ms wait
		if ((iData & 0x0100) == 0)
			return 1;
		myAddress = (uint8_t) iData;
		crc16 = _crc16_update(crc16, myAddress);
		iData = getchare0time(10);
		frameType = (uint8_t) iData;
		crc16 = _crc16_update(crc16, myAddress);

		// read crc
		iData = getchare0time(10);
		i = (uint8_t) iData;
		crc16 = _crc16_update(crc16, i);
		iData = getchare0time(10);
		i = (uint8_t) iData;
		crc16 = _crc16_update(crc16, i);

		if (myAddress != moduleConfig.myAddress)
			return 4;
		if (crc16 != 0)
			return 5;

		return 0;
	} else
		return 1;
}

void userSetRs485() {
	uint16_t getData;
	uint8_t i;
	char buffer[10];

	printf("%cUSTAWIENIA RS485\n",12);

	while (1) {
		printf("0 Wyjœcie z edycji\n");
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(stringBaudRateTable[baudRateIndex])));
		printf("1 Szybkoœæ transmisji %s", buffer);
		printf("2 Adres modu³u %d\n", moduleConfig.myAddress);
		printf("\nWybierz numer parametru do edycji\n");

		scanf("%d", &getData);
		switch (getData) {
		case 0: {
			printf("Koniec konfiguracji radia\n");
			return;
			break;
		}
		case 1: {
			for (i = 0; i != (uint8_t) 9; i++) {
				strcpy_P(buffer, (PGM_P) pgm_read_word(&(stringBaudRateTable[i])));
				printf("%d %s\n", i, buffer);
			}
			printf("wybierz numer prêdkoœci\n");
			scanf("%d", &getData);
			baudRateIndex = (uint8_t) getData;
			eeprom_write_byte(&eeBaudRateIndex, baudRateIndex);
			break;
		}
		case 2: {
			printf("podaj adres\n");
			scanf("%d", &getData);
			moduleConfig.myAddress = (uint8_t) getData;
			copyModuleConfigRamToEEprom();
			break;
		}
		default: {
			printf("Z£Y WYBÓR\n\n\n");
		}
		}
	}
}

