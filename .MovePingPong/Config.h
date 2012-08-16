#pragma once
#include "stdafx.h"

// Sizes
const float g_TableX = 0.76f;
const float g_TableY = 0.76f;
const float g_TableZ = 1.37f;
const float g_BallRadius = 0.03f;
const float g_NetOverhang = 0.1525f;
const float g_RacketRadiusA = 0.15f;
const float g_RacketRadiusB = 0.165f;
const float g_RacketThickness = 0.01f;
const float g_GroundY = 0.0f;
// Factors
const float g_SpinFactor = 0.2f;
const float g_AirResistance = 0.0f;
const float g_HitStrength = 1.5f;
const float g_Gravity = 9.81f;

struct CameraConfig
{
	Ogre::Vector3 pos;
	Ogre::Vector3 lookAt;

	CameraConfig(Ogre::Vector3 pos, Ogre::Vector3 lookAt)
		:pos(pos), lookAt(lookAt)
	{}
};

const CameraConfig g_Cameras[] = 
{
	CameraConfig(Ogre::Vector3(0.0f, 1.9f, 3.5f), Ogre::Vector3(0.0f, 0.9f, 0.0f)),
	CameraConfig(Ogre::Vector3(5.0f, 2.0f, 0.0f), Ogre::Vector3(0.0f, 0.0f, 0.0f)),
};

const int defaultCamera = 0;
