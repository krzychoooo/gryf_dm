/*
 * radio_config.h
 *
 *  Created on: 3 kwi 2014
 *      Author: krzysztof
 */

#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

#include <avr/io.h>

#define NUMBER_SLAVE 8

extern uint8_t slaveModulesAddress[NUMBER_SLAVE];
extern uint8_t numberSlaveModules;
extern uint8_t EEslaveModulesAddress[NUMBER_SLAVE];
extern uint8_t EEnumberSlaveModules;
extern const uint8_t FlashslaveModulesAddress[NUMBER_SLAVE];
extern const uint8_t FlashnumberSlaveModules;

typedef struct RC1180Config{
	uint8_t uIAddress[2];
	uint8_t bIDAddres[2];
	uint8_t brAddress[2];
	uint8_t bRateRS232[2];
	uint8_t bRateRadio[2];
	uint8_t chanel[2];
	uint8_t power[2];
	uint8_t rssi[2];
	uint8_t cRCMode[2];
	uint8_t endChar[2];
	uint8_t addressMode[2];
} TRC1180Config;
#define CONFIGSIZE 11;

void radioRC1180Init();
void setDestinationAddres(uint8_t destAddress);
uint8_t sendCommandToRC1180(uint8_t command, uint8_t data);
void programMemoryRC1180(uint8_t *data, uint8_t numberData);
void copyConfigEEpromToRam();
void copyConfigRamToEEprom();
void copyConfigFlashToRam();
void setRC1180FromConfigRam();
void userSetRC1180();
void copyApplicationSetingFromFlashToRam();
void copyApplicationSetingFromEepromToRam();
void copyApplicationSetingFromRamToEeprom();


#endif /* RADIO_CONFIG_H_ */
