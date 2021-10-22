#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//�󶨵���Ⱦ����֮ǰ������ͶӰ
	vcbuf.Update(gfx, 
		DirectX::XMMatrixTranspose( //ת��
			parent.GetTransformXM() * gfx.GetProjection()));//�������Ȼ�ȡDrawable�����еı任�����gfx�е�ͶӰ���� ��˺�õ���VertexShaderʹ�õı任����
	vcbuf.Bind(gfx);  //ֱ�Ӱ󶨵���Ⱦ����
}
