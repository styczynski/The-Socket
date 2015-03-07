
#ifndef DEV_PLATFORM_UNIX
  #ifndef DEV_PLATFORM_WINDOWS
    #warning "There's no present headers configuration defines."
    #warning "Please define DEV_PLATFORM_UNIX or DEV_PLATFORM_WINDOWS."
    #warning "The default - Windows headers will be used."
    #warning "This will probably cause compilation errors!"
    #define DEV_PLATFORM_WINDOWS
  #endif
#endif

#ifdef DEV_PLATFORM_WINDOWS
  #pragma message "Using WINDOWS headers configuration."
#endif
#ifdef DEV_PLATFORM_UNIX
  #pragma message "Using UNIX headers configuration."
#endif

#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <initializer_list>
#include <deque>
#include <string>

#ifdef DEV_PLATFORM_UNIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <signal.h>
  #include <process.h>
  #include <pthread.h>
#endif
#ifdef DEV_PLATFORM_WINDOWS
  #include <winsock.h>
  #include <signal.h>
  #include <process.h>
  #include <pthread.h>
#endif
