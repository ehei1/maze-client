#pragma once


class Body;
class Command;
class Event;

enum ProcessType;
enum EventType;


class Processor
{
public:
	virtual ~Processor();

	virtual bool Run(const Ogre::FrameEvent&, Body&);
	virtual bool Receive(const Event&, Body&) = 0;

	inline ProcessType GetType() const { return mType; }

	void Add(Command*);
	void Remove(EventType, Body&);
	bool Update(const Event&, Body&);


protected:
	Processor(ProcessType);


private:
	const ProcessType mType;

	typedef std::list< Command* > CommandContainer;
	CommandContainer mRunningCommandContainer;
};