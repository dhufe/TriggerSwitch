
#include <iostream>
#include <cstdio>
#include <thread>
#include "hidapi.h"
#include "usb_hid_raw.h"
#include "usb_hid_exception.h"

using namespace std;

HIDDevice::HIDDevice ( uint16_t nwVID, uint16_t nwPID )
    : bOpen         ( false )
    , pHIDHandle    ( NULL )
    , nwVendorID    ( nwVID )
    , nwProductID   ( nwPID )
{
    hid_init();
}

HIDDevice::~HIDDevice ( void ) {
    if ( this->pHIDHandle )
        CloseDevice();

    hid_exit();
}


bool HIDDevice::OpenDevice ( void ) {

    if ( this->pHIDHandle )
        return false;

    this->pHIDHandle = hid_open ( nwVendorID , nwProductID, NULL );

    if ( this->pHIDHandle == NULL ) {
        throw HIDException ( "HID Open : Failed to open!" );
        return false;
    } else {
        int iNonBlocking = 0;
        iNonBlocking     = hid_set_nonblocking ( this->pHIDHandle, 1 );
        if ( iNonBlocking == -1 ) {
            throw HIDException ( "HID Open : error setting non-blocking access.");
            return false;
        }
    }


    bOpen = true;
    return true;
}

bool HIDDevice::OpenDevice (  uint16_t nwVID, uint16_t nwPID ) {
    if ( this->pHIDHandle )
        return false;

    this->nwVendorID  = nwVID;
    this->nwProductID = nwPID;

    return OpenDevice();
}

void HIDDevice::CloseDevice ( void ) {
    if ( this->pHIDHandle )
        hid_close ( this->pHIDHandle );

    this->pHIDHandle = NULL;
    bOpen            = false;
}

bool HIDDevice::WriteDevice ( const uint8_t* ptr, size_t szLen ) {

    if ( bOpen == false )
        return false;

    if ( ! this->pHIDHandle )
        return false;

#if 1
    unsigned char* pcData = (unsigned char*)(calloc( szLen + 1 , sizeof(unsigned char)));
    memcpy(&(pcData[1]), ptr, szLen);
    int iResult = hid_write ( this->pHIDHandle, (const unsigned char*) ( pcData ), szLen + 1 );
    free((void*)pcData);
#else
    int iResult = hid_write(this->pHIDHandle, (const unsigned char*)(ptr), szLen );
#endif
    

    if ( iResult < 0 ) {
        throw HIDException ("HID Write : error writing data to device.") ;
        return false;
    }
    return true;
}

bool HIDDevice::ReadDevice ( uint8_t* ptr, size_t szLen ) {

    if ( !bOpen )
        return false;

    if ( !this->pHIDHandle )
        return false;

    int iResult = 0;

    while ( ( iResult = hid_read ( this->pHIDHandle, (unsigned char*) (ptr), szLen ) ) == 0) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
//            usleep ( 10 );
    }

    if ( iResult < 0 ) {
        throw HIDException ("HID Read : error reading data from device.");
        return false;
    }
    return true;
}

