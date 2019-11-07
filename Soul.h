#pragma once


class Body;
class Event;


class Soul
{
public:
	Soul(void);
	virtual ~Soul(void);

	void AddEvent(Event*);

	inline int GetIndex() const { return mIndex; }

	void FrameRendering();
	void Run(const Body&, const Ogre::FrameEvent&);

private:
	Body* mBody;

	int mIndex;
};