/*
	shader就是一个函数，给定输入，返回输出
	shader至少要输出一个位置，必须是齐次坐标
	顶点着色器的入口函数是main 可以在属性里面修改，记得还要在属性里指定————————此着色器类型为"Vertex Shader"
	写完后，编译此文件会生成一个.cso文件，这是个二进制文件， 这就是运行时要加载的一个文件，将其绑定到渲染管线，用某个函数去加载它
	1>------ 已启动生成: 项目: hw3d, 配置: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//定义结构体实现2个输出的语义
struct VSOut
{
	float3 color: Color;
	float4 pos: SV_Position;
};

// 使用常数缓存 cbuffer是关键字 matrix是4*4 float
// 没赋值为啥就能用，猜测：我们在绘制函数里 把常数缓存绑定到顶点着色器了，所以可能这里的transform可以直接自动赋值？？？
cbuffer cBuf
{
	matrix transform;
};

//我们定义的颜色是4个参数RGBA ，这里只拿3个 忽略第四个是可以的
VSOut main(float2 pos : Position, float3 color : Color)
{
	VSOut vso;
	vso.pos = mul(float4(pos.x, pos.y, 1.0f, 1.0f), transform);//注意是右乘，因为这是行向量，1*4 · 4*4 = 1*4 
	vso.color = color;   
	return vso;
}