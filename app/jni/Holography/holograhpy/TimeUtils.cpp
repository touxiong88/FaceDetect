#include "fflog.h"
#include <stdint.h>
#include <sys/time.h>
#include <errno.h>
#include <chrono>

/*
static std::chrono::steady_clock::time_point  basesec;
long getTimeMs() {

    std::chrono::steady_clock::time_point t1 = std::chrono::system_clock::now();
    if (basesec.count() == 0) {
        basesec =t1;
    }


	
    return  (t1 - basesec).count();
}
*/


static std::time_t  basesec;
std::time_t getTimeStamp()  
{  
    std::chrono::time_point<std::chrono::system_clock,std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());  
    auto tmp=std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());  
    std::time_t timestamp = tmp.count();  

    if (basesec == 0) {
        basesec =timestamp;
    }
	
    //std::time_t timestamp = std::chrono::system_clock::to_time_t(tp);  
    return timestamp-basesec;  
}  

