#pragma once
#include "Graphics.h"
#include <DirectXMath.h>

class Bindable;

class Drawable
{
public:
	Drawable() = delete;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noexcept(!IS_DEBUG);
	virtual void Update(float dt) noexcept = 0;
	void AddBind(std::unique_ptr<Bindable> bind) noexcept;
	void AddIndexBuffer(std::unique_ptr<class IndexBuffer> ibuf) noexcept;
	virtual ~Drawable() = default;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	//Ψһָ�� ��ΪҪ�õ���̬����ŵ���Bindable���͵�ָ�� Bindable�Ǹ������ͣ�����ʹ���桢���벼�֡���ɫ�����ȵȶ���
	std::vector<std::unique_ptr<Bindable>> binds; 
};