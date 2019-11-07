#pragma once

class PacketDataOperator
{
	void* mMemory;
	char* mCurPtr;
	size_t mMemorySize;
	size_t mMoveSize;

	// End Of Memory
	bool mIsEOM;
public:
	PacketDataOperator(void* memoryblock, size_t memoryblockSize) {
		mMemory = memoryblock;
		mCurPtr = static_cast<char*>(memoryblock);
		mMemorySize = memoryblockSize;
		mMoveSize = 0;
	}
	~PacketDataOperator(void) {}

	void ResetData();

	template <class RetV>
	RetV ReadData() {
		RetV data;
		const size_t dataSize = sizeof(RetV);
		ReadData( &data, dataSize );
		return data;
	}

	template <class T>
	bool ReadData( T& data ) {
		const size_t dataSize = sizeof(T);
		
		return ReadData( &data, dataSize );
	}

	bool ReadData( void* memory, size_t memorySize ) {
		if( mMoveSize + memorySize > mMemorySize )
		{
			mIsEOM = true;
			return false;
		}

		memcpy( memory, mCurPtr, memorySize );
		mCurPtr += memorySize;
		mMoveSize += memorySize;

		return true;
	}

	template <class T>
	bool WriteData( const T& data )	{
		const size_t dataSize = sizeof(data);
		return WriteData( &data, dataSize );
	}
	bool WriteData( void* memory, size_t memorySize ) {
		if( mMoveSize + memorySize > mMemorySize )
		{
			mIsEOM = true;
			return false;
		}

		memcpy( mCurPtr, &memory, memorySize );
		mCurPtr += memorySize;
		mMoveSize += memorySize;

		return true;
	}

	bool MovePtr( const size_t moveSize ) {
		if( mMoveSize + moveSize > mMemorySize )
		{
			mIsEOM = true;
			return false;
		}

		mCurPtr += moveSize;
		mMoveSize += moveSize;

		return true;
	}

	void* GetMemoryPtr() const { return mMemory; }
	void* GetCurPtr() { return mCurPtr; }
	size_t GetMemorySize() const { return mMemorySize; }
	size_t GetPacketSize() const { return mMoveSize; }
	size_t GetRemainMemorySize() const { return mMemorySize - mMoveSize; }
};
