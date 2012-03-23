#pragma once
#include "MovePrecompiled.h"
#include <direct.h>

class MOVE_EXPORT CIniFile
{

public:
	static inline float toF(std::string value)
	{
		std::istringstream iss(value);
		float retval;
		iss >> retval;
		return retval;
	}

	static inline int toI(std::string value)
	{
		std::istringstream iss(value);
		int retval;
		iss >> retval;
		return retval;
	}

	static std::string GetValue(char* KeyName, char* SectionName, char* FileName)
	{
		char value[200];
		char path[200];
		_getcwd(value,200);
		sprintf_s(path,"%s\\%s",value,FileName);
		GetPrivateProfileString(SectionName, KeyName, "", value, 200, path);
		if (strcmp(value,"")==0)
			throw MoveConfigFileRecordNotFoundException();
		return Trim(std::string(value));
	}

	static bool SetValue(char* KeyName, std::string valuein, char* SectionName, char* FileName)
	{
		char value[200];
		char path[200];
		_getcwd(value,200);
		sprintf_s(path,"%s\\%s",value,FileName);
		WritePrivateProfileString(SectionName, KeyName, valuein.c_str(), path);
		return true;
	}

	static int GetInt(char* KeyName, char* SectionName, char* FileName)
	{
		std::string value = GetValue(KeyName, SectionName, FileName);
		return toI(value);
	}

	static float GetFloat(char* KeyName, char* SectionName, char* FileName)
	{
		std::string value = GetValue(KeyName, SectionName, FileName);
		return toF(value);
	}

	static Move::Vector3 GetVector3(char* KeyName, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string value = GetValue(KeyName, SectionName, FileName);
		std::vector<std::string> tokens;
		std::istringstream iss(value);
		copy(istream_iterator<std::string>(iss),
				 istream_iterator<std::string>(),
				 back_inserter<vector<std::string> >(tokens));
		return Move::Vector3(toF(tokens[0]),toF(tokens[1]),toF(tokens[2]));
	}

	static Move::Matrix3 GetMatrix3(char* KeyName, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string value = GetValue(KeyName, SectionName, FileName);
		vector<std::string> tokens;
		std::istringstream iss(value);
		copy(istream_iterator<std::string>(iss),
				 istream_iterator<std::string>(),
				 back_inserter<vector<std::string> >(tokens));
		return Move::Matrix3(toF(tokens[0]),toF(tokens[1]),toF(tokens[2]),
					   toF(tokens[3]),toF(tokens[4]),toF(tokens[5]),
					   toF(tokens[6]),toF(tokens[7]),toF(tokens[8]));
	}

	static bool SetValue(char* KeyName, int Value, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string str;
		std::stringstream stream;
        stream  << Value;
        str = stream.str();
		return SetValue(KeyName, str, SectionName, FileName);
	}

	static bool SetValue(char* KeyName, float Value, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string str;
		std::stringstream stream;
        stream  << Value;
        str = stream.str();
		return SetValue(KeyName, str, SectionName, FileName);
	}

	static bool SetValue(char* KeyName, Move::Vector3 Value, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string str;
		std::stringstream stream;
        stream  << Value.x << " " << Value.y << " " << Value.z;
        str = stream.str();
		return SetValue(KeyName, str, SectionName, FileName);
	}

	static bool SetValue(char* KeyName, Move::Quaternion Value, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string str;
		std::stringstream stream;
        stream  << Value.w << " " << Value.x << " " << Value.y << " " << Value.z;
        str = stream.str();
		return SetValue(KeyName, str, SectionName, FileName);
	}

	static bool SetValue(char* KeyName, Move::Matrix3 Value, char* SectionName, char* FileName)
	{
		using namespace std;
		std::string str;
		std::stringstream stream;
        stream << Value[0][0] << " " << Value[0][1] << " " << Value[0][2] << " "
			   << Value[1][0] << " " << Value[1][1] << " " << Value[1][2] << " "
			   << Value[2][0] << " " << Value[2][1] << " " << Value[2][2];
        str = stream.str();
		return SetValue(KeyName, str, SectionName, FileName);
	}

private:
	static std::string Trim(std::string str)
	{
		size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
		size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af

		// if all spaces or empty return an empty string
		if(( std::string::npos == startpos ) || ( std::string::npos == endpos))
		{
		str = "";
		}
		else
		str = str.substr( startpos, endpos-startpos+1 );
		return str;
	}
};