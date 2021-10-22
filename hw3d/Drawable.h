#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept;
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	virtual ~Drawable() = default;
private:
	// ���ָ����ڵ�Ŀ���ǣ���Щbindable������������������ҪΪ���ǻ���������
	// ע��ûһ��Drawableʵ�� ֻ�ܴ���һ���������棬���忴AddIndexBuffer���� ֻ�����һ�� 
	const IndexBuffer* pIndexBuffer = nullptr;
	// Ψһָ�� ��ΪҪ�õ���̬����ŵ���Bindable���͵�ָ�� Bindable�Ǹ������ͣ������ǻ��桢���벼�֡���ɫ�����ȵȶ���
	// ֮����ѭ���������binds ������������ŵ���Щָ��ָ���Bindable�ڲ���binds����(��̬) ����������
	// binds�����д����Ĵ��󶨵Ķ�������֪��˭��˭ ֻ֪�� �������Ҫ���и������е�bind()
	std::vector<std::unique_ptr<Bindable>> binds; 
};