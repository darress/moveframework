/* Playstation Move on the PC.
   This file reads all the values from the Move, and changes its colour, and rumbles.
   There should be no platform specific code here, and no knowledge of the transport layer
   by Carl Kenner 4 October 2010 - 16 November 2010
   This file (and anything derived from it, based on it, or using it) is released under a 
   non-military license, it may not be used for any military purpose, including military 
   research, or military funded research, training, recruitment, troop entertainment, 
   design of military systems, or anything else involving the military. I didn't write this
   to help kill people, sorry.
*/

#include "MovePrecompiled.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MoveDevice.h"
#include "hidapi.h"

#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#pragma comment ( lib, "Irprops.lib")

namespace MoveDevice
{
	hid_device* MoveHandles[MAXMOVES] = {0};
	TMoveOutput MoveOutput[MAXMOVES] = {0};
	int MoveCount = 0;

	int OpenMoves() {
		struct hid_device_info *devs, *cur_dev;

		unsigned char buf[49];
		hid_device *handle;

		devs = hid_enumerate(0x054c, 0x03d5);
		cur_dev = devs;	
		while (cur_dev) {
			handle = hid_open_path(cur_dev->path);
			if (hid_read_timeout(handle,buf,49,500)>0)
			{
				MoveHandles[MoveCount++]=handle;
			}
			else
			{
				hid_close(handle);
			}
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);

	  return MoveCount;
	}

	void getHostBtAddress(unsigned char* addr)
	{
		HANDLE hRadio;
		BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(btfrp) };
		HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio( &btfrp, &hRadio );
		if ( NULL != hFind )
		{
			BLUETOOTH_RADIO_INFO radioInfo;

			radioInfo.dwSize = sizeof(radioInfo);

			if (ERROR_SUCCESS == BluetoothGetRadioInfo(hRadio, &radioInfo))
			{
				memcpy(addr,radioInfo.address.rgBytes,6);
			}
			CloseHandle( hRadio );
			BluetoothFindRadioClose( hFind );
		}
		else
			throw MoveNoBTDongleFound();
	}

	int PairMoves() {
		int numBtMacSet=0;
		struct hid_device_info *devs, *cur_dev;

		unsigned char buf[49];
		hid_device *handle;
		unsigned char hostAddr[6];
		unsigned char curAddr[6];
		getHostBtAddress(hostAddr);
		devs = hid_enumerate(0x054c, 0x03d5);
		cur_dev = devs;	
		while (cur_dev) {
			handle = hid_open_path(cur_dev->path);
			memset(buf,0,49);
			//get the current host mac address
			buf[0]=0x04;
			if (hid_get_feature_report(handle, buf, 49)>1)
			{
				memcpy(curAddr,&buf[10],6);
				//if not the same, set the new
				if (memcmp(curAddr,hostAddr,6)!=0)
				{
				buf[0]=0x05;
				memcpy(&buf[1],hostAddr,6);
				//TODO set the mac in buf
				if (hid_send_feature_report(handle, buf, 49)>1)
					numBtMacSet++;
				}
			}
			hid_close(handle);
			cur_dev = cur_dev->next;
		}
		hid_free_enumeration(devs);

	  return numBtMacSet;
	}

	int GetMoveCount() {
		return MoveCount;
	}

	void CloseMoves() {
	  for (int i=0; i<MAXMOVES; i++) {
			if (MoveHandles[i]) {
				hid_close(MoveHandles[i]);
				MoveHandles[i]=0;
			}
	  }
	}

	// sends output to the Move, must be done every second or so, 
	// or the light and rumble switch themselves off
	bool SendMoveOutput(int index) {
		unsigned char report[49];
		memset(report,0,49);
		report[0] = 2; // must be 2 or 6 to control colour
	    report[1] = 0; // unknown
		report[2] = MoveOutput[index].r;
		report[3] = MoveOutput[index].g;
		report[4] = MoveOutput[index].b;
		report[5] = 0; // must be an even number to allow rumble
		report[6] = MoveOutput[index].rumble;
		MoveOutput[index].framecount=0; // don't need to resend for a while
		return hid_write(MoveHandles[index], report, 49)>1;
	}

	// Sets the colour of the move's glowing ball, each component is 0x00 to 0xFF, 
	// but the green led is much darker than the other leds, so FF FF FF looks purple.
	// Use something like FF AF FF for white.
	bool SetMoveColour(int index, int r, int g, int b) {
		if (index>=MAXMOVES || index<0) return false;
		if (MoveOutput[index].framecount<100 && MoveOutput[index].r==r && MoveOutput[index].g==g && MoveOutput[index].b==b) return true;
		MoveOutput[index].r = r;
		MoveOutput[index].g = g;
		MoveOutput[index].b = b;
		return SendMoveOutput(index);
	}

	// Sets the Move's amount of vibration. Rumble can be 0 to 255.
	// Some low values of rumble will not turn rumble on if it is off,
	// but also won't turn rumble off if it is on. 0 will always turn it off.
	bool SetMoveRumble(int index, int rumble) {
		if (index>=MAXMOVES || index<0) return false;
		if (MoveOutput[index].framecount < 100 && MoveOutput[index].rumble == rumble) return true;
		MoveOutput[index].rumble = rumble;
		return SendMoveOutput(index);
	}

	// decode 12 bit signed twos-complement
	int TwelveBits(int n) {
	  if (n>0x800) n-=0x1000;
	  return n;
	}

	// I originally thought it was more complicated than this. Oops.
	int NineBits(int n) {
	  return n-511;
	}


	// old provides the data from half a frame before. That means we have double the frame rate.
	// Note that the data in old was not previously reported.
	// Unfortunately, I don't know what the frame rate is. I think it's about 85 Hz.
	// NOTE! All the addresses are 1 less than they were before to remove MotionInJoy specific stuff!
	bool ReadMove(int index, PMove data, PMove old) {
		if (index>=MoveCount || index<0) {
			return false;
		}
		MoveOutput[index].framecount++;
		if (MoveOutput[index].framecount >= 100) {
			SendMoveOutput(index);
		}
		if (!data) {
			return false;
		}

		unsigned char report[49];
		if (hid_read(MoveHandles[index], report, 49)<1) {
			return false;
		}

		data->Buttons    = report[2] | (report[1] << 8) 
							| ((report[3] & 1) << 16) | ((report[4] & 0xF0) << 13);
		data->TAnalog    = report[6]; // report 5 is the same, but slightly older
		data->RawBattery = report[12];
		data->Timestamp  = report[11]*256 + report[43];
		data->SeqNumber  = report[4] & 0x0F;

		// Uses left handed coordinates like GlovePIE or Direct3D: x is right, y is up, z is forwards
		// rotations are like GlovePIE: yaw is turn right, pitch is tilt up, roll is tilt right
		data->RawForceX = report[20]*256 + report[19] - 32768; // same as 6 bytes earlier
		data->RawForceZ = report[22]*256 + report[21] - 32768;
		data->RawForceY = report[24]*256 + report[23] - 32768;
	
		data->RawGyroPitch = (report[32]*256 + report[31]) - 32768; // same as 6 bytes earlier
		data->RawGyroRoll  = (report[34]*256 + report[33]) - 32768;
		data->RawGyroYaw   = 32768 - (report[36]*256 + report[35]);
	
		// -1733 = 29.0, -1685.5 = 29 degrees C // -1681.5 = 29.5
		data->RawTemperature = TwelveBits(report[37]*16+(report[38] >> 4)); 
	
		data->RawMagnetX = -TwelveBits((report[38] & 15)*256+report[39]);
		data->RawMagnetZ =  TwelveBits(report[40]*16 + (report[41] >> 4));
		data->RawMagnetY = -TwelveBits((report[41] & 15)*256+report[42]);
  
		if (old) {
			old->TAnalog = report[5];
			old->RawForceX = report[14]*256 + report[13] - 32768; // same as 6 bytes earlier
			old->RawForceZ = report[16]*256 + report[15] - 32768;
			old->RawForceY = report[18]*256 + report[17] - 32768;
			old->RawGyroPitch = (report[26]*256 + report[25]) - 32768; // same as 6 bytes earlier
			old->RawGyroRoll  = (report[28]*256 + report[27]) - 32768;
			old->RawGyroYaw   = 32768-(report[30]*256 + report[29]);
		}

		return true;
	}

	// This is as far as I have got with the calibration. Note that this function will only work if it's
	// connected via libusb-win32, or normal windows HID (not finished implementing yet), since MotionInJoy
	// doesn't give us a way of reading this data.
	bool ReadMoveCalibration(int index, PMoveCalib calib) {
		if (index>=MAXMOVES || index<0 || !calib) return false;
	  // the calibration has 3 parts, which you get by reading report 0x10 3 times
	  unsigned char temp[49];
	  unsigned char report[49+47+47];
	  int parts=0;
	  while (parts!=(1|2|4)) {
		  memset(temp,0,49);
		  temp[0]=0x10;
		  if (hid_get_feature_report(MoveHandles[index], temp, 49)<1) return false;
		switch (temp[1] & 0xF) {
		  case 0: memcpy(report, temp, 49); parts |= 1; break;
		  case 1: memcpy(&report[49], &temp[2], 47); parts |= 2; break;
		  case 2: memcpy(&report[49+47], &temp[2], 47); parts |= 4; break;
		}
	  }
	  // I don't know what these headers are. Maybe temperatures?
	  calib->AccHeader = report[2]+report[3]*256;
	  char map[6] = {1,3, 2,0, 5,4}; // convert to order minx,maxx,miny,maxy,minz,maxz
	  // accelerometer readings for 6 different orientations
	  for (int v=0; v<6; v++) {
		int i = map[v];
		calib->AccVectors[v][0] = report[4+i*6+0]+report[4+i*6+1]*256-32768;
		calib->AccVectors[v][1] = report[4+i*6+4]+report[4+i*6+5]*256-32768;
		calib->AccVectors[v][2] = report[4+i*6+2]+report[4+i*6+3]*256-32768;
	  }
	  //this values have to be subtracted from the gyro values
	  for (int i=0; i<2; i++) {
		calib->GyroBiasHeaders[i] = report[40+i*8] + report[41+i*8]*256;
		calib->GyroBiasVectors[i][0] = report[42+i*8+0] + report[42+i*8+1]*256-32768;
		calib->GyroBiasVectors[i][1] = report[42+i*8+4] + report[42+i*8+5]*256-32768;
		calib->GyroBiasVectors[i][2] = report[42+i*8+2] + report[42+i*8+3]*256-32768;
	  }
	  // from 56 to 64 are all zeros, 65 is 0x01, I don't know what (if anything) it means

	  // this part is actually a matrix, or 3 vectors, in little-endian, with each one preceded by 0x01E0 (480)
	  // the matrix contains the gyro readings at 8 rpm to remove the bias, one of the bias vectors must be substracted
	  calib->GyroHeader = report[66]+report[67]*256; // another strange header
	  char map2[3] = {0,2,1}; // convert to order x,y,z
	  for (int v=0; v<3; v++) {
		int i = map2[v];
		calib->GyroVectors[v][0] = report[70+i*8+0]+report[70+i*8+1]*256-32768;
		calib->GyroVectors[v][1] = report[70+i*8+4]+report[70+i*8+5]*256-32768;
		calib->GyroVectors[v][2] = report[70+i*8+2]+report[70+i*8+3]*256-32768;
	  }

	  calib->UnknownHeader = report[92]+report[93]*256; // another strange header
	  // from 92 to 126 there are 8 float values, 2 vectors and than 2 independent values
	  for (int i=0; i<2; i++) {
		  int intValue;
		  intValue=(report[94+i*12+0]+(report[94+i*12+1]<<8)+(report[94+i*12+2]<<16)+(report[94+i*12+3]<<24));
		  calib->UnknownVectors[i][0]=*reinterpret_cast<float*>(&intValue);
		  intValue=(report[94+i*12+8]+(report[94+i*12+9]<<8)+(report[94+i*12+10]<<16)+(report[94+i*12+11]<<24));
		  calib->UnknownVectors[i][1]=*reinterpret_cast<float*>(&intValue);
		  intValue=(report[94+i*12+4]+(report[94+i*12+5]<<8)+(report[94+i*12+6]<<16)+(report[94+i*12+7]<<24));
		  calib->UnknownVectors[i][2]=*reinterpret_cast<float*>(&intValue);
	  }
	  //the remaining 2 float values
	  for (int i=0; i<2; i++) {
		  int intValue=(report[118+i*4+0]+(report[118+i*4+1]<<8)+(report[118+i*4+2]<<16)+(report[118+i*4+3]<<24));
		  calib->UnknownValues[i]=*reinterpret_cast<float*>(&intValue);
	  }
		return true;
	}

	bool ReadMoveBluetoothSettings(int index, PMoveBluetooth bt) {
		if (index>=MAXMOVES || index<0 || !bt) return false;
		unsigned char report[49];
		memset(report,0,49);
		report[0]=0x04;
		if (hid_get_feature_report(MoveHandles[index], report, 49)<1) return false;
		memcpy(bt->MoveBtMac,&report[1],6);
		memcpy(bt->HostBtMac,&report[10],6);
		memcpy(bt->unknown,&report[7],3);
		snprintf(bt->MoveBtMacString,18,"%02X:%02X:%02X:%02X:%02X:%02X",report[6],report[5],report[4],report[3],report[2],report[1]);
		snprintf(bt->HostBtMacString,18,"%02X:%02X:%02X:%02X:%02X:%02X",report[15],report[14],report[13],report[12],report[11],report[10]);
		return true;
	}

}