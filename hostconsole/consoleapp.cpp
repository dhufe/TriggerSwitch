#include <iostream>
#include <iomanip>
#include <cstdio>

#include <thread>
#include <chrono>
#include <cstring>
using namespace std;

#include <getopt.h>
#include "hidapi.h"

#include "usb_hid_raw.h"
#include "usbpacket.h"

/* Flag set by ‘--verbose’. */
static int verbose_flag;

uint16_t crc16( uint8_t *data, unsigned short size) {
    uint16_t out  = 0;
    int bits_read = 0, bit_flag;
    
    /* Sanity check: */
    if(data == NULL)
        return 0;
    
    while(size > 0)
    {
        bit_flag = out >> 15;
        
        /* Get next bit: */
        out <<= 1;
        out |= (*data >> (7 - bits_read)) & 1;
        
        /* Increment bit counter: */
        bits_read++;
        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }
        
        /* Cycle check: */
        if(bit_flag)
            out ^= USB_CRC_POLY;
        
    }
    return out;
}

void printTable ( unsigned char* ptr, unsigned long szLen ) {
    unsigned int nLineCnt = 0;
    for ( unsigned long y = 0; y < szLen; ++y) {
        if ( ( ( y ) % 16 ) == 0 ) {
            if ( y ) {
                std::cout << std::endl << std::hex << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << nLineCnt << std::setw(0) << " : ";
            } else {
                std::cout << std::hex << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << nLineCnt << std::setw(0) << " : ";
            }
            nLineCnt+=16;
        }
        std::cout << std::setw(2) << std::hex << std::noshowbase << std::uppercase << (int) (ptr[y]) << std::setw(0) << " ";
    }
    std::cout << std::endl;
}

int main ( int argc, char* argv[]){
    
    int y = 0;
    
    usb_datapacket_t usbpayload;

    memset(usbpayload.pData.pnPtr, 0, USB_CRC_RANGE);
    uint8_t nbState = 0;
    int nbAction = 0;
        
    while ( 1 ) {
        static struct option long_options[] =
        {
            /* These options set a flag. */
            { "on", no_argument, &nbAction, 1 },
            { "off", no_argument, &nbAction, 0 },
            { "toggle", no_argument, &nbAction, 2 },
            { "switch", no_argument, &nbAction, 3 },
            { "osc", no_argument, &nbAction, 4 },
            {"state",   required_argument, 0,                 's'},
            {"mask",    required_argument, 0,                 'm' },
            {"verbose", required_argument, &verbose_flag,        1},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;
        
        y = getopt_long (argc, argv, "s:u:", long_options, &option_index);
        
        /* Detect the end of the options. */
        if (y == -1)
            break;
        
        switch ( y ) {
            case 's':
                nbState = atoi(optarg);
                if (verbose_flag) {
                    std::cout << "nbState : " << (int)(nbState) << std::endl;
                }
                usbpayload.pData.pPayload.nPinMask = (1 << nbState);
                break;
                
            case 'm':
                nbState = atoi(optarg);
                if (verbose_flag) {
                    std::cout << "nbMask : " << (int)(nbState) << std::endl;
                }
                usbpayload.pData.pPayload.nPinMask = nbState;
                break;
                            
            default:
                break;
        }
        
    }
    
   
    // USB sending and receiving stuff
    HIDDevice Node( VID, PID );
    
    try {
        Node.OpenDevice ();
    } catch (HIDException& e) {
        std::cerr << e.description() << std::endl;
        return -1;
    }
    
   

      
    usbpayload.pData.pPayload.nPinState = (uint8_t)(nbAction);

    usbpayload.nCheckSum           = crc16 ( usbpayload.pData.pnPtr, USB_CRC_RANGE );
    
    if (verbose_flag) {
        std::cout << std::endl << "resulting data package: " << std::endl;
        printTable((unsigned char*)(&usbpayload), sizeof(usb_datapacket_t) / sizeof(char));
        std::cout << std::endl;
    }
    try {
        Node.WriteDevice ( (const uint8_t*) (&usbpayload) , sizeof(usb_datapacket_t) / sizeof (char) );
    } catch ( HIDException& e) {
        std::cerr << e.description() << std::endl;
        return -1;
    }
    
    if (verbose_flag) {
        std::cout << "--- reading device response ---" << std::endl;
    }

    try {
        Node.ReadDevice ( (uint8_t*) (&usbpayload), sizeof ( usb_datapacket_t ) / sizeof ( char ) );
    } catch (HIDException& e) {
        std::cerr << e.description() << std::endl;
        return -1;
    }
    
    if (verbose_flag) {
        std::cout << "reported result package: " << std::endl;
        printTable((unsigned char*)(&usbpayload), sizeof(usb_datapacket_t) / sizeof(char));
        std::cout << std::endl;
    }
    uint16_t nResCRC = crc16 ( usbpayload.pData.pnPtr, USB_CRC_RANGE );
    
    if ( nResCRC != usbpayload.nCheckSum ) {
        std::cerr << "CRC16 failed." << std::endl;
    } else {
        std::cout << "CRC16 passed." << std::endl;
    }
    
    if ( usbpayload.pData.pPayload.nResultCode == USB_NERROR ) {
        std::cout << "Device responds: everything was fine and no error occured." << std::endl;
    } else {
        std::cerr << "Device responds: something went wrong, probably a checksum failure." << std::endl;
    }
    
    if ( Node.isOpen() )
        Node.CloseDevice();
    
    return 0;
}
