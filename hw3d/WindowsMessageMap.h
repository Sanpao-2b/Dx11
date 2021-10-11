#pragma once
#include <unordered_map> //无序映射容器 内部实现了哈希表 查找极快 但是建立相对费性能 跟普通map容器使用的时候并无区别
#include <Windows.h>


class WindowsMessageMap
{
public:
	WindowsMessageMap(); 
	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp)const;
private:
	std::unordered_map<DWORD, std::string> map; 
};

