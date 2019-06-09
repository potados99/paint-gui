
#ifndef object_h
#define object_h

#include <iostream>
#include <string>

class object {
private:
    static unsigned long object_count;
    
protected:
    unsigned long count_;

public:
    object(): count_(++object_count) {}
    virtual std::string to_string() { return "object count " + std::to_string(count_); }
};


#endif /* object_h */
