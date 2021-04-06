#ifndef __INPUTDEVICELISTENER_HPP__
#define __INPUTDEVICELISTENER_HPP__

#include <vector>
#include <linux/input-event-codes.h>
#include "InputDevice.hpp"

struct MouseCoordinates
{
    int x,
        y;
};


class InputDeviceListener{
    private:
        struct
        {
            int x,
                y;
        }  mouseBounds;
        bool mouseBoundsEnabled;
        std::vector<InputDevice> devices;
        unsigned char keyboard[KEY_CNT] = {0};
        MouseCoordinates mousePosition;
    protected:

        /*
        *
        */
        void addDevice(InputDevice&& device);

        /*
        *
        */
        virtual void onKeyDown(int key) {};

        /*
        *
        */
        virtual void onKeyUp(int key) {};

        /*
        *
        */
        virtual void onKeyHeld(int key) {};

        /*
        *
        */
        virtual void onMouseWheelScroll(int amount) {};
    public:
        /*
        *
        */
        void setMouseBounds(int x, int y);

        /*
        *
        */
       void enableMouseBounds();

        /*
        *
        */
       void disableMouseBounds();

        /*
        *
        */
        void updateState();

        /*
        *
        */
        bool isKeyPressed(int key);

        /*
        *
        */
        const MouseCoordinates& getMousePosition();

        /*
        *
        */
        InputDeviceListener(int mouseBoundX, int mouseBoundY);

        /*
        *
        */
        InputDeviceListener();

        /*
        *
        */
        ~InputDeviceListener();
};




#endif