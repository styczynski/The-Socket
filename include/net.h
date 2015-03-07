#include "basic.h"

namespace net {

	#define SPECIAL_COMMAND_PING_RETURNED "##:{ping.return}"
	#define SPECIAL_COMMAND_PING "##:{ping}"

	string logger = "";

	#ifdef ALLOW_DEBUG
		#ifdef NET_DEBUG
			#ifndef NET_DEBUG_OUT
				#define NETLOG(str) logger+=str;
			#else
				#define NETLOG(str) logger+=str;NET_DEBUG_OUT (str);
			#endif
		#else
			#define NETLOG(str)
		#endif
	#else
		#ifdef NET_DEBUG
			#warning "You used NET_DEBUG without ALLOW_DEBUG definition!\nPlease define ALLOW_DEBUG!"
		#endif
	#endif

	//int NET_ERROR = 0;
	#define NET_MAX_DATA_SIZE 900
	#define NET_BAD 0x01
	#define NET_TIMEOUT 0x02
	#define NET_GOOD 0x00
	#define isGoodNet() ((NET_ERROR == NET_GOOD))
	#define NET_NULL ("<null>")
	#define isNetNull(X) (X==NET_NULL)

	#define ipv4 ipadress
	#define IPV4(x) MACRO_STR(x)

	const static char NET_SPEC_TRANSMISSION_OVER_SIGN__  = '\0';


	template <typename T>
	class net_session_ {
		private:
			SOCKET socket;
			T id;
			void* data;

		public:

			net_session_() {
				socket = 0;
			}

			net_session_(SOCKET argSocket, T argID, void* argData) {
				socket = argSocket;
				id = argID;
				data = argData;
			}

			inline T& getID() {
				return id;
			}

			inline SOCKET& getSocket() {
				return socket;
			}

			inline T& operator!() {
				return id;
			}

			inline SOCKET& operator~() {
				return socket;
			}

			inline operator SOCKET() {
				return socket;
			}

			inline operator T() {
				return id;
			}

			inline void setID(T newID) {
				id = newID;
			}

			inline void* getRawDataPointer() {
				return data;
			}

			template <typename D>
			inline D* getDataPointer() {
				return (D*)(data);
			}

			template <typename D>
			inline D& getData() {
				return *((D*)(data));
			}

			template <typename D>
			inline void setData(const D& newData) {
				data = (void*)(&newData);
			}

			template <typename D>
			inline void setData(D* newDataPtr) {
				data = (void*)newDataPtr;
			}

	};
	typedef net_session_<string> net_session;

	template <typename T>
	struct netresult {
		private:
			T data;
			int NET_ERROR = NET_GOOD;

		public:
			netresult() {

			}

			netresult(T arg0, int errcode) {
				data = arg0;
				NET_ERROR = errcode;
			}

			netresult(T arg0) {
				data = arg0;
			}

			inline int errCode() {
				return NET_ERROR;
			}

			inline bool good() {
				return NET_ERROR == NET_GOOD;
			}

			inline void oferr(int x) {
				if(NET_ERROR==NET_GOOD) {
					NET_ERROR = x;
				}
			}

			inline void operator=(netresult x) {
				data = x.data;
			}

			inline void operator=(T x) {
				data = x;
			}

			inline bool operator==(netresult x) {
				return data == x.data;
			}

			inline bool operator==(T x) {
				return data == x;
			}

			inline T getData() {
				return data;
			}
	};

	struct net_connection;
	inline void netcn_funct_wrapper_on_ping_received(net_connection* nc, clock_t now);

	struct ipadress {
		private:
			int keya;
			int keyb;
			int keyc;
			int keyd;
			int port;

		public:
			ipadress(string ip) {
				 port = -1;
				 Splitter split ( ip, "." );
				 keya = strToInt( split[0] );
				 keyb = strToInt( split[1] );
				 keyc = strToInt( split[2] );
				 keyd = strToInt( split[3] );
				 Splitter sps ( ip, ":" );
				 if(sps.size()>1) {
				 	port = strToInt(sps[1]);
				 }
			}

			ipadress(int a, int b, int c, int d, int nport = -1) {
				port = nport;
				keya = a;
				keyb = b;
				keyc = c;
				keyd = d;
			}

			ipadress(long long a, int nport = -1) {
				port = nport;
				keyd = a%1000;
				a = (a-keyd)/1000;
				keyc = a%1000;
				a = (a-keyd)/1000;
				keyb = a%1000;
				a = (a-keyd)/1000;
				keya = a%1000;
			}

			inline string strIP() {
				return 	intToStr(keya)+"."+intToStr(keyb)+"."+intToStr(keyc)+"."+intToStr(keyd);
			}

			inline int getPort() {
				return port;
			}

			inline bool isPortAvailable() {
				return port != -1;
			}
	};

	inline netresult<int> sendRawNet(SOCKET socket, string data, net_connection* cn) {

		int NET_ERROR = NET_GOOD;
		int result = send(socket, data.c_str(), sizeof(data.c_str()), 0);

		return netresult<int>(result, NET_ERROR);
	}

	inline netresult<string> recvRawNet(SOCKET socket, net_connection* cn) {

		int NET_ERROR = NET_GOOD;

		char data[NET_MAX_DATA_SIZE];
		for(int it=0;it<NET_MAX_DATA_SIZE;++it) {
			data[it] = '\0';
		}
		if(recv(socket, data, NET_MAX_DATA_SIZE, 0)==0x00)  {
			NET_ERROR = NET_GOOD;
		} else {
			NET_ERROR = NET_BAD;
		}

		return netresult<string>( string(data), NET_ERROR );
	}

	netresult<string> recvRawNetTimeoutReceiver_output___;
	SOCKET recvRawNetTimeoutReceiver_input___;
	net_connection* recvRawNetTimeoutReceiver_input_cn___;
	thread(recvRawNetTimeoutReceiver___) {
		recvRawNetTimeoutReceiver_output___ = recvRawNet(recvRawNetTimeoutReceiver_input___, recvRawNetTimeoutReceiver_input_cn___);
		ExitThread(0);
	}

	inline netresult<string> recvRawNet(SOCKET socket, long timeout, net_connection* cn) {
		int NET_ERROR = NET_GOOD;

		recvRawNetTimeoutReceiver_input___ = socket;
		recvRawNetTimeoutReceiver_input_cn___ = cn;
		recvRawNetTimeoutReceiver_output___ = "<error>";

		auto hThread = beginThread(recvRawNetTimeoutReceiver___);
		auto result = waitForThread(hThread, timeout);

		if( result == WAIT_OBJECT_0 ) {
			if(recvRawNetTimeoutReceiver_output___=="<error>") {
				NET_ERROR = NET_BAD;
				return netresult<string>("", NET_ERROR);
			} else {
				recvRawNetTimeoutReceiver_output___.oferr(NET_ERROR);
				return recvRawNetTimeoutReceiver_output___;
			}
		} else if( result == WAIT_TIMEOUT ){
			NET_ERROR = NET_TIMEOUT;
			return netresult<string>("", NET_ERROR);
		}

		if(recvRawNetTimeoutReceiver_output___=="<error>") {
			NET_ERROR = NET_BAD;
			return netresult<string>("", NET_ERROR);
		}

		recvRawNetTimeoutReceiver_output___.oferr(NET_ERROR);
		return netresult<string>(recvRawNetTimeoutReceiver_output___);
	}

	inline netresult<int> sendNet(SOCKET socket, string data, net_connection* cn) {
		int NET_ERROR = NET_GOOD;

		NET_ERROR = NET_GOOD;

		DEBUG({
			NETLOG("[Connection] to "+intToStr(socket)+": sendNet("+data+")\n");
		});

		string ppt = "0";
		ppt[0] = NET_SPEC_TRANSMISSION_OVER_SIGN__;

		const char* buffer = (data+ppt).c_str();
		const unsigned int length = data.size()+1;
		size_t i = 0;

	    int apss = length*2.5;
	    int apst = 0;
	    for (i = 0; i < length; i += send(socket, buffer, length - i, 0)) {
	    	++apst;
	    	if(apst>apss) {
	    		 return netresult<int>(i, NET_ERROR);
	    	}
	    };

	    return netresult<int>(length, NET_ERROR);
	}


	struct net_stack_sorter__ {
		inline bool operator()(const string &a, const string &b) const {
			if(a==SPECIAL_COMMAND_PING_RETURNED) return false;
			if(b==SPECIAL_COMMAND_PING_RETURNED) return true;
			return a>b;
		}
	} net_stack_sorter;

	class net_stack_receiver {

		public:
		deque <string> *recv_stack_;


		net_stack_receiver() {
			recv_stack_ = new deque<string>();
		}

		inline vector<string> observeMessageQueue() {
			vector<string> result;
			for(string s : *recv_stack_) {
				result.push_back(s);
			}
			return result;
		}

		inline void dispatchSpecials(deque<string>& recv_stack, SOCKET socket, net_connection* cn) {
			DEBUG({
				string str = "";
				for(int it=0;it<recv_stack.size();++it) {
					str += "["+recv_stack[it]+"], ";
				}
				NETLOG("[Special-dispatch:before] Stack: {"+str+"}\n");
			});
			int len = recv_stack.size();
			for(int it=0;it<len;++it) {
				if(true) {
					if(recv_stack[it]==SPECIAL_COMMAND_PING) {
						recv_stack.erase(recv_stack.begin()+it);
						--len;
						--it;
						sendNet(socket, SPECIAL_COMMAND_PING_RETURNED, cn);
					} else if(recv_stack[it]==SPECIAL_COMMAND_PING_RETURNED) {
						recv_stack.erase(recv_stack.begin()+it);
						--len;
						--it;
						netcn_funct_wrapper_on_ping_received( cn, clock() );
					} else {

					}
				}
			}
			DEBUG({
				string str = "";
				for(int it=0;it<recv_stack.size();++it) {
					str += "["+recv_stack[it]+"], ";
				}
				NETLOG("[Special-dispatch:after] Stack: {"+str+"}\n");
			});
		}

		inline netresult<string> recvNet(SOCKET socket, net_connection* cn, long timeout=-1) {

			deque <string> recv_stack = *recv_stack_;
			//sort(recv_stack.begin(), recv_stack.end(), net_stack_sorter);

			int NET_ERROR = NET_GOOD;
			NET_ERROR = NET_GOOD;

			dispatchSpecials(recv_stack, socket, cn);
			/*if(recv_stack.size()>0) {
				if(recv_stack.back()==SPECIAL_COMMAND_PING) {
					recv_stack.pop_back();
					sendNet(socket, SPECIAL_COMMAND_PING_RETURNED, cn);
				} else if(recv_stack.back()==SPECIAL_COMMAND_PING_RETURNED) {
					recv_stack.pop_back();
					netcn_funct_wrapper_on_ping_received( cn, clock() );
				}
			}*/

			if(recv_stack.size()>0) {
				string pom = recv_stack.back();
				recv_stack.pop_back();
				DEBUG({
					NETLOG("[Connection] from "+intToStr(socket)+": recvNet("+pom+")\n");
				});
				return netresult<string>(pom, NET_ERROR);
			}


			DEBUG({
				NETLOG("[Connection] from "+intToStr(socket)+": listening... (stack-size: "+intToStr(recv_stack.size())+")\n");
			});

			string result = "\0";
			bool cont = true;
			int it = 0;
			while(cont) {

				++it;
				if(NET_ERROR != NET_GOOD) {
					NET_ERROR = NET_TIMEOUT;
					DEBUG({
						NETLOG("[Connection] from "+intToStr(socket)+": recvNet(TERMINATED_CONNECTION)\n");
					});
					return netresult<string>("", NET_ERROR);
				}

				Sleep(1);

				netresult<string> rcvresult;
				if(timeout<0) {
					rcvresult = recvRawNet(socket, cn);
				} else {
					rcvresult = recvRawNet(socket, timeout, cn);
				}

				if(NET_ERROR == NET_GOOD) {
					NET_ERROR = rcvresult.errCode();
				}
				result += rcvresult.getData();

				string ppt = "00";
				ppt[0] = NET_SPEC_TRANSMISSION_OVER_SIGN__;
				ppt[1] = NET_SPEC_TRANSMISSION_OVER_SIGN__;

				result += ppt;
				const int len = result.size();
				string buf = "";
				for(int it=0;it<len;++it) {

					if(result[it]==NET_SPEC_TRANSMISSION_OVER_SIGN__) {
						if(buf!="") {
							recv_stack.push_front(buf);
						}
						buf = "";
					} else {
						buf += "0";
						buf[buf.size()-1] = result[it];
					}
				}

				dispatchSpecials(recv_stack, socket, cn);

				/*if(recv_stack.size()>0) {
					if(recv_stack.back()==SPECIAL_COMMAND_PING) {
						recv_stack.pop_back();
						sendNet(socket, SPECIAL_COMMAND_PING_RETURNED, cn);
					} else if(recv_stack.back()==SPECIAL_COMMAND_PING_RETURNED) {
						recv_stack.pop_back();
						netcn_funct_wrapper_on_ping_received( cn, clock() );
					}
				}*/

				if(recv_stack.size()>0) {
					string pom = recv_stack.back();
					recv_stack.pop_back();
					DEBUG({
						NETLOG("[Connection] from "+intToStr(socket)+": recvNet("+pom+")\n");
					});
					return netresult<string>(pom, NET_ERROR);
				} else {
					DEBUG({
						NETLOG("[Connection] from "+intToStr(socket)+": recvNet("+""+")\n");
					});
					return netresult<string>("", NET_ERROR);
				}

			}
			return netresult<string>("<null>", NET_ERROR);
		}
	};

	//inline bool goodNet() {
	//	return NET_ERROR == NET_GOOD;
	//}

	struct netping {
		private:
			int val;
			bool active;
			bool hasTimeout;

		public:
			netping(int arg0, bool arg1, bool arg2) {
				val = arg0;
				active = arg1;
				hasTimeout = arg2;
			}

			inline bool ready() {
				return (active || hasTimeout);
			}

			inline int ms() {
				return val;
			}

			inline float sec() {
				return (float)val*0.001f;
			}

			inline bool timeout() {
				return hasTimeout;
			}

			inline bool alive() {
				return !(ready() && timeout());
			}

			inline bool goodVal() {
				return val>=0;
			}

			inline bool good() {
				return active && !hasTimeout && val>=0;
			}

	};


	struct net_connection {
		friend inline void netcn_funct_wrapper_on_ping_received(net_connection* nc, clock_t now);

		private:
			SOCKET* from;
			net_stack_receiver* net_from;
			int last_error = NET_GOOD;
			int ping_val = -1;
			int last_good_ping_val = -1;
			clock_t lastPingMeasurement;
			bool pingForceCancelled = false;
			bool pingStillActive = false;

			inline void onPingReceived(clock_t finalTime) {
				 ping_val = float( clock() - lastPingMeasurement ) * 1000.0 /  CLOCKS_PER_SEC;
				 if(ping_val>=0) last_good_ping_val=ping_val;
				 lastPingMeasurement = clock();
				 pingStillActive = false;
				 pingForceCancelled = false;
			}

		public:

			net_connection() {
				from = nullptr;
				net_from = nullptr;
			}

			net_connection(net_stack_receiver* receiver, SOCKET* data) {
				from = data;
				net_from = receiver;
			}

			inline SOCKET getTargetSocket() {
				return *from;
			}

			inline vector<string> observeMessageQueue() {
				return net_from->observeMessageQueue();
			}

			inline bool valid() {
				return from != nullptr;
			}

			inline bool good() {
				return true;//return goodNet();
			}

			inline netping ping() {
				return netping(ping_val, !pingStillActive, pingForceCancelled);
			}

			inline int measurePing() {
				if(pingStillActive) {
					if(float( clock() - lastPingMeasurement ) * 1000.0 /  CLOCKS_PER_SEC > 100) {
						pingStillActive = false;
						lastPingMeasurement = clock();
						ping_val = -1;
						pingForceCancelled = true;
						DEBUG({
							NETLOG("[PING] Ping was measured.\n");
						});
					}
				} else {
					pingStillActive = true;
					lastPingMeasurement = clock();
					DEBUG({
						NETLOG("[PING] Ping request was sent.\n");
					});
					send(SPECIAL_COMMAND_PING);
				}
				return -1;
			}

			inline bool alive() {
				auto rs = ping();
				return (rs.ready() && rs.timeout()) || (!rs.ready());
			}

			inline void send(string data) {
				auto result = sendNet(*from, data, this);
				last_error = result.errCode();
			}

			inline void dispatch() {
				auto result = recv(10);
				while(result.errCode()!=NET_GOOD) result=recv(10);
			}

			inline netresult<string> recv(long timeout=-1) {
				auto result = net_from->recvNet(*from, this, timeout);
				last_error = result.errCode();
				return result;
			}

			#if defined(STANDARD_ALPHA) || defined(NET_STANDARD_ALPHA)
				inline void sendRaw(string data) {
					auto result = sendRawNet(*from, data, this);
					last_error = result.errCode();
				}

				inline string recvNet(long timeout=250) {
					auto result = recvRawNet(*from, timeout, this);
					last_error = result.errCode();
					return result;
				}
			#endif
	};


	class net_recipient {
		private:
			vector< net_session > recipients; //net_stack_receiver

			inline void addSocketTarget(net_session sck) {
				recipients.push_back(sck);
			}

			inline void addSocketTarget(vector< net_session > sck) {
				for(net_session s : sck) recipients.push_back(s);
			}

			inline void addSocketTarget(initializer_list< net_session > sck) {
				for(net_session s : sck) recipients.push_back(s);
			}

		public:
			typedef typename vector< net_session >::iterator net_recipient_iterator;

			net_recipient() {

			}

			net_recipient(net_session newRecipient) {
				addSocketTarget(newRecipient);
			}

			net_recipient(vector< net_session > newRecipients) {
				addSocketTarget(newRecipients);
			}

			net_recipient(initializer_list< net_session > newRecipients) {
				addSocketTarget(newRecipients);
			}

			net_recipient_iterator begin() { return recipients.begin(); }
    		net_recipient_iterator end() { return recipients.end(); }
	};

	class net_message {
		private:
			vector<string> data;
		public:

			net_message() {

			}

			net_message(string str) {
				data.push_back(str);
			}

			net_message(vector<string> l) {
				for(string s : l) {
					data.push_back(s);
				}
			}

			net_message(initializer_list<string> l) {
				for(string s : l) {
					data.push_back(s);
				}
			}

			inline net_message& recvFrom(net_connection c) {
				netresult<string> vr = c.recv();
				if(vr.good()) {
					data.push_back(vr.getData());
				}
				return *(this);
			}

			inline string& get() {
				return data.back();
			}

			inline string& operator[](int index) {
				return data[index];
			}

			inline void sendTo(net_connection c) {
				for(string s : data) {
					c.send(s);
				}
			}

			inline void sendTo(vector<net_connection> c) {
				for(string s : data) {
					for(net_connection ce : c) ce.send(s);
				}
			}

			inline void sendTo(initializer_list<net_connection> c) {
				sendTo(vector<net_connection>(c));
			}

			inline void sendTo(net_stack_receiver* receiver, SOCKET* c) {
				sendTo(net_connection(receiver, c));
			}

			inline void sendTo(net_stack_receiver* receiver, SOCKET c) {
				sendTo(net_connection(receiver, &c));
			}

			inline void sendTo(net_stack_receiver* receiver, vector<SOCKET> c) {
				for(SOCKET& ce : c) {
					sendTo(net_connection(receiver, &ce));
				}
			}

			inline void sendTo(net_stack_receiver* receiver, initializer_list<SOCKET> c) {
				for(SOCKET ce : c) {
					sendTo(net_connection(receiver, &ce));
				}
			}

			inline void sendTo(net_stack_receiver* receiver, net_recipient recipients) {
				for(SOCKET ce : recipients) {
					sendTo(net_connection(receiver, &ce));
				}
			}

			inline void sendTo(net_stack_receiver* receiver, vector< net_session > recipients) {
				for(SOCKET ce : recipients) {
					sendTo(net_connection(receiver, &ce));
				}
			}

			inline void sendTo(net_stack_receiver* receiver, initializer_list< net_session > recipients) {
				for(net_session ce : recipients) {
					sendTo(net_connection(receiver, &(~ce)));
				}
			}

			inline void clearMessage() {
				data.clear();
			}

			inline void addToMessage(string s) {
				data.push_back(s);
			}

			inline void addToMessage(vector<string> l) {
				for(string s : l) {
					data.push_back(s);
				}
			}

			inline void addToMessage(initializer_list<string> l) {
				for(string s : l) {
					data.push_back(s);
				}
			}
	};

	inline void netcn_funct_wrapper_on_ping_received(net_connection* nc, clock_t now) {
		cout.flush();
		nc->onPingReceived(now);
	}

	inline net_connection make_net_connection(net_stack_receiver* receiver, SOCKET* data) {
		net_connection ret(receiver, data);
		return ret;
	}


	#include "client.h"
	#include "server_idresolver.h"
	#include "server.h"

}
