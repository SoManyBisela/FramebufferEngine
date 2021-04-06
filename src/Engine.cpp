#include "Engine.hpp"
#include <string>
#include <functional>
#include <stdexcept>
#include <fcntl.h>
#include <iostream>

//used for device loading
static std::string exec(const char* cmd) {
    char buffer[1024];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("Command execution failed.");
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



void Engine::loadDevices(){
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
}

Engine::Engine():
    shouldClose(false),
    updateGraphics(true),
    FramebufferWriter(),
    InputDeviceListener(getWidth(), getHeight()){
        loadDevices();
};

Engine::~Engine(){
    
}

void Engine::start(){
    while(!shouldClose){
        updateState();
        if(updateGraphics)
            draw();
        applyBuffer();
    }
}


void Engine::pause(){
    updateGraphics = false;
}

void Engine::play(){
    updateGraphics = true;
}

void Engine::close(){
    shouldClose = true;
}