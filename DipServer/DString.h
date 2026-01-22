#if !defined(DIP_SERVER_DSTRING_DEFFF)
#define DIP_SERVER_DSTRING_DEFFF
class DString;
class DString{
public:
	char* str;
	int L;
	int MaxL;
	DString();
	DString(char* s);
	~DString();
	void Allocate(int sz);
	void ReadFromFile(char* file);
	void WriteToFile (char* file);
	void Add(DString& Str);
	void Add(char* Str);
	void Replace0(char* src,char* dst);
	void Replace(char* src,DString& dst);
	void Replace(char* src,int value){
		char cc[16];
		sprintf_s( cc,sizeof(cc),"%d",value);
		Replace(src,cc);
	};
	void Replace(char* src,char* dst,...){
		char ccc[512];
        va_list va;
        va_start(va,dst);
        vsprintf (ccc,dst,va);   
        va_end(va);
		Replace0(src,ccc);
	};
	void Clear();
	void Free();
	void Assign(char* str);
	void Assign(DString& str);
	DString& operator = (char* s);
	DString& operator + (char* s);
	DString& operator + (DString& ds1);
	DString& operator += (char* s);
	DString& operator += (DString& ds);
	bool operator == (char* s);
	bool operator == (DString& ds);
	char operator [] (int index);
};
#endif
