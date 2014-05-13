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
		printf(" alarm %02x",inFrameBuffer[inFrameBufferRdIndex]);
		crc16 = _crc16_update(crc16, inFrameBuffer[inFrameBufferRdIndex]);
	}

	putchare0((uint8_t) crc16);
	putchare0((uint8_t) (crc16 >> 8));

	inFrameBufferRdIndex = 0;
	inFrameBufferWrIndex = 0;
}


/*
 * return error code
 * 0 - OK
 * 1 - no farame in buffer
 * 2 - already no implement
 * 3 - already no implement
 * 4 - frame OK but not my addres
 * 5 - CRC16 error
 */

uint8_t getFrameFromMc() {
	uint16_t iData;
	uint8_t myAddress;
	uint8_t frameType;
	uint8_t i;
	uint16_t crc16;
	uint8_t debugFrame;

	debugFrame = 0;

	if (rx_counter_usarte0) {
		crc16 = 0xffff;

		iData = getchare0time(0x0a);
		if(debugFrame)printf("%c",iData);
		crc16 = _crc16_update(crc16, (uint8_t) iData);
		myAddress = (uint8_t) iData;

		iData = getchare0time(10);
		if(debugFrame)printf("%c",iData);
		crc16 = _crc16_update(crc16, (uint8_t) iData);
		frameType = (uint8_t) iData;

		// read crc
		iData = getchare0time(10);
		if(debugFrame)printf("%c", (uint8_t) iData);
		i = (uint8_t) iData;
		crc16 = _crc16_update(crc16, i);

		iData = getchare0time(10);
		if(debugFrame)printf("%c", (uint8_t) iData);
		i = (uint8_t) iData;
		crc16 = _crc16_update(crc16, i);

		if(debugFrame)printf("%c%c\n\r",(uint8_t)crc16, (uint8_t)(crc16>>8));

		if (crc16 != 0)
			return 5;

		if (myAddress != moduleConfig.myAddress)
			return 4;

		return 0;
	} else
		return 1;
}

void userSetRs485() {
	uint16_t getData;
	uint8_t i;
	char buffer[10];

	printf("\n\rUSTAWIENIA RS485\n\r");

	while (1) {
		printf("0 Wyjœcie z edycji\n\r");
		strcpy_P(buffer, (PGM_P) pgm_read_word(&(stringBaudRateTable[baudRateIndex])));

		printf("1 Szybkoœæ transmisji %s\n\r", buffer);
		printf("2 Adres modu³u %d\n\r", moduleConfig.myAddress);
		printf("\nWybierz numer parametru do edycji\n\r");

		scanf("%d", &getData);
		switch (getData) {
		case 0: {
			printf("Koniec konfiguracji rs485\n\r");
			return;
			break;
		}
		case 1: {
			for (i = 0; i != (uint8_t) 9; i++) {
				strcpy_P(buffer, (PGM_P) pgm_read_word(&(stringBaudRateTable[i])));
				printf("%d %s\n\r", i, buffer);
			}
			printf("wybierz numer prêdkoœci\n\r");
			scanf("%d", &getData);
			baudRateIndex = (uint8_t) getData;
			eeprom_write_byte(&eeBaudRateIndex, baudRateIndex);
			break;
		}
		case 2: {
			printf("podaj adres\n\r");
			scanf("%d", &getData);
			moduleConfig.myAddress = (uint8_t) getData;
			copyModuleConfigRamToEEprom();
			break;
		}
		default: {
			printf("Z£Y WYBÓR\n\n\n\r");
		}
		}
	}
}

