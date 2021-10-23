#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	parent(parent)
{
	if (!pVcbuf) //��鳣���������Ƿ��Ѿ����䣬���û���� �Ǿͷ���һ��
	{
		pVcbuf = std::make_unique<VertexConstantBuffer<DirectX::XMMATRIX>>(gfx); //����gfx�����ǵ���VertexConstantBuffer���ĸ���е� �ڶ������캯��������buffer
	}
}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//�󶨵���Ⱦ����֮ǰ������ͶӰ
	pVcbuf->Update(gfx, 
		DirectX::XMMatrixTranspose( //ת��
			parent.GetTransformXM() * gfx.GetProjection()));//�������Ȼ�ȡDrawable�����еı任�����gfx�е�ͶӰ���� ��˺�õ���VertexShaderʹ�õı任����
	pVcbuf->Bind(gfx);  //ֱ�Ӱ󶨵���Ⱦ����
}
std::unique_ptr<VertexConstantBuffer<DirectX::XMMATRIX>> TransformCbuf::pVcbuf;