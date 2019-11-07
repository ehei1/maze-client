#pragma once

namespace Utility
{
	template< class DeletingClass >
	void SafeDelete(DeletingClass*& deletingClass)
	{
		if(nullptr != deletingClass)
		{
			delete deletingClass;
			deletingClass = nullptr;
		}
	}

	// 클래스가 소멸될 때, 참조로 받은 변수에 예약된 값을 설정해준다
	// 주의: 클래스가 소멸될 시점에 이미 변수가 파괴된 경우는 예상하지 않는다
	template< class FlagType, class ValueType >
	class AutoFlagger
	{
	public:
		AutoFlagger(FlagType& value) :
		mValue(value)
		{}

		virtual ~AutoFlagger()
		{
			mValue = mReservedValue;
		}

		void Reserve(ValueType value)
		{
			mReservedValue = value;
		}

	private:
		FlagType& mValue;
		ValueType mReservedValue;
	};

	void OutputDebug(const wchar_t*, ...);
	void OutputDebug(const char*, ...);

	Ogre::Vector3 GetDirection(const Ogre::SceneNode&);

	bool ProjectPosition(
		const Ogre::Camera&,
		const Ogre::Vector3& position,
		Ogre::Vector2&);

	// LUJ, Y축을 기준으로 점을 회전시킨다
	void RotateXZ(
		Ogre::Real& x,
		Ogre::Real& z,
		const Ogre::Radian&,
		const Ogre::Vector3& axis,
		const Ogre::Vector3& position);

	bool IsMouseOnGui();

	int GetUniqueIndex();
}