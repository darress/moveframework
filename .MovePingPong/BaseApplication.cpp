#include "stdafx.h"
#include "BaseApplication.h"

//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{
	mGUI = 0;  
	mPlatform = 0;
}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
	if (mGUI)
	{
		mGUI->shutdown();
		delete mGUI;
		mGUI = 0; 
	}
	 
	if (mPlatform)
	{
		mPlatform->shutdown();
		delete mPlatform;
		mPlatform = 0;
	}

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    // Grab the OpenGL RenderSystem, or exit
	if(mRoot->restoreConfig() || mRoot->showConfigDialog())
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "Ogre Move Test");

        // Let's add a nice window icon
        HWND hwnd;
        mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
        LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
        SetClassLong( hwnd, GCL_HICON, iconID );

		return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,5,15));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,0));
    mCamera->setNearClipDistance(0.05);

    //mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Only add the minimally required resource locations to load up SDKTrays and the Ogre head mesh
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/packs/pingpong.zip", "Zip", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/fonts", "FileSystem", "General");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("media/MyGUI_Media", "FileSystem", "General");
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{

    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
    mRoot = new Ogre::Root("");

    // A list of required plugins
    Ogre::StringVector required_plugins;
    required_plugins.push_back("D3D9 RenderSystem");
	required_plugins.push_back("GL RenderSystem");
    required_plugins.push_back("Octree & Terrain Scene Manager");

    // List of plugins to load
    Ogre::StringVector plugins_toLoad;
    plugins_toLoad.push_back("RenderSystem_Direct3D9");
	plugins_toLoad.push_back("RenderSystem_GL");
    plugins_toLoad.push_back("Plugin_OctreeSceneManager");

    // Load the OpenGL RenderSystem and the Octree SceneManager plugins
    for (Ogre::StringVector::iterator j = plugins_toLoad.begin(); j != plugins_toLoad.end(); j++)
    {
#ifdef _DEBUG
        mRoot->loadPlugin(*j + Ogre::String("_d"));
#else
        mRoot->loadPlugin(*j);
#endif;
    }

    // Check if the required plugins are installed and ready for use
    // If not: exit the application
    Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
    for (Ogre::StringVector::iterator j = required_plugins.begin(); j != required_plugins.end(); j++)
    {
        bool found = false;
        // try to find the required plugin in the current installed plugins
        for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
        {
			Ogre::String pluginName=(*k)->getName();
            if (pluginName == *j)
            {
                found = true;
                break;
            }
        }
        if (!found)  // return false because a required plugin is not available
        {
            return false;
        }
    }

    setupResources();

    bool carryOn = configure();
    if (!carryOn) return false;

    chooseSceneManager();
    createCamera();
    createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

	mPlatform = new MyGUI::OgrePlatform();
	mPlatform->initialise(mWindow, mSceneMgr); // mWindow is Ogre::RenderWindow*, mSceneManager is Ogre::SceneManager*
	mGUI = new MyGUI::Gui();
	mGUI->initialise();

    createFrameListener();

    // Create the scene
    createScene();

    return true;
};
//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(mWindow->isClosed())
        return false;

    if(mShutDown)
        return false;

    //Need to capture/update each device
    mKeyboard->capture();
    mMouse->capture();

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{
    if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
    {
        mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        mShutDown = true;
    }

    MyGUI::InputManager::getInstance().injectKeyPress(MyGUI::KeyCode::Enum(arg.key), arg.text);
    return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
{
	MyGUI::InputManager::getInstance().injectKeyRelease(MyGUI::KeyCode::Enum(arg.key));
    return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
{
	MyGUI::InputManager::getInstance().injectMouseMove(arg.state.X.abs, arg.state.Y.abs, arg.state.Z.abs);
    return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	MyGUI::InputManager::getInstance().injectMousePress(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
    return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	MyGUI::InputManager::getInstance().injectMouseRelease(arg.state.X.abs, arg.state.Y.abs, MyGUI::MouseButton::Enum(id));
    return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}
