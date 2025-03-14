typedef void fnVoid();
typedef void fnVoidLPB(byte*);
typedef void fnVoidLPC(char*);
typedef void tpSaveAllDipData(byte** ptr,int* size);
typedef void tpPerformDipCommand(char* Data,int size);
typedef void tpLoadAllDipData(byte* ptr,int size);
typedef void tpStartDownloadInternetFile(char* Name,char* Server,char* DestName);
typedef void tpProcessDownloadInternetFiles();
typedef void tpSendRecBuffer(byte* Data,int size,bool Final);

extern fnVoid* ProcessDipRelations;
extern fnVoid* InitDipForThisMap;
extern fnVoid* ClearAllDipData;
extern fnVoidLPB* FreeSaveDipData;
extern tpSaveAllDipData* SaveAllDipData;
extern tpLoadAllDipData* LoadAllDipData;
extern tpPerformDipCommand* PerformDipCommand;
extern tpStartDownloadInternetFile* StartDownloadInternetFile;
extern tpProcessDownloadInternetFiles* ProcessDownloadInternetFiles;
extern tpSendRecBuffer* SendRecBuffer;