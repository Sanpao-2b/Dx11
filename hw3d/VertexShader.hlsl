// 没赋值为啥就能用，猜测：我们在绘制函数里 把常数缓存绑定到顶点着色器了，所以可能这里的transform可以直接自动赋值？？？
cbuffer cBuf
{
	matrix transform;
};

//不用再接收颜色变量了，顶点不需要颜色的输入输出了，具体的上色工作 交给像素着色器了
float4 main(float3 pos : Position) : SV_Position
{
	return mul(float4(pos, 1.0f), transform);
}