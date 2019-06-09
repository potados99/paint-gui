#ifndef app_h
#define app_h

#include <vector>
#include <thread>
#include <iostream>
#include "input_msg.h"
#include "touch.h"

#include <ctime>
#include <sys/time.h>
#include <unistd.h>

class app {
private:
    std::vector<input_msg>  inbox_;
    std::thread             input_thread_;
    
    touch                   touch_;

    
public:
    app(const char *touch_event_path):
    input_thread_(std::thread([&] { start_input_loop(); })),
    inbox_(std::vector<input_msg>()),
    touch_(touch(touch_event_path)) {}
    
    ~app() {
        
    }
    
    void start_main_loop();
    void start_input_loop();
    
    std::thread& input_thread() { return input_thread_; }
};











#endif /* app_h */
