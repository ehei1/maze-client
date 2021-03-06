/*
	OgreNewt library - connecting Ogre and Newton!

	Demo09_PlayerController
*/
#include "OgreNewtonApplication.h"
#include "OgreNewtonFrameListener.h"

#include <OgreNewt.h>
#include <OgreNewt_BasicFrameListener.h>


OgreNewtonApplication::OgreNewtonApplication(void)
{
	// create OgreNewt world.
	m_World = new OgreNewt::World();
    mPlayer = NULL;

	mEntityCount = 0;

}

OgreNewtonApplication::~OgreNewtonApplication(void)
{
	// destroy world.
    if( mPlayer )
        delete mPlayer;
	delete m_World;
}



void OgreNewtonApplication::createScene()
{

	// setup CEGUI
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer( mWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr );
	new CEGUI::System( mGUIRenderer );

	// load up CEGUI stuff...
	try
	{
		using namespace CEGUI;
		CEGUI::Logger::getSingleton().setLoggingLevel( CEGUI::Informative );

		CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
        CEGUI::System::getSingleton().setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
        CEGUI::System::getSingleton().setDefaultFont((CEGUI::utf8*)"BlueHighway-10");

		CEGUI::Window* sheet = CEGUI::WindowManager::getSingleton().createWindow( (CEGUI::utf8*)"DefaultWindow", (CEGUI::utf8*)"root_wnd" );
		CEGUI::System::getSingleton().setGUISheet( sheet );

		//makeGUI();
		//setupGUI();
	
	}
	catch (CEGUI::Exception)
	{}


	// sky box.
	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
	
	// shadows on!
	mSceneMgr->setShadowTechnique( Ogre::SHADOWTYPE_STENCIL_ADDITIVE );


	// floor object!
	Entity* floor;
	SceneNode* floornode;
	floor = mSceneMgr->createEntity("Floor", "simple_terrain.mesh" );
	floornode = mSceneMgr->getRootSceneNode()->createChildSceneNode( "FloorNode" );
	floornode->attachObject( floor );
	floor->setMaterialName( "Simple/BeachStones" );
	floor->setCastShadows( false );

	


	//Ogre::Vector3 siz(100.0, 10.0, 100.0);
	OgreNewt::CollisionPtr col = OgreNewt::CollisionPtr(new OgreNewt::CollisionPrimitives::TreeCollision( m_World, floor, true, 0 ));
	OgreNewt::Body* bod = new OgreNewt::Body( m_World, col );
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
	delete col;
#endif
	
	//floornode->setScale( siz );
	bod->attachNode( floornode );
	bod->setPositionOrientation( Ogre::Vector3(0.0,-10.0,0.0), Ogre::Quaternion::IDENTITY );


	// make a simple rope.
	Ogre::Vector3 size(3,1.0,1.0);
	Ogre::Vector3 pos(0,1,0);
	Ogre::Quaternion orient = Ogre::Quaternion::IDENTITY;

	// loop through, making bodies and connecting them.
	OgreNewt::Body* parent = NULL;
	OgreNewt::Body* child = NULL;

	for (int x=0;x<8;x++)
	{
		// make the next box.
		child = makeSimpleBox(size, pos, orient);

		// now make a new joint connecting this to the last box.
		OgreNewt::Joint* joint;

		// make the joint right between the bodies...
		

		if (parent)
		{
			joint = new OgreNewt::BasicJoints::BallAndSocket( m_World, child, parent, pos-Ogre::Vector3(size.x/2,0,0) );
		}
		else
		{
			// no parent, this is the first joint, so just pass NULL as the parent, to stick it to the "world"
			joint = new OgreNewt::BasicJoints::BallAndSocket( m_World, child, NULL, pos-Ogre::Vector3(size.x/2,0,0) );
		}

		// offset pos a little more.
		pos += Ogre::Vector3(size.x,0,0);

		// save the last body for the next loop!
		parent = child;
	}

	for (int i=0; i<15;i++)
	{
		pos = Ogre::Vector3( 10-rand()%20, 4+rand()%2, 10-rand()%20 );
		size = Ogre::Vector3( 1+rand()%3, 1+rand()%3, 1+rand()%3 );

		OgreNewt::Body* bod = makeSimpleBox( size, pos, orient );
	}


    // player playground

    bod = NULL;
    for( int i = 0; i < 20; i++ )
    {
        bod = makeSimpleBox( Ogre::Vector3(4.0, 0.4, 1.0), Ogre::Vector3(15.0, -10.0+i*0.4, -2.0-i*0.4), Ogre::Quaternion::IDENTITY);
        bod->setMassMatrix(0, Ogre::Vector3::ZERO);
    }

    bod = makeSimpleBox( Ogre::Vector3(10.0, 0.4, 10.0), Ogre::Vector3(-17.0, -8.0, -15.0), Ogre::Quaternion(Ogre::Degree(60), Ogre::Vector3::UNIT_X));
    bod->setMassMatrix(0, Ogre::Vector3::ZERO);

    bod = makeSimpleBox( Ogre::Vector3(10.0, 0.4, 10.0), Ogre::Vector3(-27.0, -8.5, -15.0), Ogre::Quaternion(Ogre::Degree(20), Ogre::Vector3::UNIT_X));
    bod->setMassMatrix(0, Ogre::Vector3::ZERO);
    
    bod = makeSimpleBox( Ogre::Vector3(10.0, 0.4, 10.0), Ogre::Vector3(-37.0, -8.0, -15.0), Ogre::Quaternion(Ogre::Degree(80), Ogre::Vector3::UNIT_Y) * Ogre::Quaternion(Ogre::Degree(50), Ogre::Vector3::UNIT_X));
    bod->setMassMatrix(0, Ogre::Vector3::ZERO);

    makePlayer();

	
	// position camera
	mCamera->setPosition(0.0, -3.0, 23.0);

	//make a light
	Ogre::Light* light;

	light = mSceneMgr->createLight( "Light1" );
	light->setType( Ogre::Light::LT_POINT );
	light->setPosition( Ogre::Vector3(0.0, 100.0, 100.0) );



}


void OgreNewtonApplication::createFrameListener()
{
	mFrameListener = new OgreNewtonFrameListener( mWindow, mCamera, mSceneMgr, m_World, mPlayer);
	mRoot->addFrameListener(mFrameListener);

	mNewtonListener = new OgreNewt::BasicFrameListener( mWindow, m_World, 60 );
	mRoot->addFrameListener(mNewtonListener);
}

void OgreNewtonApplication::destroyScene()
{
	CEGUI::System* sys = CEGUI::System::getSingletonPtr();
	delete sys;

	// CEGUI Cleanup
	delete mGUIRenderer;
}


OgreNewt::Body* OgreNewtonApplication::makeSimpleBox( const Ogre::Vector3& size, const Ogre::Vector3& pos, const Ogre::Quaternion& orient )
{
	Entity* box1;
	SceneNode* box1node;

	box1 = mSceneMgr->createEntity( "Entity"+Ogre::StringConverter::toString(mEntityCount++), "box.mesh" );
	box1node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	box1node->attachObject( box1 );
	box1node->setScale( size );

	OgreNewt::ConvexCollisionPtr col = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::Box( m_World, size, 0 ));
	OgreNewt::Body* bod = new OgreNewt::Body( m_World, col );


	// base mass on the size of the object.
	Ogre::Real mass = size.x * size.y * size.z * 2.5;
	// calculate the inertia based on box formula and mass
	Ogre::Vector3 inertia, offset;
    col->calculateInertialMatrix(inertia, offset);

#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
	delete col;
#endif
				
	bod->attachNode( box1node );
	bod->setMassMatrix( mass, mass*inertia );
    bod->setCenterOfMass(offset);
	bod->setStandardForceCallback();

	box1->setMaterialName( "Simple/BumpyMetal" );


	bod->setPositionOrientation( pos, orient );

	return bod;
}

void OgreNewtonApplication::makePlayer()
{
    Entity* ellipsoid;
    SceneNode* node;

    ellipsoid = mSceneMgr->createEntity( "PlayerControllerEntity", "ellipsoid.mesh");
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject( ellipsoid );
    node->setScale(Ogre::Vector3(1,2.5,1));
    
    OgreNewt::ConvexCollisionPtr col = OgreNewt::ConvexCollisionPtr(new OgreNewt::CollisionPrimitives::Ellipsoid( m_World, Ogre::Vector3(1,2.5,1), 0 ));
    OgreNewt::Body* bod = new OgreNewt::Body( m_World, col );
    Ogre::Vector3 inertia, offset;
    col->calculateInertialMatrix(inertia, offset);
#ifdef OGRENEWT_NO_COLLISION_SHAREDPTR
	delete col;
#endif
    bod->attachNode(node);
    bod->setMassMatrix( 50, 50*inertia );
    bod->setCenterOfMass(offset);
    bod->setStandardForceCallback();

    ellipsoid->setMaterialName("Simple/dirt01");

    bod->setPositionOrientation(Ogre::Vector3(0,20,20), Ogre::Quaternion::IDENTITY);

    mPlayer = new OgreNewt::PlayerController(bod, 0.4);
}

