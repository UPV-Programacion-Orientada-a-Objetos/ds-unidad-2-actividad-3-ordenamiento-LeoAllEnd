#include "SerialSource.h"
#include <cstdlib>
#include <cstring>

SerialSource::SerialSource(const char* portName) {
    connected = false;
    bufferPos = 0;
    
    #ifdef _WIN32
        hSerial = CreateFileA(portName,
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL,
                             NULL);
        
        if (hSerial == INVALID_HANDLE_VALUE) {
            return;
        }
        
        DCB dcbSerialParams = {0};
        dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
        
        if (!GetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            return;
        }
        
        dcbSerialParams.BaudRate = CBR_9600;
        dcbSerialParams.ByteSize = 8;
        dcbSerialParams.StopBits = ONESTOPBIT;
        dcbSerialParams.Parity = NOPARITY;
        
        if (!SetCommState(hSerial, &dcbSerialParams)) {
            CloseHandle(hSerial);
            return;
        }
        
        COMMTIMEOUTS timeouts = {0};
        timeouts.ReadIntervalTimeout = 50;
        timeouts.ReadTotalTimeoutConstant = 50;
        timeouts.ReadTotalTimeoutMultiplier = 10;
        
        if (!SetCommTimeouts(hSerial, &timeouts)) {
            CloseHandle(hSerial);
            return;
        }
        
        connected = true;
    #else
        serialPort = open(portName, O_RDWR | O_NOCTTY);
        
        if (serialPort < 0) {
            return;
        }
        
        struct termios tty;
        memset(&tty, 0, sizeof(tty));
        
        if (tcgetattr(serialPort, &tty) != 0) {
            close(serialPort);
            return;
        }
        
        cfsetospeed(&tty, B9600);
        cfsetispeed(&tty, B9600);
        
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CSIZE;
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~CRTSCTS;
        tty.c_cflag |= CREAD | CLOCAL;
        
        tty.c_lflag &= ~ICANON;
        tty.c_lflag &= ~ECHO;
        tty.c_lflag &= ~ECHOE;
        tty.c_lflag &= ~ECHONL;
        tty.c_lflag &= ~ISIG;
        
        tty.c_iflag &= ~(IXON | IXOFF | IXANY);
        tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
        
        tty.c_oflag &= ~OPOST;
        tty.c_oflag &= ~ONLCR;
        
        tty.c_cc[VTIME] = 1;
        tty.c_cc[VMIN] = 0;
        
        if (tcsetattr(serialPort, TCSANOW, &tty) != 0) {
            close(serialPort);
            return;
        }
        
        connected = true;
    #endif
}

SerialSource::~SerialSource() {
    if (connected) {
        #ifdef _WIN32
            CloseHandle(hSerial);
        #else
            close(serialPort);
        #endif
    }
}

bool SerialSource::readLine(char* line, int maxLen) {
    int linePos = 0;
    
    while (linePos < maxLen - 1) {
        char c;
        
        #ifdef _WIN32
            DWORD bytesRead;
            if (!ReadFile(hSerial, &c, 1, &bytesRead, NULL)) {
                return false;
            }
            if (bytesRead == 0) {
                continue;
            }
        #else
            int n = read(serialPort, &c, 1);
            if (n <= 0) {
                continue;
            }
        #endif
        
        if (c == '\n' || c == '\r') {
            if (linePos > 0) {
                line[linePos] = '\0';
                return true;
            }
        } else {
            line[linePos++] = c;
        }
    }
    
    line[linePos] = '\0';
    return linePos > 0;
}

int SerialSource::getNext() {
    char line[256];
    
    if (readLine(line, 256)) {
        return atoi(line);
    }
    
    return -1;
}

bool SerialSource::hasMoreData() {
    return connected;
}

bool SerialSource::isConnected() {
    return connected;
}