#ifndef FILESOURCE_H
#define FILESOURCE_H

#include "DataSource.h"
#include <cstdio>

class FileSource : public DataSource {
private:
    FILE* file;
    bool hasData;
    int nextValue;
    
    void readNextValue();
    
public:
    FileSource(const char* filename);
    ~FileSource();
    
    int getNext();
    bool hasMoreData();
};

#endif