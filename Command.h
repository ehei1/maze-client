#pragma once


class Body;
class Event;

enum EventType;


class Command
{
public:
	virtual ~Command();

	virtual void Initialize(Body&) = 0;
	virtual bool Run(const Ogre::FrameEvent&, Body&) = 0;
	virtual void Release(Body&) = 0;
	virtual void SetValue(const Event&) = 0;

	inline EventType GetType() const { return mType; }
	inline void Stop() { mIsStop = true; }
	inline bool IsStop() const { return mIsStop; }


protected:
	Command(const Event&);


private:
	const EventType mType;
	bool mIsStop;
};


