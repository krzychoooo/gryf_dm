/*
 * radio_config.h
 *
 *  Created on: 3 kwi 2014
 *      Author: krzysztof
 */

#ifndef RADIO_CONFIG_H_
#define RADIO_CONFIG_H_

typedef struct {
	int8_t UIAddress;
	int8_t DIDAddres;
	int8_t BrAddress;
	int8_t BRateRS232;
	int8_t BRateRadio;
	int8_t Chanel;
	int8_t Power;
	int8_t Rssi;
	int8_t CRCMode;
}T1180Config;


void setDestinationAddres(int8_t destAddress);
void sendCommandToRC1180(int8_t command);
void programMemoryRC1180(int8_t address, int8_t data);


#endif /* RADIO_CONFIG_H_ */
