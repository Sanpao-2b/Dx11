/*
	shader就是一个函数，给定输入，返回输出
	shader至少要输出一个位置，必须是齐次坐标
	顶点着色器的入口函数是main 可以在属性里面修改，记得还要在属性里指定————————此着色器类型为"Vertex Shader"
	写完后，编译此文件会生成一个.cso文件，这是个二进制文件， 这就是运行时要加载的一个文件，将其绑定到渲染管线，用某个函数去加载它
	1>------ 已启动生成: 项目: hw3d, 配置: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//输入和输出跟普通函数不同，要多一个“语义标记”，如下
//注意输出标记SV_Position在函数后面 SV是系统变量的缩写
float4 main(float2 pos : Position) : SV_Position
{
	return float4(pos.x, pos.y, 0.0f, 1.0f);
}