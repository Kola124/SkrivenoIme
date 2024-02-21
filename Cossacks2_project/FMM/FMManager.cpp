#include <windows.h>
#include <stdio.h>              
#include <stdlib.h>             
#include <assert.h>             
#include "FMManager.h"
//-----------------------------------------------------------------------------------
FMManager MManager;
//-----------------------------------------------------------------------------------
bool FMManager::isInit = false;
//-----------------------------------------------------------------------------------

//#define STARFORCE

//#ifdef STARFORCE
//GLOBAL PTR_AI=NULL;
//#endif
bool FMManager::Initialize(unsigned totalSize)
{
/*
#ifdef STARFORCE
	try{
		if(((DWORD*)PTR_AI)[0]=='NFMM')return true;
	}catch(...){
		return true;
	};
#endif
	*/
	if(isInit) 
		return true;
	ZeroMemory(&HeapBlock, sizeof(HeapBlock));
	ZeroMemory(&MemoryStatus, sizeof(MemoryStatus));
	ZeroMemory(HashTableA, sizeof(HashTableA));
	ZeroMemory(HashTableS, sizeof(HashTableS));
	InvalidHandlers = NULL;
	Handlers = NULL;
	handlers = 0;
	HeapEnd = NULL;
	PageSize = 0;
	PhysSize = 0;
	isAllocateAllPages = true;

	FMM_LOG_OPEN();

	SYSTEM_INFO sSysInfo; 
	GetSystemInfo(&sSysInfo);
	GlobalMemoryStatus(&MemoryStatus);

	PageSize = sSysInfo.dwPageSize;
	PhysSize = (unsigned)((unsigned __int64)MemoryStatus.dwAvailPhys*FMM_PHYSMEM_PERCENT)/100;

	HeapBlock.BaseAddress = NULL;
	while(totalSize >= FMM_MINMEM_RESERVE)
	{
		HeapBlock.BaseAddress = 
			(FMPTR)VirtualAlloc(NULL, totalSize, MEM_RESERVE, PAGE_NOACCESS);
		if(!HeapBlock.BaseAddress)
			totalSize -= 1024*1024*20;
		else
			break;
	}
	if(!HeapBlock.BaseAddress)
	{
		FMM_ERROR("VirtualAlloc heap reserve failed\nNot enought memory to run program");
		return false;
	}
	HeapBlock.TotalSize = totalSize;
	HeapBlock.TotalPages = (HeapBlock.TotalSize + PageSize-1)/PageSize;
	HeapEnd = HeapBlock.BaseAddress;

	// - ������������ ���� �������
	unsigned hSize = FMM_MAXHANDLERS*sizeof(Handlers[0]);
	Handlers = (FMMHANDLER*)VirtualAlloc(NULL, hSize, MEM_COMMIT, PAGE_READWRITE);
	
	HANDLERS_OPEN();
	{
		ZeroMemory(Handlers, hSize);
		handlers = 1;
		Handlers[0].BaseAddress = HeapBlock.BaseAddress;
		Handlers[0].TotalSize = HeapBlock.TotalSize;
		Handlers[0].Next = NULL;
		Handlers[0].Prev = NULL;
		Handlers[0].SubNextA = NULL;
		Handlers[0].SubNextS = NULL;
		Handlers[0].SubPrevS = NULL;
		Handlers[0].SubPrevA = NULL;
		Handlers[0].isFree = true;
		FMMADDHASHS(Handlers);
	}
	HANDLERS_CLOSE();

	isInit = true;
	return true;
}
//-----------------------------------------------------------------------------------
bool FMManager::Release(void)
{
	if(!isInit) 
		return false;
	FMM_LOG_CLOSE();
	return !!VirtualFree((void*)HeapBlock.BaseAddress, 0, MEM_RELEASE);
}
//-----------------------------------------------------------------------------------
// Logging routines
//-----------------------------------------------------------------------------------
bool FMManager::FMM_LOG_OPEN(void)
{
	#ifdef FMM_LOGGING_ENABLE
	if((logfile = fopen(FMM_LOGFILENAME, "wt")) == NULL)
	{
		MessageBox(NULL, FMM_LOGFILENAME, "Cannot open log file", MB_OK);
		ExitProcess(-1);
	}
	#endif
	return true;
}
//-----------------------------------------------------------------------------------
bool FMManager::FMM_LOG_CLOSE(void)
{
	#ifdef FMM_LOGGING_ENABLE
	if(!logfile)
		return false;
	fclose(logfile);
	#endif
	return true;
}
//-----------------------------------------------------------------------------------
void FMManager::FMM_ERROR(char* str, ...) 
{
	if(!str)	return;
	static char buf[1024];

	va_list args;
	va_start(args, str);
	sprintf(buf, str, args);
	va_end(args);
	MessageBox(NULL, buf, "FMMANAGER ERROR!", MB_OK);
}
//-----------------------------------------------------------------------------------
void FMManager::FMM_LOG(char* str, ...) 
{
	#ifdef FMM_LOGGING_ENABLE
	if(!str)	return;

	va_list args;
	va_start(args, str);
	vfprintf(logfile, str, args);
	vprintf(str, args);
	fflush(logfile);
	va_end(args);
	#endif
}
//-----------------------------------------------------------------------------------
// Allocation routines
//-----------------------------------------------------------------------------------
FMManager::FMMHANDLER* FMManager::GetFreeHandler(void)
{
	FMMHANDLER* h = NULL;
	if(InvalidHandlers)
	{
		h = InvalidHandlers;
		InvalidHandlers = InvalidHandlers->Next;
	}
	else
	{
		if(handlers >= FMM_MAXHANDLERS)
		{
			FMM_ERROR("There are too much handlers: %u!\n", handlers);
			return NULL;
		}
		h = &Handlers[handlers++];
		h->r2 = handlers-1;
	}
	return h;
}
/*-----------------------------------------------------------------------------------
	��������� ������ ���������� ����� ���������� ������ ����������� ������� � ��������
������� ��������, ������� � �������, ����������� �� �������������� �������.
	�� ����������, ����� ��������� �� ������� ��������, � ������� �� ��� ������, �
������, ����� ��� ������ �� ����� ������ � ������. ������ (�����) ����� ���������� 
����������, � �� ���������� � ������� �������. ������ (������) ����� ���������� ������
� ���������� � ������� ��������.
/----------------------------------------------------------------------------------*/
FMManager::FMMHANDLER* FMManager::AddHandler(unsigned size)
{
	assert(handlers);

	FMMHANDLER* _h = NULL;
	FMMHANDLER* h = NULL;

	unsigned chain_idx = 0;

	// - ���� ����� ����� ������������ �������
	for(unsigned i = FMMHASHS(size); i < FMM_MAXHASHITEMS; i++)
	{
		chain_idx = 0;
		_h = HashTableS[i];
		while(_h)
		{
			// - ������ �������, ������ �� ������� HashTableS, ��� ������ ������
			if(_h->TotalSize >= size)
			{
				h = _h;
				break;
			}
			_h = _h->SubNextS;
			chain_idx++;
		}
		if(h) break;
	}
	if(!h)
	{
		FMM_LOG("Cannot find block of %u bytes size!\n", size);
		return NULL;
	}
	// - ��������� ������� ��������� ����� �� ������� ��������
	DELETE_HS(h);

	if(h->TotalSize > size)
	{
		// - ������� ��� ���� �����, ������� ����� ��������������� ������� �����
		_h = GetFreeHandler();
		if(!_h) return NULL;
	
		_h->BaseAddress = h->BaseAddress + size;
		_h->TotalSize = h->TotalSize - size;
		_h->isFree = true;
		h->TotalSize = size;

		FMMHANDLER* h0 = h;
		FMMHANDLER* h1 = _h;
		FMMHANDLER* h2 = h->Next;
		
		h0->Next = h1;
		h1->Next = h2;
		h1->Prev = h0;
		if(h2)
			h2->Prev = h1;

		// - �������� ������ ����� � ������� ��������
		FMMADDHASHS(_h);
	}
	// - �������� ������� ����� � ������� �������
	FMMADDHASHA(h);

	return h;
}
//-----------------------------------------------------------------------------------
FMPTR FMManager::Allocate(int size)
{
	if(!isInit && !Initialize()) return NULL;
	if(!size)
	{
		FMM_LOG("Cannot allocate 0 bytes!\n");
		return NULL;
	}
	if(HeapBlock.AllocatedSize + size > HeapBlock.TotalSize) 
	{
		FMM_ERROR("Cannot allocate %u bytes! No free memory!\n", size);
		FMM_ERROR("Memory allocated %u\n", HeapBlock.AllocatedSize);
		return NULL;
	}
	size = (size+0x0f)&~0x0f;

	FMMHANDLER* h = AddHandler(size);
	if(!h) return NULL;
	
	FMPTR addr1 = h->BaseAddress;
	FMPTR addr2 = addr1 + h->TotalSize;

	if(HeapEnd < addr2) HeapEnd = addr2;
	HeapBlock.AllocatedSize += size;

	SetAccess(addr1, addr2-addr1, PAGE_READWRITE);
	
	FMM_LOG("%u bytes allocated\n", size);
	return h->BaseAddress;
}
/*-----------------------------------------------------------------------------------
	���������� ���������� ������ ���������� ����� ������ ������ � �������� �������
� ������� ������� �������, ���������� ���-��������� ��������� ������. ��� ���������� 
������, ������ �������������, � ������������ �������� ��������� ������� ������� ������.
��������� ������ ���������� ����������� � ���������� � ������ ���������� �������.
/----------------------------------------------------------------------------------*/
bool FMManager::Deallocate(FMPTR addr)
{
	if(!isInit) return false;

	FMMHANDLER* _h = NULL;
	FMMHANDLER* h = NULL;

	// - ���� �� ������ ����� ������� ������
	unsigned hash_idx = FMMHASHA(addr), chain_idx = 0;

	_h = HashTableA[hash_idx];
	while(_h)
	{
		if(_h->BaseAddress == addr)
		{
			h = _h;
			break;
		}
		_h = _h->SubNextA;
		chain_idx++;
	}
	if(!h)
	{
		FMM_LOG("Cannot find block with address 0x%08x!\n", addr);
		return false;
	}
	FMM_LOG("Deallocated %u bytes from address 0x%08x\n", h->TotalSize, addr);

	// - ��������� ������� ��������� ����� �� ������� �������
	DELETE_HA(h);

	// - ��������� ������ � ��������� �������� �������
	FMPTR addr1 = h->BaseAddress;
	FMPTR addr2 = addr1 + h->TotalSize;

	if(HeapEnd == addr2) HeapEnd = addr1;
	HeapBlock.AllocatedSize -= h->TotalSize;

	FMMHANDLER* h0 = h->Prev;
	FMMHANDLER* h1 = h;
	FMMHANDLER* h2 = h->Next;

	// - ����� ������ ���������� ��������
	int page1 = (int)((addr1 - HeapBlock.BaseAddress)/PageSize);
	// - ����� ��������� ���������� ��������
	// - �������� 1, (���� > 0) ��� ����, ����� �������� ���������� ����� ��������
	int page2 = (int)((addr2 - HeapBlock.BaseAddress - 1)/PageSize);

	FMMHANDLER* hPrev = h0;
	FMMHANDLER* hNext = h2;
	bool isLUse = false;
	bool isRUse = false;
	while(hPrev)
	{
		// - ������� ����� ��������, ��������� ��� ������ �����
		int page = (int)(hPrev->BaseAddress + hPrev->TotalSize - 
				  			  HeapBlock.BaseAddress - 1)/PageSize;
		// - ���� ����� �� ������� �����	��������, �������
		if(page < page1) break;
		// - ���� �� ���� �������� ���� �������� ����, �� �������� ������
		if(!hPrev->isFree) 
		{
			isLUse = true;
			break;
		}
		hPrev = hPrev->Prev;
	}
	while(hNext)
	{
		// - ������� ����� ��������, ������ ��� ������� �����
		int page = (int)(hNext->BaseAddress - HeapBlock.BaseAddress)/PageSize;
		// - ���� ����� �� ������� ������ ��������, �������
		if(page > page2) break;
		// - ���� �� ���� �������� ���� �������� ����, �� �������� ������
		if(!hNext->isFree) 
		{
			isRUse = true;
			break;
		}
		hNext = hNext->Next;
	}

	if(isLUse)
		page1++;
	if(isRUse)
		page2--;
/*
	if(page1 >= 0 && page2 >= page1)
	{
		// - ���� �� �������� ������ � ������, ���� ���� ���������� ���� ���� ��������!
		if(!VirtualFree((void*)(page1*PageSize + HeapBlock.BaseAddress), 
														(page2-page1)*PageSize+1, MEM_DECOMMIT))
		{
			FMM_MESSAGE("Cannot decommit pages from %u to %u!\n", page1, page2);
			return false;
		}
	}
*/
	if(h0 && h0->isFree)
	{
		// - ������� ���������� ����� (Prev)
		h1->TotalSize += h0->TotalSize;
		h1->BaseAddress = h0->BaseAddress;

		DELETE_HS(h0);
		DELETE_H(h0);
	}
	if(h2 && h2->isFree)
	{
		// - ������� ��������� ����� (Next)
		h1->TotalSize += h2->TotalSize;

		DELETE_HS(h2);
		DELETE_H(h2);
	}
	// - ��������� ����� � ������� ��������
	FMMADDHASHS(h1);

	return true;
}
//-----------------------------------------------------------------------------------
// Access routines
//-----------------------------------------------------------------------------------
bool FMManager::SetAccess(FMPTR base, unsigned size, unsigned mode)
{
	if(!VirtualAlloc(base, size, MEM_COMMIT, mode))
	{
		FMM_ERROR("Cannot commit memory at 0x%08x size 0x%08x!\n", base, size);
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------
bool FMManager::GetInfo(FMMINFO* m)
{
	m->AllocatedSize = HeapBlock.AllocatedSize;
	m->AllHandlers = handlers;
	m->AllocatedHandlers = 0;
	m->FreeHandlers = 0;

	unsigned i;
	for(i = 0; i < handlers; i++)
		Handlers[i].r2 = 0;

	for(i = 0; i < FMM_MAXHASHITEMS; i++)
	{
		FMMHANDLER* h = HashTableA[i];
		unsigned chain_idx = 0;
		while(h)
		{
			if(h->r2)
			{
				FMM_LOG("Cycling detected at HashTableA[%u] in the element number %u!\n", i, chain_idx);
				return false;
			}
			h->r2 = 1;
			m->AllocatedHandlers++;
			if(m->AllocatedHandlers > handlers)
			{
				FMM_LOG("Cycling detected at HashTableA[%u]!\n", i);
				return false;
			}
			chain_idx++;
			h = h->SubNextA;
		}
	}
	
	for(i = 0; i < FMM_MAXHASHITEMS; i++)
	{
		FMMHANDLER* h = HashTableS[i];
		unsigned chain_idx = 0;
		while(h)
		{
			if(h->r2)
			{
				FMM_LOG("Cycling detected at HashTableS[%u] in the element number %u!\n", i, chain_idx);
				return false;
			}
			h->r2 = 1;
			m->FreeHandlers++;
			if(m->FreeHandlers > handlers)
			{
				FMM_LOG("Cycling detected at HashTableS[%u]!\n", i);
				return false;
			}
			chain_idx++;
			h = h->SubNextS;
		}
	}

	m->TotalPages = HeapBlock.TotalPages;
	m->NonePages = 0;
	m->NewPages = 0;
	m->PresentPages = 0;
	m->CashedPages = 0;

	MEMORY_BASIC_INFORMATION Buffer;

	unsigned TotalPages = 0;
	unsigned NonePages = 0;
	unsigned PresentPages = 0;

	unsigned addr1 = (unsigned)HeapBlock.BaseAddress;
	unsigned addr2 = addr1 + HeapBlock.TotalSize;

	while(addr1 < addr2)
	{
		VirtualQuery((void*)addr1, &Buffer, sizeof(MEMORY_BASIC_INFORMATION));
		
		if(Buffer.RegionSize > HeapBlock.TotalSize)
			Buffer.RegionSize = HeapBlock.TotalSize;

		addr1 += Buffer.RegionSize;
		unsigned pages = (Buffer.RegionSize + PageSize-1)/PageSize;
		TotalPages += pages;

		if(Buffer.State == MEM_FREE || Buffer.State == MEM_RESERVE)
			NonePages += pages;
		else
		if(Buffer.State == MEM_COMMIT)
			PresentPages += pages;
	}
	return true;
}
//-----------------------------------------------------------------------------------
bool FMManager::SelfTest(void)
{
	unsigned valid_handlers = 0;
	unsigned invalid_handlers = 0;
	unsigned allocated_handlers = 0;
	unsigned free_handlers = 0;

	FMMHANDLER* start_h = NULL;
	FMMHANDLER* end_h = NULL;
	FMMHANDLER* min_h = NULL;

	FMPTR base = (FMPTR)0xffffffff;
	unsigned start_num = 0;	// - ����� ��������� ���������, ������ ���� 1
	unsigned end_num = 0;	// - ����� �������� ���������, ������ ���� 1

	for(int i = 0; i < handlers; i++)
	{
		if(Handlers[i].BaseAddress <= base && Handlers[i].TotalSize)
		{
			base = Handlers[i].BaseAddress;
			min_h = &Handlers[i];
		}
		if(!Handlers[i].Prev && Handlers[i].TotalSize)
		{
			start_h = &Handlers[i];
			start_num++;
		}
		if(!Handlers[i].Next && Handlers[i].TotalSize)
		{
			end_h = &Handlers[i];
			end_num++;
		}
		Handlers[i].r2 = 0;
	}
	if(start_num != 1) 
	{
		FMM_LOG("ERROR: There are %u start points in global chain!\n", start_num);
		return false;
	}
	if(end_num != 1) 
	{
		FMM_LOG("ERROR: There are %u end points in global chain!\n", end_num);
		return false;
	}
	if(start_h != min_h)
	{
		FMM_LOG("ERROR: The start handler is have not minimum base address!\n");
		return false;
	}

	FMMHANDLER* h1 = start_h;
	while(h1)
	{
		FMMHANDLER* h2 = h1->Next;
		if(h2)
		{
			if(h2->Prev != h1)
			{
				FMM_LOG("ERROR: Global chain is corrupted!\n");
				return false;
			}
		}
		else
		if(h1 != end_h)
		{
			FMM_LOG("ERROR: End handler is have not maximum base address!\n");
			return false;
		}
		h1->r2 = 1;
		h1 = h2;
		valid_handlers++;
	}

	h1 = InvalidHandlers;
	while(h1)
	{
		FMMHANDLER* h2 = h1->Next;
		if(h2)
		{
			if(h2->Prev != h1)
			{
				FMM_LOG("ERROR: Invalid handlers chain is corrupted!\n");
				return false;
			}
		}
		if(h1->r2)
		{
			FMM_LOG("ERROR: There is collision between the global and the invalid handlers chains!\n");
			return false;
		}
		h1 = h2;
		invalid_handlers++;
	}
	if(invalid_handlers + valid_handlers != handlers)
	{
		FMM_LOG("ERROR: There are %u lost handlers!\n", handlers - invalid_handlers - valid_handlers);
		return false;
	}

	// - ������� �������� ���������
	for(int i = 0; i < handlers; i++)
		Handlers[i].r2 = 0;

	// - �������� ������� ��������
	for(int i = 0; i < FMM_MAXHASHITEMS; i++)
	{
		h1 = HashTableS[i];
		while(h1)
		{
			FMMHANDLER* h2 = h1->SubNextS;
			if(h2)
			{
				if(h2->SubPrevS != h1)
				{
					FMM_LOG("ERROR: Sizes chain is corrupted!\n");
					return false;
				}
			}
			if(h1->r2)
			{
				FMM_LOG("ERROR: There is collision in the sizes chain!\n");
				return false;
			}
			h1->r2 = 1;
			h1 = h2;
			free_handlers++;
		}
	}
	// - �������� ������� �������
	for(int i = 0; i < FMM_MAXHASHITEMS; i++)
	{
		h1 = HashTableA[i];
		while(h1)
		{
			FMMHANDLER* h2 = h1->SubNextA;
			if(h2)
			{
				if(h2->SubPrevA != h1)
				{
					FMM_LOG("ERROR: Addresses chain is corrupted!\n");
					return false;
				}
			}
			if(h1->r2)
			{
				FMM_LOG("ERROR: There is collision in the addresses chain!\n");
				return false;
			}
			h1->r2 = 2;
			h1 = h2;
			allocated_handlers++;
		}
	}
	unsigned num = free_handlers + allocated_handlers + invalid_handlers;
	if(num != handlers)
	{
		FMM_LOG("ERROR: There are %u lost handlers!\n", handlers - num);
		return false;
	}
	
	FMM_LOG("Self test is OK!\n");
	return true;
}
//-----------------------------------------------------------------------------------
//#ifdef STARFORCE
//void __declspec(dllexport) SFINIT0_InitAI(){
//	PTR_AI=GlobalAlloc(GMEM_FIXED,4);
//	((DWORD*)PTR_AI)[0]='NFMM';
//};
//#endif