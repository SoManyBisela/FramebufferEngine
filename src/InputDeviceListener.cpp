#include "InputDeviceListener.hpp"

#include <string>
#include <functional>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "InputReadingError.hpp"

#define KEY_PRESSED 1
#define KEY_RELEASED 0


void InputDeviceListener::addDevice(InputDevice&& device){
    devices.push_back(device);
}


void InputDeviceListener::updateState(){
    int wheelRel = 0;
    for(InputDevice d : devices){
        std::vector<input_event> events(d.readInputEvents());
        for(auto ev : events){
            switch (ev.type){
                case EV_REL:
                    if(ev.code == REL_X){
                        mousePosition.x += ev.value;
                        if(mouseBoundsEnabled){
                            if(mousePosition.x < 0)
                                mousePosition.x = 0;
                            else if(mousePosition.x > mouseBounds.x)
                                mousePosition.x = mouseBounds.x;
                        }
                    }else if(ev.code == REL_Y){
                        mousePosition.y += ev.value;
                        if(mouseBoundsEnabled){
                            if(mousePosition.y < 0)
                                mousePosition.y = 0;
                            else if(mousePosition.y > mouseBounds.y)
                                mousePosition.y = mouseBounds.y;
                        }
                    }else if(ev.code == REL_WHEEL){
                        wheelRel += ev.value;
                    }
                    break;
                case EV_KEY:
                    keyboard[ev.code] = ev.value;
                    if(ev.value == KEY_PRESSED){
                        onKeyDown(ev.code);
                    }else if(ev.value == KEY_RELEASED){
                        onKeyUp(ev.code);
                    }else if(ev.value == 2){
                        onKeyHeld(ev.code);
                    }/*else{
                        throw new std::runtime_error("unknown value on key event: " + std::to_string(ev.value)); //for dev purpose
                    }*/
                    break;
            }
        }
    }
    if(wheelRel)
        onMouseWheelScroll(wheelRel);
}

bool InputDeviceListener::isKeyPressed(int key){
    return keyboard[key] > 0;
}


const MouseCoordinates& InputDeviceListener::getMousePosition(){
    return mousePosition;
}

void InputDeviceListener::setMouseBounds(int x, int y){
    mouseBounds = {x,y};
    enableMouseBounds();
}

void InputDeviceListener::enableMouseBounds(){
    mouseBoundsEnabled = true;
}

void InputDeviceListener::disableMouseBounds(){
    mouseBoundsEnabled = false;
}

InputDeviceListener::InputDeviceListener(int mouseBoundX, int mouseBoundY):
    mousePosition({mouseBoundX/2, mouseBoundY/2}){
    setMouseBounds(mouseBoundX, mouseBoundY); //sets mouse bounds and enables mouse bounds
}

InputDeviceListener::InputDeviceListener(){
    disableMouseBounds();
}

InputDeviceListener::~InputDeviceListener(){
}
/*
//used for device loading
static std::string exec(const char* cmd) {
    char buffer[1024];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw InputReadingError("Command execution failed.");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

//used for device loading
static void execOnSplit(const std::string &target, const std::string &delimiter, const std::function<void(const std::string&)> callback){

    auto start = 0U;
    auto end = target.find(delimiter);
    while (end != std::string::npos)
    {
        callback(target.substr(start, end - start));
        start = end + delimiter.length();
        end = target.find(delimiter, start);
    }

    if(target.substr(start, end).length() > 0)
        callback(target.substr(start, end));
}

void InputDeviceListener::loadDevices(){
    std::string result = exec("ls -lRa /dev/input/by-path/ | grep -E \"(event-kbd|event-mouse)\" | awk '{value = $NF; num = split(value, arr, \"/\"); printf \"/dev/input/%s\\n\", arr[num]}'");
    std::vector<int> fds;
    auto callback = [&fds](std::string in){fds.push_back(open(in.c_str(), O_RDONLY | O_NONBLOCK));};
    execOnSplit(result, "\n", callback);
    for (auto fd : fds){
        if( fd != -1 ){
            InputDevice device(fd);
            addDevice(std::move(device));
        }
    }
}*/

