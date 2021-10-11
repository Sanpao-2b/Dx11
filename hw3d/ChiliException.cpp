#include "ChiliException.h"
#include <sstream> //��ʽ������ַ�����Ҫ�õĶ���

ChiliException::ChiliException(int line, const char * file) noexcept
	:
	line(line),
	file(file)
{}

//what()�������صĲ����ַ��� �����ַ�ָ�룬ָ��ĳ���ڴ�
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
	std::ostringstream oss;       //��ostringstream����ʽ����Щ�����
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();             //str()���ص���һ��string����� 
}
