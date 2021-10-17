#pragma once
#include <exception>
#include <string>
//自定义的一个异常基类
class ChiliException :public std::exception
{
public:
	ChiliException(int line, const char* file) noexcept;
	const char* what() const noexcept override;  //返回出错的信息描述
	virtual const char* GetType() const noexcept;//错误类型
	int GetLine() const noexcept;                //错误行
	const std::string& GetFile() const noexcept; //错误文件
	std::string GetOriginString() const noexcept;//把错误类型、行、文件 格式化成字符串 
private:
	int line;        //出错的行数
	std::string file;//出错的文件
protected: //孩子可以访问
	mutable std::string whatBuffer; 
};

