#include "StdAfx.h"
#include "ShinIOCPClient.h"

CShinIOCPClient::CShinIOCPClient(void)
{
	m_socketClient = INVALID_SOCKET;
	m_connectionIdx = 0;
}

CShinIOCPClient::~CShinIOCPClient(void)
{
}

void CShinIOCPClient::Initialize( unsigned long connectIdx )
{
	m_connectionIdx = connectIdx;
}

void CShinIOCPClient::Release()
{
	for( BOOSTALLOCATEDBUFFER::const_iterator bufferIter = m_allocatedbuffer.begin() ; bufferIter != m_allocatedbuffer.end() ; ++bufferIter )
	{
		void* buffer = bufferIter->first;
		VariableMemoryPool::GetInstance().Free( buffer );
	}
}

void* CShinIOCPClient::GetBuffer( size_t size )
{
	void* ptr = VariableMemoryPool::GetInstance().Alloc( size );
	m_allocatedbuffer.insert( BOOSTALLOCATEDBUFFER::value_type( ptr, size ) );
	return ptr;
}

void CShinIOCPClient::ReleaseBuffer( void* packet )
{
	m_allocatedbuffer.erase( packet );
	VariableMemoryPool::GetInstance().Free( packet );
}

size_t CShinIOCPClient::GetBufferSize( void* packet )
{
	BOOSTALLOCATEDBUFFER::const_iterator iter = m_allocatedbuffer.find( packet );
	if( iter == m_allocatedbuffer.end() )
	{
		return 0;
	}

	const size_t size = iter->second;
	return size;
}