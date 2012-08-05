// Ogre headers you need
#pragma once

#include <OgreException.h>
#include <OgreRoot.h>
#include <OgreConfigFile.h>
#include <OgreCamera.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreLogManager.h>
#include <OgreAnimation.h>
#include "OgreHardwarePixelBuffer.h"
#include "res/resource.h"
#include <OgrePlugin.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <Windows.h>
#include <WinBase.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setupapi.h>
#include <vector>
#include <cmath>
#include <list>

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

#include "IMoveManager.h"
#include "IMoveObserver.h"
#include "MoveFactory.h"
#include "MoveData.h"
#include "MoveButton.h"