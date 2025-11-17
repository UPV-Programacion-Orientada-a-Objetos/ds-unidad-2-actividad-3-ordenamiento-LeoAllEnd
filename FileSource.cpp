#include "FileSource.h"

FileSource::FileSource(const char* filename) {
    file = fopen(filename, "r");
    hasData = (file != NULL);
    
    if (hasData) {
        readNextValue();
    }
}

FileSource::~FileSource() {
    if (file != NULL) {
        fclose(file);
    }
}

void FileSource::readNextValue() {
    if (file != NULL) {
        if (fscanf(file, "%d", &nextValue) == 1) {
            hasData = true;
        } else {
            hasData = false;
        }
    } else {
        hasData = false;
    }
}

int FileSource::getNext() {
    int value = nextValue;
    readNextValue();
    return value;
}

bool FileSource::hasMoreData() {
    return hasData;
}