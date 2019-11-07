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

	// Ŭ������ �Ҹ�� ��, ������ ���� ������ ����� ���� �������ش�
	// ����: Ŭ������ �Ҹ�� ������ �̹� ������ �ı��� ���� �������� �ʴ´�
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

	// LUJ, Y���� �������� ���� ȸ����Ų��
	void RotateXZ(
		Ogre::Real& x,
		Ogre::Real& z,
		const Ogre::Radian&,
		const Ogre::Vector3& axis,
		const Ogre::Vector3& position);

	bool IsMouseOnGui();

	int GetUniqueIndex();
}