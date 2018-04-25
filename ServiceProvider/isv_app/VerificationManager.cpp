
#include "VerificationManager.h"
#include "GeneralSettings.h"

#include  <iomanip>

using namespace util;
using namespace std;


VerificationManager *VerificationManager::instance = NULL;


VerificationManager::VerificationManager()
{
    this->nm = NetworkManagerServer::getInstance(Settings::rh_port);
    this->ws = WebService::getInstance();
    this->ws->init();
}


VerificationManager::~VerificationManager()
{ }


VerificationManager *VerificationManager::getInstance()
{
    if (instance == NULL)
    {
        instance = new VerificationManager();
    }

    return instance;
}


void VerificationManager::sessionCloseHandler(Session *pSession)
{
    delete serviceProviders[pSession];
    serviceProviders.erase(pSession);
}


void VerificationManager::sessionStartHandler(Session *pSession)
{
    ServiceProvider *sp = new ServiceProvider(this->nm, this->ws);
    pSession->setCallbackHandler([sp](string v, int type) {
        return sp->incomingHandler(v, type);
    });
    pSession->setCallbackSessionClose([this](AbstractNetworkOps *pSession) {
        return this->sessionCloseHandler((Session *)pSession);
    });
    serviceProviders[pSession] = sp;
}


int VerificationManager::init()
{
    serviceProviders.clear();
    this->nm->Init();
    this->nm->setCallbackSessionStart([this](Session *pSession) {
        return this->sessionStartHandler(pSession);
    });
}


void VerificationManager::start()
{
    this->nm->startService();
}

