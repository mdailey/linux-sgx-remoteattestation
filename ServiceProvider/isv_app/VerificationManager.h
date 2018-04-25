#ifndef VERIFICATIONMANAGER_H
#define VERIFICATIONMANAGER_H

#include <string>
#include <map>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

#include "ServiceProvider.h"
#include "NetworkManagerServer.h"
#include "LogBase.h"
#include "Messages.pb.h"
#include "WebService.h"

using namespace std;

class VerificationManager {

public:
    static VerificationManager* getInstance();
    virtual ~VerificationManager();
    int init();
    void start();

private:
    VerificationManager();
    void sessionStartHandler(Session *pSession);
    void sessionCloseHandler(Session *pSession);

private:
    static VerificationManager* instance;
    NetworkManagerServer *nm = NULL;
    WebService *ws = NULL;
    map<Session *, ServiceProvider *> serviceProviders;
};

#endif










