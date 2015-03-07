
#ifndef DEV_PLATFORM_UNIX
  #ifndef DEV_PLATFORM_WINDOWS
    #warning "There's no present headers configuration defines."
    #warning "Please define DEV_PLATFORM_UNIX or DEV_PLATFORM_WINDOWS."
    #warning "The default - Windows headers will be used."
    #warning "This will probably cause compilation errors!"
    #define DEV_PLATFORM_WINDOWS
  #endif
#endif

#ifdef DEV_PLATFORM_UNIX
  #pragma message ( "Using UNIX headers configuration." )
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <conio.h>
  #include <signal.h>
  #include <deque>
  #include <string>
  #include <process.h>
  #include <iostream>
  #include <vector>
  #include <functional>
  #include <pthread.h>
  #include <algorithm>
  #include <initializer_list>
#endif
#ifdef DEV_PLATFORM_WINDOWS
  #pragma message ( "Using WINDOWS headers configuration." )
  #include <winsock.h>
  #include <conio.h>
  #include <signal.h>
  #include <deque>
  #include <string>
  #include <process.h>
  #include <iostream>
  #include <vector>
  #include <functional>
  #include <pthread.h>
  #include <algorithm>
  #include <initializer_list>
#endif
