#ifndef __CLOSING_SIGNAL_HANDLER_HPP__
#define __CLOSING_SIGNAL_HANDLER_HPP__
#include <cstdint>
#include <cstdlib>
#include <signal.h>
#include <list>
#include "Closeable.hpp"

#define SEG_FAULT 0
#define CTRL_C 1
typedef bool (*closingBehaviour)(int);

class ClosingSignalHandler{ 
private:

	std::list<Closeable*> toClose;										//the list of Closeable objects to close in case of interruption
	closingBehaviour onClose;											//what to do when closing
	static ClosingSignalHandler *instance;								//singleton instance

	ClosingSignalHandler();

	/*
	*	this method calls all the finalize methods of the Closeable instances in the toClose list,
	*	executes the onClose function if defined, or else it ends the program
	*/
	void sigHandler(int signum);

	//required to have a function pointer to assing as the handler
	static void staticSigHandler(int signum);

public:
	//used to execute the onClose of the closeables
	void triggerClosingBehaviours();

	//used to get the instance of the ClosingSignalHandler object
	static ClosingSignalHandler* getInstance();

	//sets the closing behaviour
	void setClosingBehaviour(closingBehaviour newClosingBehaviour);

	//adds a Closeable to the list to be closed
	void addCloseable(Closeable* f);

	//removes a Closeable from the list
	void removeCloseable(Closeable* f);

};
#endif