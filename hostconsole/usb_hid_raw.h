

#ifndef USB_DRV_H
#define USB_DRV_H

#include <inttypes.h>
#include <cstdlib>
#include <cstring>
#include "usb_hid_exception.h"

#define VID 0x03eb // Atmel
#define PID 0x0002 // beliebig

class HIDDevice {
    public:
        explicit HIDDevice ( uint16_t nwVID = VID, uint16_t nwPID = PID  );
                 //HIDDevice ( void );
        virtual ~HIDDevice ( void );

        bool OpenDevice ( uint16_t nwVID, uint16_t nwPID );
        bool OpenDevice ( void );

        void CloseDevice ( void );
        bool WriteDevice ( const uint8_t* ptr, size_t szLen );
        bool ReadDevice  ( uint8_t* ptr, size_t szLen );

        bool isOpen      ( void ) const {
            return bOpen;
        }

    private:
        bool         bOpen;
        hid_device  *pHIDHandle;
        uint16_t     nwVendorID;
        uint16_t     nwProductID;
};

#endif // USB_DRV_H
