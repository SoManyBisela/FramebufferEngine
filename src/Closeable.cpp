#include "Closeable.hpp"
#include "ClosingSignalHandler.hpp"
Closeable::Closeable(){
	ClosingSignalHandler::getInstance()->addCloseable(this);
}

Closeable::~Closeable(){
    ClosingSignalHandler::getInstance()->removeCloseable(this);
}