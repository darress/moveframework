// Playstation Move on the PC, shared header file
// this file contains function prototypes
// by Carl Kenner 4 October 2010 - 16 November 2010
/*
   This file (and anything derived from it, based on it, or using it) is released under a 
   non-military license, it may not be used for any military purpose, including military 
   research, or military funded research, training, recruitment, troop entertainment, 
   design of military systems, or anything else involving the military. I didn't write this
   to help kill people, sorry.
*/

#pragma once

#include "MoveButton.h"

// Maximum number of moves to support.
#define MAXMOVES 4

#ifdef _MSC_VER 
#define snprintf sprintf_s
#endif

namespace MoveDevice
{
	typedef struct tmove 
	{
		int Buttons;
		int TAnalog;
		int RawBattery;
		int RawForceX, RawForceY, RawForceZ;
		int RawGyroYaw, RawGyroPitch, RawGyroRoll;
		int RawTemperature;
		int RawMagnetX, RawMagnetY, RawMagnetZ;
		int Timestamp;
		int SeqNumber;
	} TMove, *PMove;

	typedef struct tnav 
	{
		int Buttons;
		int L1Analog, L2Analog;
		int UpAnalog, DownAnalog, LeftAnalog, RightAnalog;
		int LeftStickX, LeftStickY;
		unsigned char report[49];
	} TNav, *PNav;

	typedef struct tmoveoutput 
	{
		unsigned char r, g, b;
		unsigned char rumble;
		unsigned framecount;
	} TMoveOutput, *PMoveOutput;

	typedef struct tmovecalib 
	{
		short int AccHeader;
		short int AccVectors[6][3];
		short int GyroBiasHeaders[2];
		short int GyroBiasVectors[2][3];
		short int GyroHeader;
		short int GyroVectors[3][3];
		short int UnknownHeader;
		float UnknownVectors[2][3];
		float UnknownValues[2];
	} TMoveCalib, *PMoveCalib;

	typedef struct tmovebluetooth 
	{
		char MoveBtMacString[18], HostBtMacString[18];
		unsigned char MoveBtMac[6], HostBtMac[6];
		unsigned char unknown[3];
	} TMoveBluetooth, *PMoveBluetooth;

	int OpenMoves();
	int GetMoveCount();
	void CloseMoves();
	int PairMoves();

	bool SetMoveColour(int index, int r, int g, int b);
	bool SetMoveRumble(int index, int rumble);
	bool ReadMove(int index, PMove data, PMove old=0);
	bool ReadMoveCalibration(int index, PMoveCalib calib);
	bool ReadMoveBluetoothSettings(int index, PMoveBluetooth bt);
}