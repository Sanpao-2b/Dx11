#include "TransformCbuf.h"

TransformCbuf::TransformCbuf(Graphics & gfx, const Drawable & parent)
	:
	vcbuf(gfx),
	parent(parent)
{}

void TransformCbuf::Bind(Graphics & gfx) noexcept
{
	//�󶨵���Ⱦ����֮ǰ���ȸ���һ�£���Ϊ����Drawable�Ǳߵ�С�����ǻ�Ҫ�ı任ɶ��
	vcbuf.Update(gfx, 
		DirectX::XMMatrixTranspose(
			parent.GetTransformXM() * gfx.GetProjection()));//�������Ȼ�ȡDrawable�����еı任�����ͼ�����е�ͶӰ���� ��˺�õ���VertexShaderʹ�õı任����
	vcbuf.Bind(gfx);  //ֱ�Ӱ󶨵���Ⱦ����
}
