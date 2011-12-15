#include "MovePrecompiled.h"
#include "MoveEmulator.h"
#include "IniFile.h"

using namespace std;


namespace Move
{
	MoveEmulator::MoveEmulator(void)
	{
		roll =0;
		pitch =0;
		yaw =0;

		player_roll =0;
		player_pitch =0;
		player_yaw =0;

		EmulatorSpeed = 0.005f;


	}


	MoveEmulator::~MoveEmulator(void)
	{
	}

	int MoveEmulator::initMoves()
	{
		string emu_mode = CIniFile::GetValue("mode","Emulator","settings.cfg");
		if(emu_mode.compare("controller")==0)
		{
			EmuMode = CONTROLLER;
			throw 0;
			//TODO: cini cant do that any more
			/*std::vector<CIniFile::Record> section = CIniFile::GetSection("Emulator_buttons","settings.cfg");

			for(std::vector<CIniFile::Record>::iterator i= section.begin(); i!=section.end(); i++)
			{
				keyMap.insert(std::pair<int,int>(CIniFile::toI((*i).Value), (CIniFile::toI((*i).Key))));

			}*/

			return 1;
		}
		if(emu_mode.compare("read_log")==0)
		{
			EmuMode = READ_LOG;
			string emu_file = CIniFile::GetValue("emu_file","Emulator","settings.cfg");
			std::ifstream file;
			file.open( emu_file, std::ifstream::in);
			float w,x,y,z;
			char c;
			while(file>>w>>c>>x>>c>>y>>c>>z)
			{
				Quaternion q(w,x,y,z);
				orientations.push(q);
			}
			file.close();
			return 1;
		}
		// returns the number of moves
		return 0;
	}

	Quaternion MoveEmulator::getOrientation(int id)
	{

		if(EmuMode == READ_LOG)
		{
			if(!orientations.empty())
			{
				Quaternion q = orientations.front();
				orientations.pop();
				return q;
			}
			else
				return Quaternion(1,0,0,0);
		}


		else if(EmuMode == CONTROLLER)
		{
			//[cos(a/2), sin(a/2) * nx, sin(a/2)* ny, sin(a/2) * nz]


			/*if(KeyMap[1][0])
				pitch -= EmulatorSpeed;

			if(KeyMap[1][1])
				pitch += EmulatorSpeed;

			if(KeyMap[0][1])
				roll += EmulatorSpeed;

			if(KeyMap[2][1])
				roll -= EmulatorSpeed;

			if(KeyMap[0][0])
				yaw += EmulatorSpeed;

			if(KeyMap[2][0])
				yaw -= EmulatorSpeed;*/

			float roll_angle = 0;
			float pitch_angle = 0;
			float yaw_angle = 0;
			float actual_roll = 0;
			float actual_pitch = 0;
			float actual_yaw = 0;

			if(id==0)
			{
				
				actual_roll = roll;
				actual_pitch = pitch;
				actual_yaw = yaw;

			}
			if(id==1)
			{
				actual_roll = player_roll;
				actual_pitch = player_pitch;
				actual_yaw = player_yaw;
			}
			
			roll_angle = cos(actual_roll/2.0f);
			pitch_angle = cos(actual_pitch/2.0f);
			yaw_angle = cos(actual_yaw/2.0f);

			Quaternion q1(roll_angle,0,0,(sin(actual_roll/2.0f)));
			Quaternion q2(pitch_angle,(sin(actual_pitch/2.0f)),0,0);
			Quaternion q3(yaw_angle,0,(sin(actual_yaw/2.0f)),0);

			//if (abs(test)>=3.14)
			//	//add *= -1;
			//	test = 3.14;
			//else
			//test += add;


			//float add = 0.005;
			/*if (abs(test)>=(3.14/2.0))
			test = (3.14/2.0);
			else
			test += add;*/
			//return q2*q1;
			return q2*q1*q3;
			//return q1*q2;
		}
		else
				return Quaternion(1,0,0,0);

	}

	Vector3 MoveEmulator::getPosition(int id)
	{
		if(EmuMode ==CONTROLLER)
		{
			if(id==0)
				return move_position;
			if(id==1)
				return player_position;

		}
		else
			return Vector3(0,0,0);
	}

	Vector3 MoveEmulator::getAngularVelocity(int id){return Vector3(0,0,0);}
	Vector3 MoveEmulator::getAngularAcceleration(int id){return Vector3(0,0,0);}
	Vector3 MoveEmulator::getVelocity(int id){return Vector3(0,0,0);}
	Vector3 MoveEmulator::getAcceleration(int id){return Vector3(0,0,0);}
	bool MoveEmulator::getButtonState(int id, int buttonId){return false;}
	bool MoveEmulator::pointingToDisplay(int id){return false;}
	Vector3 MoveEmulator::displayPosition(int id){return Vector3(0,0,0);}

	void MoveEmulator::initCamera(int numMoves=0)
	{
	}

	void MoveEmulator::processKeyInput(int key)
	{
		switch(keyMap[key])
		{
		case 1: pitch -= EmulatorSpeed;
						break;
		
		case 2: pitch += EmulatorSpeed;
						break;

		case 4: roll += EmulatorSpeed;
						break;

		case 3: roll -= EmulatorSpeed;
						break;

		case 5: yaw += EmulatorSpeed;
						break;

		case 6: yaw -= EmulatorSpeed;
						break;

		case 7: EmulatorSpeed -= 0.0005;
						if(EmulatorSpeed <=0.001 )
						EmulatorSpeed =0.001;
						break;

		case 8: EmulatorSpeed += 0.0005;
						break;

		case 9: player_pitch -= EmulatorSpeed;
						break;
		
		case 10: player_pitch += EmulatorSpeed;
						break;

		case 11: player_roll += EmulatorSpeed;
						break;

		case 12: player_roll -= EmulatorSpeed;
						break;

		case 13: player_yaw += EmulatorSpeed;
						break;

		case 14: player_yaw -= EmulatorSpeed;
						break;
	
		}
	
	}

	/*void MoveEmulator::PassKeyPressed(const OIS::KeyEvent &arg)
	{
		if(arg.key == OIS::KC_I)
		{
			KeyMap[1][0] = true;
		}
		if(arg.key == OIS::KC_K)
		{
			KeyMap[1][1] = true;
		}
		if(arg.key == OIS::KC_J)
		{
			KeyMap[0][1] = true;
		}
		if(arg.key == OIS::KC_L)
		{
			KeyMap[2][1] = true;
		}
		if(arg.key == OIS::KC_U)
		{
			KeyMap[0][0] = true;
		}
		if(arg.key == OIS::KC_O)
		{
			KeyMap[2][0] = true;
		}
		if(arg.key == OIS::KC_N)
		{
			EmulatorSpeed -= 0.0005;
			if(EmulatorSpeed <=0.0001 )
				EmulatorSpeed =0.0001;
		}
		if(arg.key == OIS::KC_M)
		{
			EmulatorSpeed += 0.0005;
		}

	}

	void MoveEmulator::PassKeyReleased(const OIS::KeyEvent &arg)
	{
		if(arg.key == OIS::KC_I)
		{
			KeyMap[1][0] = false;
		}
		if(arg.key == OIS::KC_K)
		{
			KeyMap[1][1] = false;
		}
		if(arg.key == OIS::KC_J)
		{
			KeyMap[0][1] = false;
		}
		if(arg.key == OIS::KC_L)
		{
			KeyMap[2][1] = false;
		}
		if(arg.key == OIS::KC_U)
		{
			KeyMap[0][0] = false;
		}
		if(arg.key == OIS::KC_O)
		{
			KeyMap[2][0] = false;
		}
	}*/

	void MoveEmulator::passMouseMoved( float x, float y, float z )
	{
		/*position.x = -(WindowW/20.0)+arg.state.X.abs/10.0;
		position.y = (WindowH/20.0)+-arg.state.Y.abs/10.0;
		position.z = -arg.state.Z.abs/10.0;*/

		move_position.x += x*0.1;
		move_position.y -= y*0.1;
		move_position.z -= z*0.1;
	}

	/*void MoveEmulator::PassWindowParam(int width, int height)
	{
		WindowW = width;
		WindowH = height;
	}*/

	void MoveEmulator::subsribeMove(IMoveObserver* observer)
	{
		observers.push_back(observer);
	}
	void MoveEmulator::unsubsribeMove(IMoveObserver* observer)
	{
		observers.remove(observer);
	}

	void MoveEmulator::subsribeCalibration(IMoveCalibrationObserver* observer)
	{
		calibrationObservers.push_back(observer);
	}
	void MoveEmulator::unsubsribeCalibration(IMoveCalibrationObserver* observer)
	{
		calibrationObservers.remove(observer);
	}
}
