#include "DNSServer.h"

bool DNSServer::start(uint16_t port, const char *domainName, IPAddress resolvedIP)
{
    _port = port;
    _resolvedIP = resolvedIP;
    if (!_udp.begin(_port))
        return false;
    _started = true;
    return true;
}

void DNSServer::processNextRequest()
{
    if (!_started)
        return;
    int packetSize = _udp.parsePacket();
    if (packetSize)
    {
        uint8_t buffer[512];
        _udp.read(buffer, 512);
        // DNS header is 12 bytes, question follows
        // Always reply with our IP for any query
        buffer[2] |= 0x80; // QR = response
        buffer[3] |= 0x80; // RA = 1
        buffer[7] = 1;     // QDCOUNT = 1
        buffer[9] = 1;     // ANCOUNT = 1
        // Write answer after question
        int qlen = packetSize - 12;
        int ansStart = packetSize;
        memcpy(buffer + ansStart, buffer + 12, qlen); // Name
        int idx = ansStart + qlen;
        buffer[idx++] = 0x00; // Type A
        buffer[idx++] = 0x01;
        buffer[idx++] = 0x00; // Class IN
        buffer[idx++] = 0x01;
        buffer[idx++] = 0x00;
        buffer[idx++] = 0x00;
        buffer[idx++] = 0x00;
        buffer[idx++] = 0x3C; // TTL
        buffer[idx++] = 0x00;
        buffer[idx++] = 0x04; // RDLENGTH
        buffer[idx++] = _resolvedIP[0];
        buffer[idx++] = _resolvedIP[1];
        buffer[idx++] = _resolvedIP[2];
        buffer[idx++] = _resolvedIP[3];
        _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
        _udp.write(buffer, idx);
        _udp.endPacket();
    }
}

void DNSServer::stop()
{
    _udp.stop();
    _started = false;
}
