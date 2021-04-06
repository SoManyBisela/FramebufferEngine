#include "ClosingSignalHandler.hpp"

ClosingSignalHandler::ClosingSignalHandler(){
    /*
     *by default nothing happens other than closing all Closeables
     */
    onClose = NULL;

    /*
    *	sets the handlers for the SIGINT(ctrl+c) and SIGSEGV(segmentation fault) 
    *	to the behavior defined in sigHandler.
    */
    struct sigaction action;
    action.sa_flags = 0;
    action.sa_handler = ClosingSignalHandler::staticSigHandler;
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGSEGV, &action, NULL);
}

void ClosingSignalHandler::sigHandler(int signum){
    int reason = signum == SIGSEGV ? SEG_FAULT : CTRL_C;
    if(onClose == NULL || onClose(reason)){
        triggerClosingBehaviours();
        exit(1);
    }

}

void ClosingSignalHandler::staticSigHandler(int signum){
    ClosingSignalHandler::instance->sigHandler(signum);
}

void ClosingSignalHandler::triggerClosingBehaviours(){
    std::list<Closeable*>::iterator it;
    
    while(toClose.size() > 0){
        Closeable *closing = *toClose.begin();
        closing->onClose();
        toClose.remove(closing);
    }
    /* vecchia implementazione 
    for(it = toClose.begin(); it != toClose.end(); ++it){
        (*it)->onClose();
    }
    */
}

ClosingSignalHandler* ClosingSignalHandler::getInstance(){
    if(ClosingSignalHandler::instance == NULL){
        ClosingSignalHandler::instance = new ClosingSignalHandler();
    }
    return ClosingSignalHandler::instance;
}

void ClosingSignalHandler::setClosingBehaviour(closingBehaviour newClosingBehaviour){
    onClose = newClosingBehaviour;
}

void ClosingSignalHandler::addCloseable(Closeable* f){
	toClose.push_back(f);
}

void ClosingSignalHandler::removeCloseable(Closeable* f){
	toClose.remove(f);
}

ClosingSignalHandler* ClosingSignalHandler::instance = NULL;