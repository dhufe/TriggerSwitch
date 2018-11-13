/**
*       @file  TriggerSwitch.c
*      @brief
*
*
*     @author  Daniel Kotschate (d3v0), d3v0@d3v0.de
*
*   @internal  TriggerSwitch
*     Created  22.02.2017 12:50:42
*    Revision  ---
*    Compiler
*     Company  d3v0@d3v0.de
*   Copyright  Copyright (c) 2015, Daniel Kotschate
*
* =====================================================================================
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include "include/usb_rawhid.h"
#include "include/usbpacket.h"

#define RC_DDR                          DDRD
#define RC_PORT                         PORTD
#define RC_PIN                          PD6

#define CPU_PRESCALE(n)                 (CLKPR = 0x80, CLKPR = (n))

volatile usb_datapacket_t   curData;

uint16_t crc16( uint8_t *data, unsigned short size) {
	uint16_t out  = 0;
	int bits_read = 0, bit_flag;

	// Sanity check:
	if(data == NULL)
	return 0;

	while(size > 0)
	{
		bit_flag = out >> 15;

		// Get next bit:
		out <<= 1;
		out |= (*data >> (7 - bits_read)) & 1;

		// Increment bit counter:
		bits_read++;
		if(bits_read > 7)
		{
			bits_read = 0;
			data++;
			size--;
		}

		// Cycle check:
		if(bit_flag)
		out ^= USB_CRC_POLY;

	}
	return out;
}


int main ( void ) {
	int8_t res = 0;
	usb_init();
	while (!usb_configured()) /* wait */ ;
	_delay_ms(1000);

	RC_DDR |= ( 1 << RC_PIN  );

	while (1) {
		res = usb_rawhid_recv( (uint8_t*) (&curData), 50 );
		if (res == 64 )  {
			// CRC16 check if packet payload is correct
			uint16_t nResCRC = crc16 ( (uint8_t*) (curData.pData.pnPtr), USB_CRC_RANGE );
			if ( nResCRC != curData.nCheckSum ) {
				// update status
				curData.pData.pPayload.nResultCode = USB_CHSUMERROR;
				} else {
				// update status
				curData.pData.pPayload.nResultCode = USB_NERROR;
				// set pin state
				switch ( curData.pData.pPayload.nPinState ) {
					case USB_PIN_STATE_LOW:
						RC_PORT &= ~(curData.pData.pPayload.nPinMask);
						break;
					case USB_PIN_STATE_HIGH:
						RC_PORT |=  (curData.pData.pPayload.nPinMask);
						break;
					case USB_PIN_STATE_TGL:
						if ( RC_PORT & (curData.pData.pPayload.nPinMask) ) {
							RC_PORT &= ~(curData.pData.pPayload.nPinMask);
						} else {
							RC_PORT |=  (curData.pData.pPayload.nPinMask);
							break;
						}
					default:
					break;
					
				}
			}
			// replay updated packet for status monitoring and feedback
			curData.nCheckSum = crc16 ( (uint8_t*) (curData.pData.pnPtr), USB_CRC_RANGE );
			usb_rawhid_send( (uint8_t*) (&curData), 50 );
		}

	}

	return 0;
}
