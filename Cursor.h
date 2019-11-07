#pragma once


class Tooltip;
class Body;

namespace Common { enum QueryType; }


class Cursor
{
public:
	Cursor(
		Ogre::SceneManager&,
		Ogre::SceneNode& parentSceneNode,
		const Ogre::Vector2& radius);
	virtual ~Cursor();

	void frameRendering(const Ogre::FrameEvent&);
	void mouseMoved(const OIS::MouseEvent&, const Ogre::Vector3& soulPosition);
	void keyPressed(const OIS::KeyEvent&);
	void keyReleased(const OIS::KeyEvent&);

	inline const Ogre::Vector3& GetPosition() const { return mCursorPosition; }
	inline const Ogre::Vector3& GetMouseCursorPosition() const { return mMouseCursorPosition; }

	Body* GetTooltipBody(Common::QueryType) const;

	inline Ogre::Real GetMaxRadius() const { return std::max(abs(mRadius.x), abs(mRadius.y)); }


private:
	void DrawClosedCircle(const Ogre::FrameEvent&, const Ogre::Vector3&);
	void DrawOpenedCircle(const Ogre::FrameEvent&, const Ogre::Vector3&);
	
	inline Ogre::Real GetRadius() const { return (mRadius.x + mRadiusValue.mBias) * mRadiusValue.mFactor; }
	void SetRadius(const Ogre::Vector2& radius);	

	void AddMaterial(const char* name, const Ogre::ColourValue&);


private:
	const Ogre::Real mAccuracy;
	const char* mEnableMaterialName;
	const char* mDisableMaterialName;

	Ogre::ManualObject* mCursorObject;

	typedef std::pair< boost::shared_ptr< Tooltip >, Ogre::String > TooltipUnit;
	typedef std::list< TooltipUnit > TooltipContainer;
	TooltipContainer mTooltipContainer;

	struct RadiusValue
	{
		Ogre::Real mBias;
		Ogre::Real mFactor;
	}
	mRadiusValue;
	Ogre::Vector2 mRadius;

	enum CircleType
	{
		CircleOpen,
		CircleClose,
	}
	mCircleType;

	Ogre::Vector3 mCursorPosition;
	Ogre::Vector3 mMouseCursorPosition;

	Ogre::MovablePlane mPlane;

	std::auto_ptr< Ogre::AxisAlignedBoxSceneQuery > mSceneQuery;
	std::auto_ptr< Ogre::RaySceneQuery > mRaySceneQuery;
};