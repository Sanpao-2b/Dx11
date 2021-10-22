#pragma once
#include "Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	//这里的函数仅提供子类访问Graphics类中的成员属性 所有子类的函数都要传入一个gfx对象，然后调用基类bindable中的这仨函数 去获得Graphics中的私有属性
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noexcept(!IS_DEBUG); //InfoManager变量只在DEBUG模式下才有
};