#include "StdAfx.h"
#include ".\IDCreater.h"

CIDCreater::CIDCreater(void) :
DEFAULT_MAX_ID(0xffffffffUL)
{
	m_bMadeALLIDs = false;

	m_StartID				= 1;
	m_EndID					= DEFAULT_MAX_ID;

	m_MaxMadeID				= 0;

	m_pSetCanGiveIDs		= &m_1setIDs;
	m_pSetReturnedIDs		= &m_2setIDs;
}

CIDCreater::CIDCreater(unsigned long maxID) :
DEFAULT_MAX_ID(0xffffffffUL)
{
	m_bMadeALLIDs			= false;

	m_StartID				= 1;
	m_EndID					= maxID;

	m_MaxMadeID				= 0;

	m_pSetCanGiveIDs		= &m_1setIDs;
	m_pSetReturnedIDs		= &m_2setIDs;
}

CIDCreater::CIDCreater(unsigned long startID, unsigned long maxID) :
DEFAULT_MAX_ID(0xffffffffUL)
{
	Init( startID, maxID );
}


CIDCreater::~CIDCreater(void)
{
}

void CIDCreater::Init(unsigned long startID, unsigned long maxID)
{
	m_bMadeALLIDs			= false;

	if( startID > maxID )
	{
		m_StartID				= 1;
		m_EndID					= DEFAULT_MAX_ID;

		m_MaxMadeID				= 0;
	}
	else
	{
		m_StartID				= startID;
		m_EndID					= maxID;

		m_MaxMadeID				= startID - 1;
	}

	m_1setIDs.clear();
	m_2setIDs.clear();

	m_pSetCanGiveIDs		= &m_1setIDs;
	m_pSetReturnedIDs		= &m_2setIDs;
}

const unsigned long CIDCreater::GetNewID()
{
	if( !m_bMadeALLIDs )
	{
		if( ++m_MaxMadeID >= m_EndID )
			m_bMadeALLIDs = true;

		return m_MaxMadeID;
	}

	if( m_pSetCanGiveIDs->empty() )
	{
		if( m_pSetReturnedIDs->empty() )
		{
			ASSERT(0);
			return (unsigned long)-1;
		}

		std::set< unsigned long >* tmp = m_pSetReturnedIDs;
		m_pSetReturnedIDs = m_pSetCanGiveIDs;
		m_pSetCanGiveIDs = tmp;
	}

	std::set< unsigned long >::iterator iter = m_pSetCanGiveIDs->begin();
	const unsigned long newID = *iter;
	m_pSetCanGiveIDs->erase( iter );

	return newID;
}

void CIDCreater::ReturnID( const unsigned long id )
{
	m_pSetReturnedIDs->insert( id );
}

bool CIDCreater::CanMakeID() const
{
	if( m_bMadeALLIDs && m_pSetCanGiveIDs->empty() && m_pSetReturnedIDs->empty() )
		return false;

	return true;
}