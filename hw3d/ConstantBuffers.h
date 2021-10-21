#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename C> //整个类都是模板，常量缓存一般要每帧更新 所以有个Update()函数
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		// 子资源：就是要写入的数据
		D3D11_MAPPED_SUBRESOURCE msr;
		// 为了更新常数缓存，需要用这个Map函数，当映射资源时，上锁
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD,0u,
			&msr
		));
		// 写入内存
		memcpy(msr.pData, &consts, sizeof(consts));
		// 写入结束 
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	//构造函数1 传入常量缓存对象进行初始化
	ConstantBuffer(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;			//常数缓存的用法基本都是动态的 每帧更新
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPU写权限
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;				//为0即可 不必深究

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}
	//版本2 没有初始化
	ConstantBuffer(Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(C);
		cbd.StructureByteStride = 0u;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
};

// 注意 继承一个模板类 不能直接使用其保护成员
// 如果要用，2种方法：  1、using 基类::成员   2、或者成员前面加this，如this->GetContext()
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
private:
	using ConstantBuffer<C>::pConstantBuffer; 
	using Bindable::GetContext;				  
public:
	using ConstantBuffer<C>::ConstantBuffer; //不提供构造函数，使用基类的构造函数 初始化基类
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
private:
	using ConstantBuffer<C>::pConstantBuffer; 
	using Bindable::GetContext;			
public:
	using ConstantBuffer<C>::ConstantBuffer; //不提供构造函数，使用基类的构造函数 初始化基类
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};