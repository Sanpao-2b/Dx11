#include "ChiliException.h"
#include <sstream> //格式化输出字符串所要用的东西

ChiliException::ChiliException(int line, const char * file) noexcept
	:
	line(line),
	file(file)
{}

//what()函数返回的不是字符串 而是字符指针，指向某块内存
const char * ChiliException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();    
	return whatBuffer.c_str(); 
}

const char * ChiliException::GetType() const noexcept
{
	return "Chili Exception";
}

int ChiliException::GetLine() const noexcept
{
	return line;
}

const std::string & ChiliException::GetFile() const noexcept
{
	return file;
}

std::string ChiliException::GetOriginString() const noexcept
{
	std::ostringstream oss;       //用ostringstream来格式化那些玩意儿
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();             //str()返回的是一个string类对象 
}
