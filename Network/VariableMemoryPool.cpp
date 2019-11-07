#include "StdAfx.h"
#include ".\variablememorypool.h"

VariableMemoryBlock::VariableMemoryBlock()
{
	mIsEOM = FALSE;
	mBasePtr = NULL;
	mPtr = NULL;
	mRemainSize = 0;
}

VariableMemoryBlock::~VariableMemoryBlock()
{
}

void VariableMemoryBlock::Init( void* ptr, size_t size )
{
	mBasePtr = ptr;
	mPtr = (char*)ptr;
	mRemainSize = size;
}

void* VariableMemoryBlock::Alloc( size_t size )
{
	if( mRemainSize < size )
	{
		mIsEOM = TRUE;
		return NULL;
	}

	void* ptr = (void*)mPtr;
	mMemoryBlockContainer.insert( MemoryBlockContainer::value_type( ptr, size ) );
	mPtr += size;
	mRemainSize -= size;
	return ptr;
}

BOOL VariableMemoryBlock::Free( void* ptr )
{
	mMemoryBlockContainer.erase( ptr );
    
	return ( mIsEOM == TRUE &&
		mMemoryBlockContainer.empty() );
}



VariableMemoryPool::VariableMemoryPool(void)
{
	InitializeCriticalSection( &mCS );

	AllocMemoryBlock();
}

VariableMemoryPool::~VariableMemoryPool(void)
{
	{
		EnterCS enterCS( mCS );

		for( MemoryContainer::const_iterator memoryIter = mMemoryContainer.begin() ; 
			memoryIter != mMemoryContainer.end() ;
			++memoryIter )
		{
			const void* const ptr = *memoryIter;
			delete ptr;
		}
		mMemoryContainer.clear();
		
		for( VariableMemoryBlockContainer::const_iterator blockIter = mVariableMemoryBlockContainer.begin() ; 
			blockIter != mVariableMemoryBlockContainer.end() ;
			++blockIter )
		{
			const VariableMemoryBlock& block = blockIter->second;
			BOOSTSINGLETONPOOL::free( block.GetBasePtr() );
		}
		mVariableMemoryBlockContainer.clear();

		mInnerMemoryBlockContainer.clear();
	}

	DeleteCriticalSection( &mCS );
}

VariableMemoryPool& VariableMemoryPool::GetInstance()
{
	static VariableMemoryPool instance;
	return instance;
}

void VariableMemoryPool::AllocMemoryBlock()
{
	mCurMemory = BOOSTSINGLETONPOOL::malloc();
	VariableMemoryBlock& block = mVariableMemoryBlockContainer[ mCurMemory ];
	block.Init( mCurMemory, MAX_MEMORY_BLOCK );
}

void VariableMemoryPool::FreeLargeBlock( void* ptr )
{
	MemoryContainer::iterator memoryIter = mMemoryContainer.find( ptr );
	if( memoryIter == mMemoryContainer.end() )
	{
		return;
	}

	delete ptr;

	mMemoryContainer.erase( memoryIter );
}

void* VariableMemoryPool::Alloc( size_t size )
{
	EnterCS enterCS( mCS );

	if( size > MAX_MEMORY_BLOCK )
	{
		void* ptr = new char[ size ];
		mMemoryContainer.insert( ptr );
		return ptr;
	}

	VariableMemoryBlock& block = mVariableMemoryBlockContainer[ mCurMemory ];
	void* ptr = block.Alloc( size );
	if( ptr == NULL )
	{
		AllocMemoryBlock();
		VariableMemoryBlock& block = mVariableMemoryBlockContainer[ mCurMemory ];
		ptr = block.Alloc( size );
	}

	mInnerMemoryBlockContainer.insert( InnerMemoryBlockContainer::value_type( ptr, mCurMemory  ) );

	return ptr;
}

void VariableMemoryPool::Free( void* ptr )
{
	EnterCS enterCS( mCS );

	InnerMemoryBlockContainer::iterator InnerMemoryBlockIter = mInnerMemoryBlockContainer.find( ptr );
	if( InnerMemoryBlockIter == mInnerMemoryBlockContainer.end() )
	{
		FreeLargeBlock( ptr );
		return;
	}

	const void* baseMemory = InnerMemoryBlockIter->second;

	VariableMemoryBlockContainer::iterator blockIter = mVariableMemoryBlockContainer.find( baseMemory );
	ASSERT( blockIter != mVariableMemoryBlockContainer.end() );

	VariableMemoryBlock& block = blockIter->second;
	if( block.Free( ptr ) == TRUE )
	{
		BOOSTSINGLETONPOOL::free( block.GetBasePtr() );
		mVariableMemoryBlockContainer.erase( blockIter );
	}

	mInnerMemoryBlockContainer.erase( InnerMemoryBlockIter );
}