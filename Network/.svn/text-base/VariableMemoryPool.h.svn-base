#pragma once

#define		USE_MULTI_THREAD
#define		MAX_MEMORY_BLOCK	1024

#include <map>
#include <set>
#include "Boost/pool/singleton_pool.hpp"

struct tagVariableMemory{};

class VariableMemoryBlock
{
	typedef std::map< void*, size_t > MemoryBlockContainer;
	MemoryBlockContainer mMemoryBlockContainer;

	void* mBasePtr;
	char* mPtr;
	size_t mRemainSize;
	BOOL mIsEOM;

public:
	VariableMemoryBlock();
	~VariableMemoryBlock();

	void* GetBasePtr() const { return mBasePtr; }

	void Init( void* ptr, size_t size );
	void* Alloc( size_t size );
	BOOL Free( void* ptr );
};

class VariableMemoryPool
{
	class EnterCS{
		CRITICAL_SECTION& mCS;
	public:
		EnterCS(CRITICAL_SECTION& cs) : mCS(cs){
			#ifdef USE_MULTI_THREAD
				EnterCriticalSection( &mCS );
			#endif
		}
		~EnterCS(){
			#ifdef USE_MULTI_THREAD
				LeaveCriticalSection( &mCS );
			#endif
		}

	};

	typedef boost::singleton_pool< tagVariableMemory, MAX_MEMORY_BLOCK, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex > BOOSTSINGLETONPOOL;

	typedef std::set< void* > MemoryContainer;
	MemoryContainer mMemoryContainer;

	typedef const void* const BaseMemory;
	typedef const void* const InnerMemory;
	typedef std::map< BaseMemory, VariableMemoryBlock > VariableMemoryBlockContainer;
	VariableMemoryBlockContainer mVariableMemoryBlockContainer;
	
	typedef std::map< InnerMemory, BaseMemory > InnerMemoryBlockContainer;
	InnerMemoryBlockContainer mInnerMemoryBlockContainer;

	void* mCurMemory;

	CRITICAL_SECTION mCS;

	void AllocMemoryBlock();
	void FreeLargeBlock( void* ptr );

public:
	VariableMemoryPool(void);
	~VariableMemoryPool(void);

	static VariableMemoryPool& GetInstance();

	void* Alloc( size_t size );
	void Free( void* ptr );
};
