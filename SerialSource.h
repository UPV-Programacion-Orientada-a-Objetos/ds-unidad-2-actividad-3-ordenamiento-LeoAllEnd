#ifndef SERIALSOURCE_H
#define SERIALSOURCE_H

#include "DataSource.h"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
#endif

class SerialSource : public DataSource {
private:
#ifdef _WIN32
    HANDLE hSerial;
#else
    int serialPort;
#endif

    bool connected;
    char buffer[256];
    int bufferPos;

    bool readLine(char* line, int maxLen);

public:
    SerialSource(const char* portName);
    ~SerialSource();

    int getNext();
    bool hasMoreData();
    bool isConnected();
};

#endif