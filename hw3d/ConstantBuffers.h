#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

//类模板,
template<typename C> 
class ConstantBuffer : public Bindable
{
public:
	//常量缓存一般要每帧更新(传入consts数据更新我们的缓存) 所以有个Update()函数
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		// 创建一个子资源，子资源里有一个void*类型的指针，这个变量我们只要他的pData成员,把数据写到msr.pData指向的内存中
		D3D11_MAPPED_SUBRESOURCE msr;

		// 为了更新常数缓存，需要用这个Map函数，由CPU向GPU的缓存中写数据， map()函数是很重要的存在
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(),  //指向ID3D11Resource的指针，指向我们想要写东西进去的那块缓存
			0u,						//子资源的索引号
			D3D11_MAP_WRITE_DISCARD,//资源被映射用于写入；资源的先前内容将是未定义的。资源必须是通过写访问
			0u,						//指定CPU在GPU工作的时候可以干什么
			&msr					//这个用处应该就是指向 pConstantBuffer指向的那块内存吧？
		));
		// 把consts传进来的数据，写入"被映射子资源"msr.pData指向的内存
		memcpy(msr.pData, &consts, sizeof(consts));
		// 写入结束后，调用UnMap()函数把 常量缓冲区的CPU权限释放
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	//构造函数1 传入常数缓存数据 进行初始化
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
	//版本2 创建buffer时 没有初始化它
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

// 注意 继承一个模板类 不能直接使用其保护成员 如果要用，2种方法：  1、using 基类::成员   2、或者成员前面加this，如this->成员

// 两种着色器缓存 创建过程都一样， 唯一不同的是绑定的阶段不同
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
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};