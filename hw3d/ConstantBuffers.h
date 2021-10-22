#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template<typename C> //�����඼��ģ�壬��������һ��Ҫÿ֡���� �����и�Update()����
class ConstantBuffer : public Bindable
{
public:
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		// ����Դ������Ҫд�������
		D3D11_MAPPED_SUBRESOURCE msr;
		// Ϊ�˸��³������棬��Ҫ�����Map��������ӳ����Դʱ������
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD,0u,
			&msr
		));
		// д���ڴ�
		memcpy(msr.pData, &consts, sizeof(consts));
		// д����� 
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	//���캯��1 ���볣�����������г�ʼ��
	ConstantBuffer(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;			//����������÷��������Ƕ�̬�� ÿ֡����
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;//CPUдȨ��
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(consts);
		cbd.StructureByteStride = 0u;				//Ϊ0���� �����

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &consts;
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
	}
	//�汾2 û�г�ʼ��
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

// ע�� �̳�һ��ģ���� ����ֱ��ʹ���䱣����Ա
// ���Ҫ�ã�2�ַ�����  1��using ����::��Ա   2�����߳�Աǰ���this����this->GetContext()
template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
private:
	using ConstantBuffer<C>::pConstantBuffer; 
	using Bindable::GetContext;				  
public:
	using ConstantBuffer<C>::ConstantBuffer; //���ṩ���캯����ʹ�û���Ĺ��캯�� ��ʼ������
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
	using ConstantBuffer<C>::ConstantBuffer; //���ṩ���캯����ʹ�û���Ĺ��캯�� ��ʼ������
	void Bind(Graphics& gfx) noexcept override
	{
		GetContext(gfx)->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};