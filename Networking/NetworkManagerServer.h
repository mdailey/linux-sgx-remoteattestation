#include "NetworkManager.h"

class NetworkManagerServer : public NetworkManager {

public:
    static NetworkManagerServer* getInstance(int port);
    void Init();
    void startService();
    void setCallbackSessionStart(CallbackSessionStart css);

private:
    NetworkManagerServer();

private:
    static NetworkManagerServer* instance;
    Server *server = NULL;

};



