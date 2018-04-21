#ifndef GENERALSETTINGS_H
#define GENERALSETTINGS_H

#include <string>

using namespace std;

namespace Settings {
	static int rh_port = 22222;
	static string rh_host = "localhost";

        // 0 for unlinkable, 1 for linkable. Should match setting sent
        // to Intel when registering for IAS
        static int quote_type = 0;
	
        // Certificate for the HTTPS connection between the SP and the App
	static string server_crt = "server.crt";

        // Private key for the HTTPS connection
	static string server_key = "server.key";

        // SPID provided by Intel after registration for the IAS service
	static string spid = "MY-SPID";

        // Location of the certificate sent to Intel when registering
        // for IAS. Should contain certificate and key, in PEM format
	static const char *ias_crt = "client_dev.pem";

	static string ias_url = "https://test-as.sgx.trustedservices.intel.com:443/attestation/sgx/v2/";
}

#endif
