#ifndef DATASOURCE_H
#define DATASOURCE_H

class DataSource {
public:
    virtual int getNext() = 0;
    
    virtual bool hasMoreData() = 0;
    
    virtual ~DataSource() {}
};

#endif