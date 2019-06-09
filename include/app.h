#ifndef app_h
#define app_h

#include <vector>
#include <thread>
#include <iostream>
#include "input_msg.h"

#include <ctime>
#include <sys/time.h>
#include <unistd.h>

class app {
private:
    std::vector<input_msg> inbox_;
    
    std::thread input_thread_;
    
    struct timeval tm;
    
    time_t start;
    time_t end;
    
    void do_thread_thing() {
        std::cout << "yeah im in thread" << std::endl;

        time_t last = time(NULL);
        
        
        while (1) {
            time_t now = time(NULL);
            if (1) {
                usleep(1000);
                // do some
                std::cout << "send" << std::endl;
                
                gettimeofday(&tm, NULL);
                start = (tm.tv_sec * 1000) + (tm.tv_usec / 1000);
                
                inbox_.push_back(input_msg());
                
                last = now;
            }
        }
    }
    
public:
    app(): input_thread_(std::thread([&] { do_thread_thing(); })), inbox_(std::vector<input_msg>()) {
        while (1) {
            if (inbox_.size()) {
                // message came.
                std::cout << "receive" << std::endl;
                
                gettimeofday(&tm, NULL);
                end = (tm.tv_sec * 1000) + (tm.tv_usec / 1000);

                printf("elapsed: %ld\n", end - start);
                
                inbox_.pop_back();
            }
        }
    }
    ~app() {
        
    }
    
    std::thread& input_thread() { return input_thread_; }
    
};











#endif /* app_h */
