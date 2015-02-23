

template <typename T>
class idresolver {
public:
	typedef typename vector< net_session_<T> >::iterator idresolver_iterator;
	typedef function< T( idresolver<T>* resolver, T& last_id ) > id_allocator;

private:
	vector< net_session_<T> > clients;
	T client_id_null_def;
	net_session_<T> null_find;
	
	T client_uid;
	id_allocator idAllocator;
	
	//TODO: auto net_target data clanup

public:

	idresolver_iterator begin() { return clients.begin(); }
    idresolver_iterator end() { return clients.end(); }
	
	idresolver(T null_def, id_allocator IDAllocator) {
		client_id_null_def = null_def;
		null_find = net_session_<T>(0, null_def, nullptr);
		client_uid = null_def;
		idAllocator = IDAllocator;
	}
	
	inline void clear() {
		clients.clear();	
	}
	
	inline vector<SOCKET> getSocketList() {
		vector<SOCKET> ret;
		const int len = clients.size();
		for(int it=0;it<len;++it) {
			ret.push_back(~clients[it]);
		}
		return ret;
	}	
	
	inline T getLastAllocatedID() {
		return client_uid;
	}
	
	inline T allocateNewID() {
		T id = idAllocator(this, client_uid);
		client_uid = id;
		return id;
	}
	
	inline vector< net_session_<T> > get() {
		return clients;	
	}
	
	inline void add(SOCKET sock, T id) {
		clients.push_back(net_session_<T>(sock, id, nullptr));
	}

	inline void add(SOCKET sock) {
		add(sock, client_id_null_def);
	}

	inline T getID(SOCKET sock) {
		return 	getID(sock, client_id_null_def);
	}
	
	inline T getID(SOCKET sock, T null_def) {
		const int len = clients.size();
		for(int it=0;it<len;++it) {
			if(~clients[it] == sock) {
				return !clients[it];
			}
		}
		return null_def;
	}
	
	inline net_session_<T>* findByID(T id, SOCKET null_def = 0) {
		const int len = clients.size();
		for(int it=0;it<len;++it) {
			if(!clients[it] == id) {
				return clients[it];
			}
		}
		return &null_find;
	}
	
	inline bool assignID(SOCKET sock, T id) {
		const int len = clients.size();
		for(int it=0;it<len;++it) {
			if(~clients[it] == sock) {
				!clients[it] = id;
				return true;	
			}
		}
		return false;
	}

	inline net_session_<T>* find(SOCKET sock, T null_def) {
		const int len = clients.size();
		for(int it=0;it<len;++it) {
			if(~clients[it] == sock) {
				return &clients[it];
			}
		}
		return &null_find;
	}

	inline net_session_<T>* find(SOCKET sock) {
		return find(sock, client_id_null_def);
	}

};

typedef idresolver<int> nullresolver;
