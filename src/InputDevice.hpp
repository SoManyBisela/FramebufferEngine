#ifndef __INPUT_DEVICE_HPP__
#define __INPUT_DEVICE_HPP__

#include <linux/input.h>
#include <vector>

class InputDevice{
private:
    int fd;
    bool closeOnDestroy;
public:
    InputDevice(int, bool = true);
    InputDevice(const InputDevice &); //copy constructor
    InputDevice(InputDevice &&); //move constructor
    ~InputDevice();
    std::vector<input_event> readInputEvents();
};

#endif