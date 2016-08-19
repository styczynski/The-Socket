[![Made by Styczynsky Digital Systems][badge sts]][link isis97]

[![Platform - Windows][badge support windows]][link isis97]

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



[badge support windows]: https://img.shields.io/badge/platform-windows-blue.svg?style=flat-square&logoWidth=20&logo=data%3Aimage%2Fpng%3Bbase64%2CiVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAAABmJLR0QA%2FwD%2FAP%2BgvaeTAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAB3RJTUUH4AgSEisSipueyAAAAHBJREFUSMdjZKA2WPv%2BPzKXkSxDiuf%2FZ7AKIEopbgsW3v%2FPwCOA4AcLMqK7jhjAQo4mUgATA43BqAWjFlADiCvQ1HjsuXNJIwPD%2BgmMtLMAGyCzqBhNRaMWDAELWBiCBRmJrcDJy2hUaj1Q3wIiLQcAUjQgoD1kMJYAAAAASUVORK5CYII%3D

[badge sts]: https://img.shields.io/badge/-styczynsky_digital_systems-blue.svg?style=flat-square&logoWidth=20&logo=data%3Aimage%2Fpng%3Bbase64%2CiVBORw0KGgoAAAANSUhEUgAAABYAAAAXCAYAAAAP6L%2BeAAAABmJLR0QA%2FwD%2FAP%2BgvaeTAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAB3RJTUUH4AgSEh0nVTTLngAAAB1pVFh0Q29tbWVudAAAAAAAQ3JlYXRlZCB3aXRoIEdJTVBkLmUHAAAAm0lEQVQ4y2Pc%2Bkz2PwMNAAs2wVMzk4jSbJY%2BD6ccEwONACMsKIh1JSEgbXKeQdr4PO1cPPQMZiGkoC7bkCQD7%2Fx7znDn35AOClK9PEJSBbNYAJz999UGrOLocsM0KHB5EZ%2FXPxiVMDAwMDD8SP3DwJA6kFka5hJCQOBcDwMDAwPDm3%2FbGBj%2BbR8tNrFUTbiAB8tknHI7%2FuTilAMA9aAwA8miDpgAAAAASUVORK5CYII%3D

[link isis97]: https://github.com/isis97/
