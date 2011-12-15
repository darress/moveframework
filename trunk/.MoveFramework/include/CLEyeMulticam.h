/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This library is part of CL-Eye SDK
// It allows the use of multiple CL-Eye cameras in your own applications
//
// For updates and file downloads go to: http://codelaboratories.com/research/view/cl-eye-muticamera-api
//
// Copyright 2008-2010 (c) Code Laboratories, Inc. All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <windows.h>
#define CLEYEMULTICAM_API extern "C" __declspec(dllimport)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLEyeMulticam Camera API
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// camera instance
typedef void *CLEyeCameraInstance;

// camera color mode
typedef enum
{ 
	CLEYE_GRAYSCALE, 
	CLEYE_COLOR
}CLEyeCameraColorMode;

// camera resolution
typedef enum
{ 
	CLEYE_QVGA,					// Allowed frame rates: 15, 30, 60, 75, 100, 125
	CLEYE_VGA					// Allowed frame rates: 15, 30, 40, 50, 60, 75
}CLEyeCameraResolution;

// camera parameters
typedef enum
{
	// camera sensor parameters
	CLEYE_AUTO_GAIN,			// [false, true]
	CLEYE_GAIN,					// [0, 79]
	CLEYE_AUTO_EXPOSURE,		// [false, true]
	CLEYE_EXPOSURE,				// [0, 511]
	CLEYE_AUTO_WHITEBALANCE,	// [false, true]
	CLEYE_WHITEBALANCE_RED,		// [0, 255]
	CLEYE_WHITEBALANCE_GREEN,	// [0, 255]
	CLEYE_WHITEBALANCE_BLUE,	// [0, 255]
	// camera linear transform parameters
	CLEYE_HFLIP,				// [false, true]
	CLEYE_VFLIP,				// [false, true]
	CLEYE_HKEYSTONE,			// [-500, 500]
	CLEYE_VKEYSTONE,			// [-500, 500]
	CLEYE_XOFFSET,				// [-500, 500]
	CLEYE_YOFFSET,				// [-500, 500]
	CLEYE_ROTATION,				// [-500, 500]
	CLEYE_ZOOM,					// [-500, 500]
	// camera non-linear transform parameters
	CLEYE_LENSCORRECTION1,		// [-500, 500]
	CLEYE_LENSCORRECTION2,		// [-500, 500]
	CLEYE_LENSCORRECTION3,		// [-500, 500]
	CLEYE_LENSBRIGHTNESS		// [-500, 500]
}CLEyeCameraParameter;

// Camera information
CLEYEMULTICAM_API int CLEyeGetCameraCount();
CLEYEMULTICAM_API GUID CLEyeGetCameraUUID(int camId);

// Library initialization
CLEYEMULTICAM_API CLEyeCameraInstance CLEyeCreateCamera(GUID camUUID, CLEyeCameraColorMode mode, 
														CLEyeCameraResolution res, int frameRate);
CLEYEMULTICAM_API bool CLEyeDestroyCamera(CLEyeCameraInstance cam);

// Camera capture control
CLEYEMULTICAM_API bool CLEyeCameraStart(CLEyeCameraInstance cam);
CLEYEMULTICAM_API bool CLEyeCameraStop(CLEyeCameraInstance cam);

// Camera settings control
CLEYEMULTICAM_API bool CLEyeSetCameraParameter(CLEyeCameraInstance cam, CLEyeCameraParameter param, int value);
CLEYEMULTICAM_API int CLEyeGetCameraParameter(CLEyeCameraInstance cam, CLEyeCameraParameter param);

// Camera video frame image data retrieval
CLEYEMULTICAM_API bool CLEyeCameraGetFrameDimensions(CLEyeCameraInstance cam, int &width, int &height);
CLEYEMULTICAM_API bool CLEyeCameraGetFrame(CLEyeCameraInstance cam, PBYTE pData, int waitTimeout = 2000);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
