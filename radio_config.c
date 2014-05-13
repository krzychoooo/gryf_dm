#include <avr/io.h>
#include "radio_config.h"
#include "pcb.h"
#include "USART/usartc0.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdio.h>
#include <util/delay.h>

uint8_t slaveModulesAddress[NUMBER_SLAVE];
uint8_t numberSlaveModules;
uint8_t EEslaveModulesAddress[NUMBER_SLAVE] EEMEM;
uint8_t EEnumberSlaveModules EEMEM;
uint8_t inFrameTime = 20;								//czas od wpisania zapytania w bufor RC1180 do przyjcia odpowiedzi (TODO wyliczyc na podstawie boud rate i liczby bajtów)
const uint8_t FlashnumberSlaveModules PROGMEM = NUMBER_SLAVE;
const uint8_t FlashslaveModulesAddress[NUMBER_SLAVE] PROGMEM = {1,2,3,4};

											//address, data
const TRC1180Config rc1180ConfigFlash PROGMEM = {
		{ 0x19, 0 },		//UIAddress
		{ 0x21, 1 },		//DIDAddres
		{ 0x28, 255 },		//BrAddress
		{ 0x1a, 0x01},		//System id
		{ 0x30, 5 },		//BRateRS232
		{ 0x30, 5 },		//BRateRadio
		{ 0x00, 13 },		//Chanel
		{ 0x01, 1 },		//Power
		{ 0x05, 1 },		//Rssi
		{ 0x15, 2 },		//crc
		{ 0x11, 0 },		//End character
		{ 0x14, 2 },		//address mode
		{ 0x0e, 0 },		//Member Max packet	length high	byte.
		{ 0x0f, 4 },		//Member Max packet	length low	byte.
		{ 0x10, 1 }			//Member Time before modem timeout and transmitting the buffer 1 -> 16ms
};

TRC1180Config rc1180ConfigRam;
TRC1180Config rc1180ConfigEEPROM EEMEM;

void copyRadioConfigEEpromToRam() {
	eeprom_read_block(&rc1180ConfigRam, &rc1180ConfigEEPROM, sizeof(rc1180ConfigRam));
	eeprom_read_block(slaveModulesAddress, EEslaveModulesAddress, sizeof(slaveModulesAddress));
	numberSlaveModules = eeprom_read_byte(&EEnumberSlaveModules);
}

void copyRadioConfigRamToEEprom() {
	eeprom_write_block(&rc1180ConfigRam, &rc1180ConfigEEPROM, sizeof(rc1180ConfigRam));
	eeprom_write_block(slaveModulesAddress, EEslaveModulesAddress, sizeof(slaveModulesAddress));
	eeprom_write_byte(&EEnumberSlaveModules, numberSlaveModules);
}

void copyRadioConfigFlashToRam() {
	memcpy_P(&rc1180ConfigRam, &rc1180ConfigFlash, sizeof(rc1180ConfigRam));
	memcpy_P(slaveModulesAddress,FlashslaveModulesAddress , sizeof(FlashslaveModulesAddress));
	numberSlaveModules = pgm_read_byte(&FlashnumberSlaveModules);
}

void setRC1180FromConfigRam() {

	programMemoryRC1180(&rc1180ConfigRam.uIAddress[0], sizeof(rc1180ConfigRam));
}

void radioRC1180Init() {
	CONFIG_RADIO_DIR_OUT;
	putcharc0('X');
	CONFIG_RADIO_OFF;
}

void setDestinationAddres(uint8_t destAddress) {
	sendCommandToRC1180('T', destAddress);
}

void readConfigMemoryRC1180(void){
	uint8_t maddress, data;

	for(maddress=0; maddress!=0x54; maddress++){
		printf("%2x=",maddress);
		data = sendCommandToRC1180('Y', maddress);
		_delay_ms(50);
		printf("%2x   ",data);
	}
}

uint8_t sendCommandToRC1180(uint8_t command, uint8_t data) {
	uint8_t retData = 0;
	uint8_t j;

	while(rx_counter_usartc0)getcharc0();
	CONFIG_RADIO_ON;  //linia config -> 0V
	j = getcharc0();  // get '>'
	switch (command) {

	case 'C': {			// set chanel
		putcharc0(command);
		j = getcharc0();
		putcharc0(data);
		j = getcharc0();
		break;
	}
	case 'P': {			// set Power
		putcharc0(command);
		j = getcharc0();
		putcharc0(data);
		j = getcharc0();
		break;
	}
	case 'T': {			// set target addres DID
		putcharc0(command);
		j = getcharc0();
		putcharc0(data);
		j = getcharc0();
		break;
	}
	case 'S': {			//  get RSSI
		putcharc0(command);
		retData = getcharc0();
		break;
	}
	case 'Y': {				//  get data from eeprom
		putcharc0(command);
		j = getcharc0();
		putcharc0(data);	// send address
		retData = getcharc0();
		break;
	}
	case 'X': {			// end Command mode
		putcharc0(command);
		break;
	}
	case 'Z': {			// enter SLEEP
		putcharc0(command);
		j = getcharc0();
		break;
	}
	case 'R': {			// enter ROUTER MODE (ONLY FOR TINY MESH VERSION)
		putcharc0(command);
		j = getcharc0();
		break;
	}
	case 'G': {			// enter GATEWAY MODE (ONLY FOR TINY MESH VERSION)
		putcharc0(command);
		j = getcharc0();
		break;
	}
	case 'N': {			// enter END NODE (ONLY FOR TINY MESH VERSION)
		putcharc0(command);
		j = getcharc0();
		break;
	}
	}

	CONFIG_RADIO_OFF;		//linia config -> VCC
	if (command != 'X')
		putcharc0('X');
	return retData;
}

/**
 * param *data		pointer to table data
 * param numberData	number to sent data  (numberData = size structure TRC1180Config)  {adr1, data1, ..... adrn, datan}
 */
void programMemoryRC1180(uint8_t *data, uint8_t numberData) {
	uint8_t i;
	char j;

	CONFIG_RADIO_ON;		// set 0V

	printf("\n\rprogramMemoryRC1180\n\r");

	j = getcharc0();		// rec '>'

	putcharc0('M');
	printf("enter prog mode\n\r");
	j = getcharc0();
	printf("prog mode active\n\r");
	for (i = 0; i != numberData/2; i++) {
		printf("%02x=",*data);
		putcharc0(*data++);
		printf("%02x  ",*data);
		putcharc0(*data++);
	}

	putcharc0(0xff);
	printf("enter end programming\n\r");
	j = getcharc0();
	printf("end programming\n\r");
	putcharc0('X');
	CONFIG_RADIO_OFF;
	_delay_ms(32);
}

void printNewVal() {
	printf("podaj now¹ wartoœæ\n");
}

/*
 * 	uint8_t UIAddress;
 uint8_t DIDAddres;
 uint8_t BrAddress;
 uint8_t BRateRS232;
 uint8_t BRateRadio;
 uint8_t Chanel;
 uint8_t Power;
 uint8_t Rssi;
 uint8_t CRCMode;
 */
void userSetRC1180(void) {
	uint8_t i;
	uint16_t getData;
//	uint8_t *ptr;

	printf("%cUSTAWIENIA RADIOWE\n",12);

	while (1) {
		printf("\n\r0 Wyjœcie z edycji\n\r");
		printf("1 Unikatowy adres ->%d\n\r", rc1180ConfigRam.uIAddress[1]);
		printf("2 Adres rozg³oszeniowy ->%d\n\r", rc1180ConfigRam.brAddress[1]);
		printf("3 Szybkoœæ transmisji ->%d\n\r", rc1180ConfigRam.bRateRS232[1]);
		printf("4 Kana³ radiowy ->%d\n\r", rc1180ConfigRam.chanel[1]);
		printf("5 Moc od 1 do 5 ->%d\n\r", rc1180ConfigRam.power[1]);
		printf("6 CRC 0-brak; 2-kontrola aktywna ->%d\n\r", rc1180ConfigRam.cRCMode[1]);
		printf("7 Liczba modu³ów slave ->%d\n\r",numberSlaveModules);
		printf("8 Adresy modu³ów slave\n\r");
		for(i=0; i != sizeof(slaveModulesAddress); i++){
				printf("  %X",slaveModulesAddress[i]);
		}
		printf("\n\r9 System id ->%d\n\r",rc1180ConfigRam.sysAddress[1]);
		printf("10 Odczyr eeprom z RC1180\n\r");
		printf("11 przywracanie ustawieñ fabrycznych\n\r");
		printf("12 Ustaw RC1180\n\r");

		printf("\n\rWybierz numer parametru do edycji\n\r");
//		printf("endchar%0x\n\r",rc1180ConfigRam.endChar[1]);
//		printf("adresmode%0x\n\r",rc1180ConfigRam.addressMode[1]);
//		printf("packedlen%0x\n\r",rc1180ConfigRam.packet_lenght_l[1]);
//		printf("timeout%0x\n\r",rc1180ConfigRam.modemTimeOut[1]);

		scanf("%d", &getData);
		switch (getData) {
		case 0: {
					printf("Koniec konfiguracji radia\n");
					copyRadioConfigRamToEEprom();
					copyApplicationSetingFromRamToEeprom();
					return;
					break;
				}
		case 1: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.uIAddress[1] = (uint8_t) getData;
			break;
		}
		case 2: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.brAddress[1] = (uint8_t) getData;
			break;
		}
		case 3: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.bRateRS232[1] = (uint8_t) getData;
			break;
		}
		case 4: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.chanel[1] = (uint8_t) getData;
			break;
		}
		case 5: {
			printNewVal();
			scanf("%i", &getData);
			rc1180ConfigRam.power[1] = (uint8_t) getData;
			break;
		}
		case 6: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.cRCMode[1] = (uint8_t) getData;
			break;
		}
		case 7: {
			printNewVal();
			scanf("%d", &getData);
			numberSlaveModules = (uint8_t) getData;
			break;
		}
		case 8: {
			for (i = 0; i != sizeof(slaveModulesAddress); i++) {
				printf("element %d ",i+1);printNewVal();
				scanf("%d", &getData);
				slaveModulesAddress[i] = (uint8_t) getData;
			}
			break;
		}
		case 9: {
			printNewVal();
			scanf("%d", &getData);
			rc1180ConfigRam.sysAddress[1] = (uint8_t) getData;
			break;
		}
		case 10: {
			readConfigMemoryRC1180();
			break;
		}
		case 11: {
			copyRadioConfigFlashToRam();
			break;
		}
		case 12: {
			setRC1180FromConfigRam();
			break;
		}
		default: {
			printf("wybierz poprawny numer");
		}
		}
	}
}

void copyApplicationSetingFromFlashToRam(){
	uint8_t i;
	for(i=0; i!=sizeof(slaveModulesAddress); i++){
		slaveModulesAddress[i] = pgm_read_byte(&FlashslaveModulesAddress[i]);
	}
	numberSlaveModules = pgm_read_byte(&FlashnumberSlaveModules);
}

void copyApplicationSetingFromEepromToRam(){
	uint8_t i;
		for(i=0; i!=sizeof(slaveModulesAddress); i++){
			slaveModulesAddress[i] = eeprom_read_byte(&EEslaveModulesAddress[i]);
		}
		numberSlaveModules = eeprom_read_byte(&EEnumberSlaveModules);
}

void copyApplicationSetingFromRamToEeprom(){
	uint8_t i;
		for(i=0; i!=sizeof(slaveModulesAddress); i++){
			 eeprom_write_byte(&slaveModulesAddress[i], EEslaveModulesAddress[i]);
		}
		 eeprom_write_byte(&numberSlaveModules, EEnumberSlaveModules);
}


