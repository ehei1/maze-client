#pragma once

#include <set>

class CIDCreater
{
	bool							m_bMadeALLIDs;

	unsigned long					m_StartID;
	unsigned long					m_EndID;
	unsigned long					m_MaxMadeID;

	std::set< unsigned long >		m_1setIDs;
	std::set< unsigned long >		m_2setIDs;

	std::set< unsigned long >*		m_pSetCanGiveIDs;
	std::set< unsigned long >*		m_pSetReturnedIDs;

public:
	const unsigned long				DEFAULT_MAX_ID;

	CIDCreater(void);
	CIDCreater(unsigned long maxID);
	CIDCreater(unsigned long startID, unsigned long maxID);
	~CIDCreater(void);

	void Init(unsigned long startID, unsigned long maxID);

	const unsigned long GetNewID();
	void ReturnID( const unsigned long id );

	bool CanMakeID() const;
};
