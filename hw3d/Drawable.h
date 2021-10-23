#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
	template<class T>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept(!IS_DEBUG);
	virtual ~Drawable() = default;
private:
	// Drawable����Ҫ���������е�StaticBinds ���Ը����Ԫ ����һ���麯�� ���᷵��һ�������е��Ǹ���̬binds,�˺������Ǿ�̬�Ͷ�̬���ӵ�����
	virtual const std::vector<std::unique_ptr<Bindable>> & GetStaticBinds() const noexcept = 0;
	
private:
	// ���ָ����ڵ�Ŀ���ǣ���Щbindable������������������ҪΪ���ǻ���������
	// ע��ÿһ��Drawableʵ�� ֻ�ܴ���һ���������棬���忴AddIndexBuffer���� ֻ�����һ�� 
	const class IndexBuffer* pIndexBuffer = nullptr;
	// Ψһָ�� ��ΪҪ�õ���̬����ŵ���Bindable���͵�ָ�� Bindable�Ǹ������ͣ������ǻ��桢���벼�֡���ɫ�����ȵȶ���
	// ֮����ѭ���������binds ������������ŵ���Щָ��ָ���Bindable�ڲ���binds����(��̬) ����������
	// binds�����д����Ĵ��󶨵Ķ�������֪��˭��˭ ֻ֪�� �������Ҫ���и������е�bind()
	std::vector<std::unique_ptr<Bindable>> binds;  
};