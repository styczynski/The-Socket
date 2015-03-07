#include "header.h"

bool setting_debug_mode = true;

#ifdef DEV_PLATFORM_WINDOWS
	#define thread(NAME) void __cdecl NAME(void* Args)
	#define beginThread(NAME)  ((HANDLE)_beginthread(NAME,0,NULL))
	#define endThread() _endthread();
	#define waitForThread(NAME,TIMEOUT) WaitForSingleObject(NAME,TIMEOUT)
	#define threadh HANDLE
#endif
#ifdef DEV_PLATFORM_UNIX
	#define thread(NAME) int NAME()
	#define beginThread(NAME) (NAME())
	#define endThread() return 0
	#define waitForThread(NAME,TIMEOUT) (0)
	#define WAIT_OBJECT_0 0
	#define WAIT_TIMEOUT 1
	#define threadh int
#endif


#ifdef ALLOW_DEBUG
	#define DEBUG(CODE) if(setting_debug_mode) CODE;
#else
	#define DEBUG(CODE)
#endif

#define forever while(1)
#define nop Sleep(0)
#define rest() forever{nop;}
#define local if(1)
#define dead if(0)
#define ofself return (*this);

#define MACRO_STR(name) #name

using namespace std;

string intToStr(int n)
{
     string tmp, ret;
     if(n < 0) {
          ret = "-";
          n = -n;
     }
     do {
          tmp += n % 10 + 48;
          n -= n % 10;
     }
     while(n /= 10);
     for(int i = tmp.size()-1; i >= 0; i--)
          ret += tmp[i];
     return ret;
}


int strToInt(string s)
{
     int tmp = 0, i = 0;
     bool m = false;
     if(s[0] == '-') {
          m = true;
          i++;
     }
     const int len = (signed int)s.size();
     for(; i < len; i++)
          tmp = 10 * tmp + s[i] - 48;
     return m ? -tmp : tmp;
}


inline string lcase(string str) {
	transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

inline pair<string, string> splitStrEq(string str) {
	const int len = str.size();
	bool mode = false;
	string buf = "0";
	string eq0 = "";
	string eq1 = "";

	for(int it=0;it<len;++it) {
		if(str[it]=='=') {
				mode = true;
		} else {
			buf[0] = str[it];
			if(mode) {
				eq1+=buf;
		    } else {
		    	eq0+=buf;
		    }
		}
	}
	return make_pair(eq0, eq1);

}

class Splitter {
  std::vector<std::string> _tokens;
public:
  typedef std::vector<std::string>::size_type size_type;
public:
  Splitter ( const std::string& src, const std::string& delim )
  {
    reset ( src, delim );
  }

  std::string& operator[] ( size_type i )
  {
    return _tokens.at ( i );
  }

  size_type size() const
  {
    return _tokens.size();
  }

  void reset ( const std::string& src, const std::string& delim )
  {
    std::vector<std::string> tokens;
    std::string::size_type start = 0;
    std::string::size_type end;
    for ( ; ; ) {
      end = src.find ( delim, start );
      tokens.push_back ( src.substr ( start, end - start ) );
      if ( end == std::string::npos )
        break;
      start = end + delim.size();
    }
    _tokens.swap ( tokens );
  }
};

inline char stri(string str, int index, char default_return=0x00) {
	if(index>=0&&index<str.size()) {
		return str[index];
	}
	return default_return;
}

inline bool str_contains(string arg0, string arg1) {
	return arg0.find(arg1) != string::npos;
}
