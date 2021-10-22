#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacros.h"

//��ģ��,
template<typename C> 
class ConstantBuffer : public Bindable
{
public:
	//��������һ��Ҫÿ֡����(����consts���ݸ������ǵĻ���) �����и�Update()����
	void Update(Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		// ����һ������Դ������Դ����һ��void*���͵�ָ�룬�����������ֻҪ����pData��Ա,������д��msr.pDataָ����ڴ���
		D3D11_MAPPED_SUBRESOURCE msr;

		// Ϊ�˸��³������棬��Ҫ�����Map��������CPU��GPU�Ļ�����д���ݣ� map()�����Ǻ���Ҫ�Ĵ���
		GFX_THROW_INFO(GetContext(gfx)->Map(
			pConstantBuffer.Get(),  //ָ��ID3D11Resource��ָ�룬ָ��������Ҫд������ȥ���ǿ黺��
			0u,						//����Դ��������
			D3D11_MAP_WRITE_DISCARD,//��Դ��ӳ������д�룻��Դ����ǰ���ݽ���δ����ġ���Դ������ͨ��д����
			0u,						//ָ��CPU��GPU������ʱ����Ը�ʲô
			&msr					//����ô�Ӧ�þ���ָ�� pConstantBufferָ����ǿ��ڴ�ɣ�
		));
		// ��consts�����������ݣ�д��"��ӳ������Դ"msr.pDataָ����ڴ�
		memcpy(msr.pData, &consts, sizeof(consts));
		// д������󣬵���UnMap()������ ������������CPUȨ���ͷ�
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	//���캯��1 ���볣���������� ���г�ʼ��
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
	//�汾2 ����bufferʱ û�г�ʼ����
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

// ע�� �̳�һ��ģ���� ����ֱ��ʹ���䱣����Ա ���Ҫ�ã�2�ַ�����  1��using ����::��Ա   2�����߳�Աǰ���this����this->��Ա

// ������ɫ������ �������̶�һ���� Ψһ��ͬ���ǰ󶨵Ľ׶β�ͬ
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
		GetContext(gfx)->PSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());
	}
};