#pragma once


#include "StdAfx.h"


enum EventType;


class Event
{
public:
	typedef Ogre::String ParameterKey;
	typedef Ogre::String ParameterValue;


private:
	EventType mType;

	typedef std::map< ParameterKey, Ogre::Any > AnyParameterContainer;
	AnyParameterContainer mAnyParameterContainer;


public:
	Event(EventType);
	~Event();

	inline EventType GetType() const { return mType; }

	template< typename Value >
	const Value& GetValue(const ParameterKey& parameterKey) const
	{
		auto iterator = mAnyParameterContainer.find(parameterKey);

		if(mAnyParameterContainer.end() != iterator)
		{
			const Ogre::Any& any = iterator->second;

			if(typeid(Value) == any.getType())
			{
				const Value& value = Ogre::any_cast< Value >(any);

				return value;
			}
		}

		const static Value emptyValue = Value();

		return emptyValue;
	}

	template< typename ValueType >
	void GetValue(ValueType& value, const ParameterKey& parameterKey) const
	{
		auto iterator = mAnyParameterContainer.find(parameterKey);

		if(mAnyParameterContainer.end() != iterator)
		{
			const Ogre::Any& any = iterator->second;

			if(typeid(ValueType) == any.getType())
			{
				value = Ogre::any_cast< ValueType >(any);
				return;
			}
		}

		value = ValueType();
	}

	template< class Value >
	inline void SetValue(const ParameterKey& key, const Value& value)
	{
		mAnyParameterContainer.insert(AnyParameterContainer::value_type(key, Ogre::Any(value)));
	}
};

