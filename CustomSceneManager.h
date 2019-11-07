/*
Hardware Occlusion Query(HOQ)

http://www.ogre3d.org/tikiwiki/Hardware+Occlusion+Query&structure=Cookbook
*/
#pragma once


#include <OgreSceneManagerEnumerator.h>


class CustomSceneManagerFactory : public Ogre::SceneManagerFactory
{
protected:
    void initMetaData(void) const;
public:
    CustomSceneManagerFactory() {}
    ~CustomSceneManagerFactory() {}
    /// Factory type name
    static const Ogre::String FACTORY_TYPE_NAME;
    Ogre::SceneManager* createInstance(const Ogre::String& instanceName);
    void destroyInstance(Ogre::SceneManager* instance);
};


class CustomSceneManager : public Ogre::SceneManager
{
public:
    CustomSceneManager(const Ogre::String& name);
    ~CustomSceneManager();
    const Ogre::String& getTypeName(void) const;

    /** Internal utility method for rendering a single object. 
    @remarks
        Assumes that the pass has already been set up.
    @param rend The renderable to issue to the pipeline
    @param pass The pass which is being used
    @param doLightIteration If true, this method will issue the renderable to
        the pipeline possibly multiple times, if the pass indicates it should be
        done once per light
    @param manualLightList Only applicable if doLightIteration is false, this
        method allows you to pass in a previously determined set of lights
        which will be used for a single render of this object.
    */
    virtual void renderSingleObject(Ogre::Renderable* rend, const Ogre::Pass* pass,
		bool lightScissoringClipping, bool doLightIteration, const Ogre::LightList* manualLightList = 0);
};


class RenderableOcclusionPair
{
public:
    RenderableOcclusionPair(Ogre::Renderable* _renderable);
    ~RenderableOcclusionPair();

    /// Returns the pixel count, and changes the state
    /// to continue the next hardware query
    unsigned int GetPixelCount();

    /// Returns the renderable to query
    Ogre::Renderable* GetRenderable();

    /// Start the query, if the state
    /// is ready to render.
    void BeginOcclusionQuery();

    /// Finish the query, if the state
    /// is ready to render. Changes
    /// the state to wait for access
    void EndOcclusionQuery();


protected:
    Ogre::HardwareOcclusionQuery* m_HardwareOcclusionQuery;
	Ogre::Renderable* m_Renderable;

    unsigned int m_PixelCount;
 
    enum PixelState
    {
        READY_FOR_RENDER,
        QUERY_STARTED,
        READY_FOR_ACCESS,
        __ENUM_MAX__ // must be last
    };
 
    PixelState m_PixelState;
};