/**
 *       @file  usbpacket.h
 *      @brief  Protocoll declaration for sending packet orientatet messages torwards the
 *              usb-hid interface.
 *
 * Detailed description starts here.
 *
 *     @author  Daniel Kotschate (daniel), daniel@d3v0.de
 *
 *   @internal
 *     Created  28.05.2015
 *    Revision  ---
 *    Compiler  gcc/g++
 *     Company  daniel@d3v0.de
 *   Copyright  Copyright (c) 2015, Daniel Kotschate
 *
 * This source code is released for free distribution under the terms of the
 * GNU General Public License as published by the Free Software Foundation.
 * =====================================================================================
 */


#ifndef __USB_PACKET_H__
#define __USB_PACKET_H__

#include <inttypes.h>

#define USB_PACKETTYPE_CONFIG   1
#define USB_PACKETTYPE_DATA     2
#define USB_PACKETTYPE_RESULT   3

// size of maximum payload      USB bulk packet - state - pin - result - dummy - checksum
//                              64 bytes
//                              -                 1
//                              -                         1
//                              -                               1
//                              -                                        1
//                              -                                                 2
//                              = 58 bytes

#define USB_MAX_PLOAD           6
#define USB_CRC_POLY            0x1021
#define USB_CRC_RANGE           4

#define USB_NERROR              1
#define USB_CHSUMERROR          2

#define USB_PIN_STATE_LOW		0
#define USB_PIN_STATE_HIGH		1
#define USB_PIN_STATE_TGL		2
#define USB_PIN_STATE_OSC		3
#define USE_PIN_STATE_SWT       4

typedef struct usb_datapacket_t {
    union  pData {
        struct pPayload {
            uint8_t   nPinState;					// pin state ( 0: low, 1: high, 2:toggle)
            uint8_t   nPinMask;						// theoretically 0-7
            uint8_t   nResultCode;                  // result type code            
            uint8_t   nDummy;						// word memory alignment
        } pPayload;
        uint8_t  pnPtr [ USB_CRC_RANGE ];
    } pData;
    uint16_t  nCheckSum;                            // checksum over payload
} usb_datapacket_t;



#endif // __USB_PACKET_H__
