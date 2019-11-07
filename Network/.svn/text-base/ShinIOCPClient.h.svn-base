#pragma once

#include <windows.h>
#include <winsock2.h>
#include <map>
#include "boost/pool/pool_alloc.hpp"
#include "VariableMemoryPool.h"

// IOCP클라이언트 정보를 담기위한 클래스
class CShinIOCPClient
{
	unsigned long		m_connectionIdx;		// 접속 Index
	SOCKET				m_socketClient;

	typedef std::map< void*, size_t, std::less<void*>, boost::fast_pool_allocator< size_t, boost::default_user_allocator_new_delete > > BOOSTALLOCATEDBUFFER;
	BOOSTALLOCATEDBUFFER m_allocatedbuffer;

public:
	CShinIOCPClient(void);
	~CShinIOCPClient(void);

	void Initialize( unsigned long connectIdx );
	void Release();

	inline const unsigned long& GetConnectionIdx() const { return m_connectionIdx; }
	inline SOCKET GetSocket() const { return m_socketClient; }
	void SetSocket( SOCKET socket ) { m_socketClient = socket; }

	void* GetBuffer( size_t size );
	void ReleaseBuffer( void* packet );
	size_t GetBufferSize( void* packet );
};
