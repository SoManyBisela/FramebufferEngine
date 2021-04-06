#include "InputDevice.hpp"
#include <unistd.h>
#include <errno.h>
#include "InputReadingError.hpp"

InputDevice::InputDevice(int fd, bool deleteOnClose):
    fd(fd),
    closeOnDestroy(deleteOnClose){
}


InputDevice::InputDevice(const InputDevice & other):
    fd(other.fd),
    closeOnDestroy(false){
}

InputDevice::InputDevice(InputDevice &&other):
    fd(other.fd),
    closeOnDestroy(other.closeOnDestroy){
    other.fd = -1;
    other.closeOnDestroy = false;
}

InputDevice::~InputDevice(){
    /*if(closeOnDestroy){
        close(fd);
    }*/
}

std::vector<input_event> InputDevice::readInputEvents(){
    std::vector<input_event> events;
    input_event ev;
    if(fd != -1){
        int bytesRead = read(fd, &ev, sizeof(input_event));
        while(bytesRead) {
            if(bytesRead == -1){
                if(errno == EBADF){
                    throw InputReadingError("Bad file descriptor in device");
                }else if(errno = EAGAIN){
                    break;
                }else{
                    throw InputReadingError(std::string("Error while reading device. errno: ")+std::to_string(errno));
                }
            }
            events.push_back(ev);
            bytesRead = read(fd, &ev, sizeof(input_event));
        }
    }
    return std::move(events);
}