/*
*   File to encapsulate all AWS IoT Core required secrets 
*   Each sensor will required a unique file like this
*
*   Todo: Pull out the cert information such that boilerplate 
*   duplication is avoided
*
*   Author: Josh Perry <josh.perry245@gmail.com>
*   Created: 15 April 2019
*/

#ifndef AWSIOTSECRETS
#define AWSIOTSECRETS

#include "../src/config/Configuration.h"

static const char SENSOR_UID[] =

// Obtain First CA certificate for Amazon AWS - Root CA
static const char caCert[] = R"EOF(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)EOF";

// Copy contents from XXXXXXXX-certificate.pem.crt here ▼
static const char clientCert[] = R"KEY(
-----BEGIN CERTIFICATE-----

-----END CERTIFICATE-----
)KEY";

// Copy contents from  XXXXXXXX-private.pem.key here ▼
static const char privateKey[] = R"KEY(
-----BEGIN RSA PRIVATE KEY-----

-----END RSA PRIVATE KEY-----
)KEY";

#endif