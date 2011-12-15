#pragma once

#include <stdexcept> 
using std::runtime_error;
class MoveOutOfImageRangeException : public runtime_error
{
public:
	MoveOutOfImageRangeException::MoveOutOfImageRangeException(): runtime_error( "attempted to reach a pixel out of range" ) {}
}; 

class MoveConfigFileNotFoundException : public runtime_error
{
public:
	MoveConfigFileNotFoundException::MoveConfigFileNotFoundException(): runtime_error( "the requested config file doesnt exist" ) {}
}; 

class MoveConfigFileRecordNotFoundException : public runtime_error
{
public:
	MoveConfigFileRecordNotFoundException::MoveConfigFileRecordNotFoundException(): runtime_error( "the requested record doesnt exist" ) {}
}; 