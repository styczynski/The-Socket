# The-Socket
C/C++ Socket Library

This library provides an easy-to-use socket interface.
It's designed for Windows usage, but it can be easily ported to Linux too.
Good luck and have fun!

## How to use
Just include `net.h` to your C/C++ project.

## Example
The library provides the well-described simple socket-server chatting appilication.

How to build the provided example:

 * Run `make all`
 * And then run the `./examples/bin/demo.exe -ip IP_ADRESS -port PORT_ADRESS -host` to set up an example socket server and `./examples/bin/demo.exe -ip IP_ADRESS -port PORT_ADRESS -as USERNAME` to run an example socket client.
 * You can alternatively use `./tools/start_test_server.bat` etc.
 * If you're not using windows you can compile demo.cpp with your favourite compiler :)
 * Have fun with the sockets!

## Easy server setup
Now the server setup is super easy!

```c

server sv; //Server instance

sv.atIP(ipv4(ip)) //Set up server IP
.atPort(port) //Set up server port
.retryWhenFailed() //Always restart server on failure
.onRun(def_server_generic_handler({ //A new thread will be created of this handler for each connected client.
    // Hell yeah!
    // It's pretty easy.
});

```

The client set up is so easy too! :)
