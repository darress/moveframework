#include "stdafx.h"
#include "Game.h"

int WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
    // Create application object
    Game app;

    try 
	{
        app.go();
    } 
	catch( Ogre::Exception& e )
	{
        MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    return 0;
}