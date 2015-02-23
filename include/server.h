
class server;

enum server_event {
	SERVER_EVENT_ON_INIT = 100,
	SERVER_EVENT_ON_EXIT = 101,
	SERVER_EVENT_ON_NEW_CONNECTION = 102,
	SERVER_EVENT_ON_RUN = 0
};

struct initial_client_thread_information___ {
	server* server_ptr;
	SOCKET* client_socket_ptr;

	inline void dispose() {
		delete client_socket_ptr;
	}
};

void *clientHandlingThread(void *lpParam);

#define def_server_generic_handler(CODE) [](server* server, server_event event, net_connection target)->void CODE
#define def_server_error_handler(CODE) [](string arg0, int arg1)->void CODE
#define def_server_generic_handler_ex(VARS, CODE) VARS(server* server, server_event event, net_connection target)->void CODE
#define def_server_error_handler_ex(VARS, CODE) VARS(string arg0, int arg1)->void CODE
#define as_server_generic_handler(FUNCTNAME) [](server* arg0, server_event arg1, net_connection arg2)->void { FUNCTNAME (arg0, arg1, arg2); }
#define as_server_error_handler(FUNCTNAME) [](string arg0, int arg1)->void { FUNCTNAME (arg0, arg1); }
#define default_server_generic_handler def_server_generic_handler({return;})
#define default_server_error_handler def_server_error_handler({return;})


void *server_opent_thread___(void *lpParam);
void *server_closet_thread___(void *lpParam);

struct server_setup {
	bool autoPing;
	bool autoSessionCleanup;
};

#define setup_server(X) server_setup(X)
#define server_loop_end() if(server->loopOver(target)) break;
#define server_loop_start() if(server->loopStart(target)) return;
#define def_server_loop(CODE) while(true){server_loop_start();local{CODE};server_loop_end();}

class server : public net_stack_receiver {

public:
	typedef function<void(string, int)> server_error_handler;
	typedef function<void(server*, server_event, net_connection)> server_generic_handler;

private:

friend void *clientHandlingThread(void *lpParam);
friend void *server_opent_thread___(void *lpParam);
friend void *server_closet_thread___(void *lpParam);

private:
	server_error_handler error_handler;
	server_generic_handler on_init_handler;
	server_generic_handler on_exit_handler;
	server_generic_handler on_run_handler;
	server_generic_handler on_new_connection_handler;

	server_setup setup = {
		autoPing: false,
		autoSessionCleanup: false
	};

	SOCKET sock;
	WSADATA WSAdata;
	string ip, hostname;
	int port;
	idresolver<string>* sockets;

	int client_uid = 1;

	bool isRunning = false;
	bool acceptNewConnection = false;
	bool retryWhenFailed__ = false;

	inline void receiveCurIPAdress__() {
	    char ac[80];
	    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
	        cerr << "Error " << WSAGetLastError() <<
	                " when getting local host name." << endl;
	        return;
	    }
	    string str(ac);
	    hostname=str;

	    struct hostent *phe = gethostbyname(ac);
	    if (phe == 0) {
	        cerr << "Yow! Bad host lookup." << endl;
	        return;
	    }

	    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
	        struct in_addr addr;
	        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
	        ip = inet_ntoa(addr);
	    }

	}

	inline void s_cl(string message, int errcode=0) {
		error_handler(message, errcode);
	}

	inline void defaultHandlers() {
		onError(def_server_error_handler({return;}));
		onInit(def_server_generic_handler({return;}));
		onRun(def_server_generic_handler({return;}));
		onExit(def_server_generic_handler({return;}));
		onConnected(def_server_generic_handler({return;}));
	}

public:

	server() {
		sockets = new idresolver<string>("0", [](idresolver<string>* idr, string last_id)->string {
			return intToStr( strToInt(last_id)+1 );
		});
		defaultHandlers();
		port = 100;
		ip = "";
	}

	server(int nport, string nip, server_generic_handler run_handler) {
		sockets = new idresolver<string>("0", [](idresolver<string>* idr, string last_id)->string {
			return intToStr( strToInt(last_id)+1 );
		});
		defaultHandlers();
		port = nport;
		ip = nip;
		onRun(run_handler);
	}

	server(int nport, string nip, server_generic_handler non_init_handler, server_generic_handler non_exit_handler, server_generic_handler run_handler) {
		sockets = new idresolver<string>("0", [](idresolver<string>* idr, string last_id)->string {
			return intToStr( strToInt(last_id)+1 );
		});
		defaultHandlers();
		port = nport;
		ip = nip;
		onRun(run_handler);
		onInit(non_init_handler);
		onExit(non_exit_handler);
	}


	inline bool loopStart(net_connection& nc) {
		if(setup.autoPing) {
			//nc.measurePing();
		}
		return false;
	}

	inline bool loopOver(net_connection& nc) {
		if(setup.autoPing) {
			if(setup.autoSessionCleanup) {
				if(!nc.ping().alive()) {
					return true;
				}
			}
			//nc.measurePing();
		}
		return false;
	}

	inline idresolver<string>* getIDResolver() {
		return sockets;
	}

	inline server retryWhenFailed(bool state = true) {
		retryWhenFailed__ = state;
		ofself
	}

	inline server atIP(string nip) {
		ip = nip;
		ofself
	}

	inline server atIP(ipadress nip) {
		ip = nip.strIP();
		if(nip.isPortAvailable()) {
			port = nip.getPort();
		}
		ofself
	}

	inline server atPort(int nport) {
		port = nport;
		ofself
	}

	template <typename D>
	inline net_session* installClient(net_connection target, const D& data) {
		net_session* ptr = sockets->find(target.getTargetSocket());
		ptr->setData(data);
		return ptr;
	}

	template <typename D>
	inline net_session* installClient(net_connection target, D* data) {
		net_session* ptr = sockets->find(target.getTargetSocket());
		ptr->setData(data);
		return ptr;
	}

	inline net_session& getClient(net_connection target) {
		return *(sockets->find(target.getTargetSocket()));
	}

	inline net_session& operator[](net_connection target) {
		return *(sockets->find(target.getTargetSocket()));
	}

	inline vector< net_session > getConnectedClients() {
		return (sockets->get());
	}

	inline server refuseConnection() {
		acceptNewConnection = false;
		ofself
	}

	inline server acceptConnection() {
		acceptNewConnection = true;
		ofself
	}

	inline server applySettings(server_setup sstp) {
		setup = sstp;
		ofself
	}

	inline server onError(server_error_handler arg0) {
		error_handler = arg0;
		ofself
	}

	inline server onInit(server_generic_handler arg0) {
		on_init_handler = arg0;
		ofself
	}

	inline server onExit(server_generic_handler arg0) {
		on_exit_handler = arg0;
		ofself
	}

	inline server onRun(server_generic_handler arg0) {
		on_run_handler = arg0;
		ofself
	}

	inline server onConnected(server_generic_handler arg0) {
		on_new_connection_handler = arg0;
		ofself
	}

	int open() {
		sockaddr_in server_addr;

		int ret = WSAStartup(0x101, &WSAdata);

		if(ret != 0) {
			s_cl("[!] Cannot setup server (WSAStartup error).");
			return 0;
		}
		receiveCurIPAdress__();

		server_addr.sin_family=AF_INET;
		server_addr.sin_addr.s_addr=INADDR_ANY;
		server_addr.sin_port=htons(port);

		sock=socket(AF_INET,SOCK_STREAM,0);

		if(sock == INVALID_SOCKET) {
			s_cl("[!] Cannot setup server (ip host is invliad or not avaliable).");
			return 0;
		}

		if( bind(sock,(sockaddr*)&server_addr,sizeof(server_addr)) !=0 ) {
			s_cl("[!] Cannot setup server (no connection).");
			return 0;
		}

		if(listen(sock,5) != 0) {
			s_cl("[!] Cannot setup server (no connection).");
			return 0;
		}
		isRunning = true;

		on_init_handler(this, server_event(SERVER_EVENT_ON_INIT), make_net_connection(this, nullptr));

		SOCKET client;

		sockaddr_in from;
		int fromlen = sizeof(from);
		pthread_t thb;

		while(isRunning) {
			acceptNewConnection = true;
			client = accept(sock,(struct sockaddr*)&from,&fromlen);
			on_new_connection_handler(this, server_event(SERVER_EVENT_ON_NEW_CONNECTION), make_net_connection(this, &client));
			if(acceptNewConnection) {
				sockets->add(client, intToStr(client_uid));
				++client_uid;

				initial_client_thread_information___* icti = new initial_client_thread_information___();
				icti->client_socket_ptr = new SOCKET();
				*icti->client_socket_ptr = client;
				icti->server_ptr = this;

				pthread_create(&thb, NULL, clientHandlingThread, (void*)(icti));
			} else {
				closesocket(client);
			}
		}
		return 1;

	}

	int close() {
		isRunning = false;
		closesocket(sock);
		WSACleanup();
		delete sockets;
		port = -1;
		ip = "0.0.0.0";
		ExitThread(0);
		return 1;
	}

	inline server closet() {
		pthread_t thb;
		pthread_create(&thb, NULL, server_closet_thread___, (void*)(this));
		ofself
	}

	inline server opent() {
		pthread_t thb;
		pthread_create(&thb, NULL, server_opent_thread___, (void*)(this));
		ofself
	}
};

void *server_opent_thread___(void *lpParam) {
	server* sv = ((server*)lpParam);
	if(sv->retryWhenFailed__) {
		while(!(sv->open())) {}
	} else {
		sv->close();
	}
	pthread_exit(NULL);
	return nullptr;
}

void *server_closet_thread___(void *lpParam) {
	server* sv = ((server*)lpParam);
	if(sv->retryWhenFailed__) {
		while(!sv->close()) {}
	} else {
		sv->close();
	}
	pthread_exit(NULL);
	return nullptr;
}

void *clientHandlingThread(void *lpParam) {
	initial_client_thread_information___* icti = (initial_client_thread_information___*)lpParam;
	SOCKET* current_client = icti->client_socket_ptr;
	DEBUG({
		NETLOG("[Process.Create] New thread was created to handle new connection (socket="+intToStr(*current_client)+")\n");
	});
	(icti->server_ptr)->on_run_handler(icti->server_ptr, server_event(SERVER_EVENT_ON_RUN), make_net_connection(icti->server_ptr, current_client));
	DEBUG({
		NETLOG("[Process.Terminate] The thread handling some connection was terminated (socket="+intToStr(*current_client)+")\n");
	});
	icti->dispose();
	delete icti;

	pthread_exit(NULL);
	return nullptr;
}
