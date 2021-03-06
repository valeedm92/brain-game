#include "OgreFramework.h"
#include "macUtils.h"
#include "Util.h"

#include "macPath.h"

namespace Ogre
{
    template<> OgreFramework* Ogre::Singleton<OgreFramework>::msSingleton = 0;
};

using namespace Ogre;

extern Util::ConfigGlobal globals;

OgreFramework::OgreFramework()
{
	m_bShutDownOgre     = false;
	m_iNumScreenShots   = 0;
    
    totalElapsed        = 0;
    
	m_pRoot				= 0;
	m_pRenderWnd        = 0;
	m_pSceneMgrMain		= 0;
	m_pSceneMgrSide 	= 0;
	m_pCameraMain		= 0;
	m_pViewportMain		= 0;
	m_pCameraSide		= 0;
	m_pViewportSide		= 0;
	m_pLog				= 0;
	m_pTimer			= 0;
    
	m_pInputMgr			= 0;
	m_pKeyboard			= 0;
	m_pMouse			= 0;
    
    m_pResourceGroupMgr = 0;
	m_pOverlayMgr       = 0;
    m_pFontMgr          = 0;
    m_pMeshMgr          = 0;
    m_pMaterialMgr      = 0;
    m_pTextureMgr       = 0;
    m_pCompositeMgr     = 0;
    
    m_pSoundMgr         = 0;
    
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
    m_ResourcePath = macBundlePath() + "/Contents/Resources/";
#elif defined(OGRE_IS_IOS)
    m_ResourcePath = macBundlePath() + "/";
#else
    m_ResourcePath = "";
#endif
    m_pTrayMgr          = 0;
    m_FrameEvent        = Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||
#if defined(OGRE_IS_IOS)
bool OgreFramework::initOgre(void* uiWindow, void* uiView, unsigned int width, unsigned int height, Ogre::RenderTargetListener *pRenderTargetListener)
#else
bool OgreFramework::initOgre(void* uiWindow, void* uiView, unsigned int width, unsigned int height, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener, Ogre::RenderTargetListener *pRenderTargetListener)
#endif
{
    new Ogre::LogManager();
    
	m_pLog = Ogre::LogManager::getSingleton().createLog("OgreLogfile.log", true, true, false);
	m_pLog->setDebugOutputEnabled(true);
    
    String pluginsPath;
    // only use plugins.cfg if not static
#ifndef OGRE_STATIC_LIB
    pluginsPath = m_ResourcePath + "plugins.cfg";
#endif
    m_pRoot = new Ogre::Root(pluginsPath, Ogre::macBundlePath() + "/ogre.cfg");
    
#ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.load();
#endif
    
    m_pRoot->initialise(false, "");
    
    Ogre::NameValuePairList params;
    params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)uiWindow);
    params["externalViewHandle"] = Ogre::StringConverter::toString((unsigned long)uiView);
    params["contentScalingFactor"] = Ogre::StringConverter::toString((unsigned long)1.0);
    
    m_pRenderWnd = m_pRoot->createRenderWindow("", width, height, false, &params);
    
	m_pSceneMgrMain = m_pRoot->createSceneManager(ST_GENERIC, "SceneManagerMain");
	m_pSceneMgrMain->setAmbientLight(Ogre::ColourValue(0.0, 0.0, 0.0));
    
	m_pCameraMain = m_pSceneMgrMain->createCamera("CameraMain");
	m_pCameraMain->setPosition(Vector3(0, 0, 50));
	m_pCameraMain->lookAt(Vector3(0, 0, 0));
	m_pCameraMain->setNearClipDistance(0.1);
    m_pCameraMain->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    m_pCameraMain->setOrthoWindow(25.0, 25.0);
	m_pViewportMain = m_pRenderWnd->addViewport(m_pCameraMain, 1,
                                                0.0,
                                                0.0,
                                                1.0,
                                                1.0);
    m_pViewportMain->getTarget()->addListener(pRenderTargetListener);    
    
	Ogre::String secName, typeName, archName;
	Ogre::ConfigFile cf;
    
    cf.load(m_ResourcePath + "resources.cfg");
    
    m_pResourceGroupMgr = Ogre::ResourceGroupManager::getSingletonPtr();
    m_pOverlayMgr = Ogre::OverlayManager::getSingletonPtr();
    m_pFontMgr = Ogre::FontManager::getSingletonPtr();
    m_pMeshMgr = Ogre::MeshManager::getSingletonPtr();
    m_pMaterialMgr = Ogre::MaterialManager::getSingletonPtr();
    m_pTextureMgr = Ogre::TextureManager::getSingletonPtr();
    m_pCompositeMgr = CompositorManager::getSingletonPtr();
    
	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
		Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || defined(OGRE_IS_IOS)
            // OS X does not set the working directory relative to the app,
            // In order to make things portable on OS X we need to provide
            // the loading with it's own bundle path location
            if (!Ogre::StringUtil::startsWith(archName, "/", false)) // only adjust relative dirs
                archName = Ogre::String(m_ResourcePath + archName);
#endif
            m_pResourceGroupMgr->addResourceLocation(archName, typeName, secName);
        }
    }
    
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
    
	m_pTrayMgr = new OgreBites::SdkTrayManager("TrayMgr", m_pRenderWnd, m_pMouse, this);
    //m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    //m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    m_pTrayMgr->hideCursor();
    m_pTrayMgr->setListener(this);
    m_pTrayMgr->setTrayPadding(10.0);
    
    Ogre::FontManager::getSingleton().getByName("SdkTrays/Caption")->load();
    //  m_quitButton = OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMLEFT, "sdkQuitButton", "QUIT", 250);
    
    m_pSoundMgr = OgreOggSound::OgreOggSoundManager::getSingletonPtr();
    m_pSoundMgr->init();
    m_pSoundMgr->createSound("SoundGreatFeedback", "ding3up2fast.wav", false, false, true);
    m_pSoundMgr->createSound("SoundBadFeedback", "negativebeep.wav", false, false, true);
    //m_pSoundMgr->createSound("SoundPod1", "spacePodWub.wav", false, false, true);
    //m_pSoundMgr->createSound("SoundPod2", "spacePodDisorient.wav", false, false, true);
    //m_pSoundMgr->createSound("SoundPod3", "spacePodWobble.wav", false, false, true);
    //m_pSoundMgr->createSound("SoundPod4", "spacePodBalance.wav", false, false, true);
    //m_pSoundMgr->createSound("SoundPod1", "pod4.wav", false, false, true);           // Rose
    //m_pSoundMgr->createSound("SoundPod2", "pod3.wav", false, false, true);            // Iris
    //m_pSoundMgr->createSound("SoundPod3", "bubbleSound.wav", false, false, true);    // Bubble Flower
    //m_pSoundMgr->createSound("SoundPod4", "pod2.wav", false, false, true);
    
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    
	m_pTimer = OGRE_NEW Ogre::Timer();
	m_pTimer->reset();
    
    requestResize();
	m_pRenderWnd->setActive(true);
    
	return true;
}

void OgreFramework::requestResize()
{
    m_pRenderWnd->windowMovedOrResized();
	m_pCameraMain->setAspectRatio(Real(m_pViewportMain->getActualWidth()) / Real(m_pViewportMain->getActualHeight()));
    globals.screenWidth = m_pRenderWnd->getWidth();
    globals.screenHeight = m_pRenderWnd->getHeight();
    globals.set();
}

OgreFramework::~OgreFramework()
{
    if(m_pInputMgr) OIS::InputManager::destroyInputSystem(m_pInputMgr);
    if(m_pTrayMgr)  delete m_pTrayMgr;
#ifdef OGRE_STATIC_LIB
    m_StaticPluginLoader.unload();
#endif
    if(m_pRoot)     delete m_pRoot;
    
    m_pInputMgr = 0;
    m_pTrayMgr = 0;
    m_pRoot = 0;
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
#if !defined(OGRE_IS_IOS)
	
	if(m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
        m_bShutDownOgre = true;
        return true;
	}
    
	if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
	{
		m_pRenderWnd->writeContentsToTimestampedFile("BOF_Screenshot_", ".png");
		return true;
	}
    
	if(m_pKeyboard->isKeyDown(OIS::KC_M))
	{
		static int mode = 0;
		
		if(mode == 2)
		{
			m_pCameraMain->setPolygonMode(PM_SOLID);
			mode = 0;
		}
		else if(mode == 0)
		{
            m_pCameraMain->setPolygonMode(PM_WIREFRAME);
            mode = 1;
		}
		else if(mode == 1)
		{
			m_pCameraMain->setPolygonMode(PM_POINTS);
			mode = 2;
		}
	}
#endif
	return true;
}

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#if !defined(OGRE_IS_IOS)
bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
	return true;
}

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	return true;
}
#endif

void OgreFramework::updateOgre(float timeSinceLastFrame)
{
#if OGRE_VERSION >= 0x10800
    m_pSceneMgrMain->setSkyBoxEnabled(true);
#endif
    
    totalElapsed += timeSinceLastFrame;
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

Ogre::String OgreFramework::getMacBundlePath() const
{
    std::cout << "MAC BUNDLE PATH: " << macBundlePath() << std::endl;
    std::cout << "APP DOCUME PATH: " << applicationDocumentsPath() << std::endl;
    //return macBundlePath();
    return applicationDocumentsPath();
}

void OgreFramework::buttonHit(OgreBites::Button* button)
{
    if (button->getName() == "quitButton")
    {
        requestOgreShutdown();
    }
}
