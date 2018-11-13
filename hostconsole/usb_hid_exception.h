#ifndef __USB_HID_EXCEPTION_H__
#define __USB_HID_EXCEPTION_H__

#include <string>

class HIDException {
    public:
        explicit HIDException ( std::string s )
            : m_s ( s )
        {
    
        }
    
        virtual ~HIDException ( void ) {
    
        }
    
        std::string description() const {
            return m_s;
        }
    
    private:
        std::string m_s;
    
};


#endif // __USB_HID_EXCEPTION_H__