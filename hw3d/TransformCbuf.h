#pragma once
#include "ConstantBuffers.h"
#include "Drawable.h"
#include <DirectXMath.h>
class TransformCbuf : public Bindable
{
public:
	//�βλ�ȡDrawble����󣬴��������������ķ���
	TransformCbuf(Graphics& gfx, const Drawable& parent);
	void Bind(Graphics& gfx) noexcept override;  //����bind ���Ȼ�ȡDrawable���� �еľ��� ����ͶӰ�任�� �õ����յ� ���ڶ�����ɫ����vsbuf
private:
	VertexConstantBuffer<DirectX::XMMATRIX> vcbuf; //������ɫ�����õ������������ �ڲ��Դ�bind�������԰󶨵���Ⱦ��Ⱦ����
	const Drawable& parent;  //����ĸ��Ҳ���������Ǹ�ĸ��������parent
};

