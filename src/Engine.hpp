#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include "FramebufferWriter.hpp"
#include "InputDeviceListener.hpp"

class Engine : public FramebufferWriter, protected InputDeviceListener{
    private:
        bool shouldClose;
        bool updateGraphics;
    protected:
        virtual void draw(float elapsedTime) = 0;
        void close(); //sets the closing flag
        void pause(); //processing noLoop();
        void play();  //processing loop();
        virtual void loadDevices();

    public:
        Engine();
        ~Engine();
        void start();
};

#endif