#pragma once
#include "Drawable.h"


class Box : public Drawable
{
public:
	//mt19937是用来产生随机数的   uniform_real_distribution 浮点数均匀分布
	//这个构造函数用于实例化盒子对象滴，并且属性全是随机
	Box( Graphics& gfx, 
		std::mt19937& rng,
		std::uniform_real_distribution<float>& adist,
		std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist,
		std::uniform_real_distribution<float>& rdist);
	// 更新参数
	void Update(float dt) noexcept override;
	// 把参数转化成DXMATRIX格式
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	float r; //半径
	// 位置
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	// 角度相关
	float theta = 0.0f;	//希腊字母θ
	float phi;			//希腊字母φ
	float chi;			//希腊字母X
	// 速度 (delta/s)
	// 相对旋转速度
	float droll;
	float dpitch;
	float dyaw;
	// 世界旋转速度
	float dtheta;
	float dphi;
	float dchi;
};

