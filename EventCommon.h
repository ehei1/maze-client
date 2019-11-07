#ifndef _COMMONEVENTPROCESS_
#define _COMMONEVENTPROCESS_

typedef size_t TransactionIndex;


enum EventType
{
	EventNone,
	EventMoveForward,
	EventMoveLeftward,
	EventMoveRightward,
	EventMoveBackward,
	EventMoveStop,
	EventChangeDirection,
	EventUseHand,
};


enum HandType
{
	HandNone,
	HandLeft,
	HandRight,
};


#endif _COMMONEVENTPROCESS_