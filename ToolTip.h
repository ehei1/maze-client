#pragma once

class Tooltip
{
public:
	Tooltip(void);
	Tooltip(Tooltip&);

	virtual ~Tooltip(void);
	void AddCaption(
		const Ogre::ColourValue& topColour,
		const Ogre::ColourValue& bottomColour,
		LPCTSTR, ...);
	void Clear();
	void SetPosition(const Ogre::Vector3&);
	void SetVisible(bool);
	bool IsVisible() const { return mIsVisible; }


private:
	Ogre::OverlayContainer* mOverlayContainer;
	Ogre::Overlay* mOverlay;

	bool mIsVisible;
};

