/*
	The-Socket
		C/C++ Socket Handling Library

		Version 1.0v
		---== Example file ==---

		This is an example of the The-Socket C/C++ library usage.
		How to use:
			Run this application with the following flags:
				[APPNAME] -ip IP_ADRESS -port PORT_ADRESS <-as USERNAME> <-host>

			For server:
				[APPNAME] -ip IP_ADRESS -port PORT_ADRESS -host

			And for the client:
				[APPNAME] -ip IP_ADRESS -port PORT_ADRESS -as USERNAME

*/
#include "net.h"

using namespace net;

bool host = false; //Is a server?
string ip = ""; //IP of client/server
int port = 100; //Port number of client/server
string login = "anomymous"; //Client login name

struct user { //Define user object
	string nick = "NULL"; //Login of the user
};

inline void runServer() { //Runs the server using global ip/port variables
	server sv; //Server instance

	sv.atIP(ipv4(ip)) //Set up server IP
	.atPort(port) //Set up server port
	.retryWhenFailed() //Always restart server on failure
	.applySettings(server_setup({ //Server settings
		autoPing: false, //Automatically sends ping requests
		autoSessionCleanup: false //If the ping is not received anymore, disconnect client instantly
	}))
	.onRun(def_server_generic_handler({ //A new thread will be created of this handler for each connected client.

		user usr; //User that has connected the server
		net_session* session = server->installClient<user>(target, usr); //Store user data.
		usr.nick = target.recv().getData(); //Receive string - user nick

		cout<<usr.nick<<"'s joined the server.\n"; //Print user nick to the screen
		cout.flush();
		def_server_loop({
			Sleep(80);
			auto res = target.recv(); //Receive incoming data
			string result = res.getData(); //Obtain received data as string.
			if(result!="") {
				net_message(string("")+"["+usr.nick+":"+session->getID()+"] "+result) //Create a string message.
					.sendTo(server, server->getConnectedClients()); //And send it to all connected clients.

				cout<<(string("")+"["+usr.nick+":"+session->getID()+"] "+result)<<"\n"; //Print received message to the client screen.
				cout.flush();
			}
		});
	}))
	.onInit(def_server_generic_handler({ //Server init-handler
		cout<<"Server inited!\n";
		cout.flush();
	}))
	.onError(def_server_error_handler({ //Server error-handler
		//arg0 - error text
		//arg1 - exit code
		cout<<"[ERRR] < "<<arg0<<" > with exit code {"<<arg1<<"}\n";
		cout.flush();
	})).open(); //The open function locks the current thread

	//This part is locked until the server is running normally.

	rest();

}

inline void runClient() { //Runs the client using global ip/port variables
	bool client_ready = false;

	client cl; //Instance of the client

	cl.atIP(ipv4(ip)) //Set up its ip (just like in case of the server)
	.atPort(port) //And now the port.
	.retryWhenFailed() //Always retry when failed
	.onRun(def_client_generic_handler_ex([&],{ //Run handler...
		target.send(login); //Send login data
		Sleep(250); //Wait a second...
		client_ready = true;
		while(true) {
			string str = "";
			str = target.recv().getData(); //Get the data from server
			if(str!="") {
				cout<<str<<"\n"; //Print the received data to the screen!
				cout.flush();
			}
		}
	}))
	.onInit(def_client_generic_handler_ex([&],{ //Just like in case of the server
		cout<<"Client inited!\n";
		cout.flush();
	})).onError(def_client_error_handler({ //Yeah, the error handler!
		cout<<"[ERRR] < "<<arg0<<" > with exit code {"<<arg1<<"}\n";
		cout.flush();
	})).opent(); //Opent() does not lock the current thread

	//This part is executed as a normal code flow

	Sleep(100);
	while(!client_ready) Sleep(100); //Wait until client is ready (if you don't know what is this strnage
																	 // client_ready variable - see begining of the run handler of this client)
	cout<<"Ready.\n"; //Yeah, our client is ready!
	cout.flush();

	while(true) {
		string str = "";
		getline(cin, str); //Read input data and...
		cl.getConnection()->send(str); //...send it to the server!
	}
}

int main(int argc, char* argv[]) {

	string temp = "";
	for(int it=1;it<argc;++it) {
		temp = string(argv[it]);
		if(temp=="-host") { //Host flags means i'm a server!
			host=true;
		} else if(temp=="-ip") { //Give me ip adress!
			++it;
			ip = string(string(argv[it]));
		} else if(temp=="-port") { //Give me the port number!
			++it;
			port = strToInt(string(argv[it]));
		} else if(temp=="-user" || temp=="-login" || temp=="-as") { //And maybe a login? - Yeah, but you don't have to...
			++it;
			login = string(argv[it]);
		}
	}

	if(host) {
		runServer(); //Run server
	} else {
		runClient(); //Or run the client!
	}

	return 0;
}
