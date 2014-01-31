#include "time.h"

#include <unistd.h>

main()
{
    Time start = Time::now();
    usleep(1500000);
    Time end = Time::now();

    std::cout<<"From "<<start<<" to "<<end<<" elapsed "<<end-start<<std::endl;
}
