#ifndef DNSSERVER_H
#define DNSSERVER_H
#include <WiFi.h>
#include <WiFiUdp.h>

class DNSServer
{
public:
    bool start(uint16_t port, const char *domainName, IPAddress resolvedIP);
    void processNextRequest();
    void stop();

private:
    WiFiUDP _udp;
    uint16_t _port;
    IPAddress _resolvedIP;
    bool _started = false;
};

#endif // DNSSERVER_H
