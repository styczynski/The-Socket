
class client;

enum client_event {
	CLIENT_EVENT_ON_INIT = 100,
	CLIENT_EVENT_ON_EXIT = 101,
	CLIENT_EVENT_ON_RUN = 0
};

typedef function<void(string, int)> client_error_handler;
typedef function<void(client*, client_event, net_connection)> client_generic_handler;

#define def_client_generic_handler(CODE) [](client* client, client_event event, net_connection target)->void CODE
#define def_client_error_handler(CODE) [](string arg0, int arg1)->void CODE
#define def_client_generic_handler_ex(VARS, CODE) VARS(client* client, client_event event, net_connection target)->void CODE
#define def_client_error_handler_ex(VARS, CODE) VARS(string arg0, int arg1)->void CODE
#define as_client_generic_handler(FUNCTNAME) [](client* arg0, client_event arg1, net_connection arg2)->void { FUNTNAME (arg0, arg1, arg2); }
#define as_client_error_handler(FUNCTNAME) [](string arg0, int arg1)->void { FUNCTNAME (arg0, arg1); }
#define default_client_generic_handler def_client_generic_handler({return;})
#define default_client_error_handler def_client_error_handler({return;})

void *client_opent_thread___(void *lpParam);
void *client_closet_thread___(void *lpParam);

class client : public net_stack_receiver {

friend void *client_opent_thread___(void *lpParam);
friend void *client_closet_thread___(void *lpParam);

private:
	client_error_handler error_handler;
	client_generic_handler on_init_handler;
	client_generic_handler on_exit_handler;
	client_generic_handler on_run_handler;

	net_connection* client_connection;
	SOCKET sock;
	WSADATA WSAdata;
	string ip;
	int port;
	bool retryWhenFailed__ = false;

	bool isRunning = false;

	inline void s_cl(string message, int errcode=0) {
		error_handler(message, errcode);
	}

	inline void defaultHandlers() {
		onError(def_client_error_handler({return;}));
		onInit(def_client_generic_handler({return;}));
		onRun(def_client_generic_handler({return;}));
		onExit(def_client_generic_handler({return;}));
	}

public:


	client() {
		defaultHandlers();
		ip = "";
		port = 100;
	}

	client(string nip, int nport, client_generic_handler run_handler) {
		defaultHandlers();
		ip = nip;
		port = nport;
		onRun(run_handler);
	}

	client(string nip, int nport, client_generic_handler non_init_handler, client_generic_handler non_exit_handler, client_generic_handler run_handler) {
		defaultHandlers();
		ip = nip;
		port = nport;
		onRun(run_handler);
		onInit(non_init_handler);
		onExit(non_exit_handler);
	}

	inline SOCKET* getServerSocketPtr() {
		return &sock;
	}

	inline SOCKET getServerSocket() {
		return sock;
	}

	inline client atIP(string nip) {
		ip = nip;
		ofself
	}

	inline client atIP(ipadress nip) {
		ip = nip.strIP();
		if(nip.isPortAvailable()) {
			port = nip.getPort();
		}
		cout<<"Setup ip = "<<ip<<"\n";
		cout<<"Setup port = "<<port<<"\n";
		ofself
	}

	inline client atPort(int nport) {
		port = nport;
		ofself
	}

	inline client retryWhenFailed(bool state = true) {
		retryWhenFailed__ = state;
		ofself
	}

	inline client onError(client_error_handler arg0) {
		error_handler = arg0;
		ofself
	}

	inline client onInit(client_generic_handler arg0) {
		on_init_handler = arg0;
		ofself
	}

	inline client onExit(client_generic_handler arg0) {
		on_exit_handler = arg0;
		ofself
	}

	inline client onRun(client_generic_handler arg0) {
		on_run_handler = arg0;
		ofself
	}

	int open() {
		int res;

	    sockaddr_in ser;
	    sockaddr addr;

	    ser.sin_family=AF_INET;
	    ser.sin_port=htons(port);

	    ser.sin_addr.s_addr=inet_addr(ip.c_str());
	    memcpy(&addr,&ser,sizeof(SOCKADDR_IN));
	    res = WSAStartup(MAKEWORD(1,1), &WSAdata);

	    if(res != 0) {
			s_cl("WSAStarup failed",WSAGetLastError());
			return 0;
		}

	    sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	    if(sock==INVALID_SOCKET ) {
	        s_cl("Invalid Socket ",WSAGetLastError());
	        return 0;
	    } else if(sock==(SOCKET)SOCKET_ERROR) {
	        s_cl("Socket Error ",WSAGetLastError());
	        return 0;
	    }


		res=connect(sock,&addr,sizeof(addr));
		if(res != 0) {
			int err = WSAGetLastError();
			if(err==WSAEADDRNOTAVAIL) {
			    s_cl("[ERROR] Invalid or unkown IP adress was given!");
			    return 0;
			} else if(err==WSAECONNREFUSED) {
				s_cl("[ERROR] Connection was activly REFUSED!\n[INFO] Better check out the server application...");
				return 0;
			} else {
				s_cl("[ERROR!!!] Connect() invokation has failed and unrecognized error has occured:");
				s_cl("[!!!] Connect() returned value {"+intToStr(res)+"} and WSA-ERR code {"+intToStr(WSAGetLastError())+"}");
				return 0;
			}
		}

		if(res !=0 ) {
	        s_cl("SERVER UNAVAILABLE",res);
			return 0;
	    } else {
	        memcpy(&ser,&addr,sizeof(SOCKADDR));
	    }

	    isRunning = true;

	    *client_connection = make_net_connection(this, &(this->sock));
		on_init_handler(this, client_event(CLIENT_EVENT_ON_INIT), *client_connection);
		while(isRunning) {
			on_run_handler(this, client_event(CLIENT_EVENT_ON_RUN), *client_connection);
		}
	    return 1;
	}

	int close() {
		isRunning = false;
		on_exit_handler(this, client_event(CLIENT_EVENT_ON_EXIT), make_net_connection(this, &(this->sock)));
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	inline client closet() {
		pthread_t thb;
		pthread_create(&thb, NULL, client_closet_thread___, (void*)(this));
		ofself
	}

	inline client opent() {
		pthread_t thb;
		pthread_create(&thb, NULL, client_opent_thread___, (void*)(this));
		ofself
	}

	inline net_connection* getConnection() {
		return client_connection;
	}


	/*inline net_connection* get_connection() {
		return default_net_connection;
	}*/
};

void *client_opent_thread___(void *lpParam) {
	client* cl = ((client*)lpParam);
	if(cl->retryWhenFailed__) {
		while(!cl->open()) {}
	} else {
		cl->open();
	}
	pthread_exit(NULL);
	return nullptr;
}

void *client_closet_thread___(void *lpParam) {
	client* cl = ((client*)lpParam);
	if(cl->retryWhenFailed__) {
		while(!cl->close()) {}
	} else {
		cl->close();
	}
	pthread_exit(NULL);
	return nullptr;
}
