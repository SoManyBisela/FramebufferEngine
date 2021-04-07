#include "src/Engine.hpp"
#include "src/ClosingSignalHandler.hpp"
#include <iostream>


class EngineTest : public Engine{
    protected:
        virtual void draw(float elapsedTime){
            MouseCoordinates m = getMousePosition();
            rect(m.x, m.y, 20, 20);
        }
        virtual void onKeyDown(int key){
            std::cout << "Key pressed: " << key << std::endl;
            if(key == KEY_ESC){
                close();
            }
        }
        virtual void onKeyUp(int key){}
        virtual void onKeyHeld(int key){}
        virtual void onMouseWheelScroll(int amount){}
    public:
        EngineTest(){
            color(255, 0,0);
        }
};

int main(){
        EngineTest test;
        test.start();
    return 0;
}