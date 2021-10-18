/*
	shader就是一个函数，给定输入，返回输出
	shader至少要输出一个位置，必须是齐次坐标
	顶点着色器的入口函数是main 可以在属性里面修改，记得还要在属性里指定————————此着色器类型为"Vertex Shader"
	写完后，编译此文件会生成一个.cso文件，这是个二进制文件， 这就是运行时要加载的一个文件，将其绑定到渲染管线，用某个函数去加载它
	1>------ 已启动生成: 项目: hw3d, 配置: Debug Win32 ------
	1>compilation object save succeeded; see F:\Project_VS2017\Repos\D3d_Pzh\hw3d\Debug\VertexShader.cso
*/

//定义结构体实现2个输出的语义

/*
	如果这个结构体中 position放在前面，运行后像素着色器会出错，
	你可能会觉得 我们在输入布局中定义了语义，他应该知道从哪里开始是颜色数据，但他不会，他只是用语义检查是否匹配，而不管读取顺序，
	所以呢这里VSOut 我们要把颜色放在前面
*/
struct VSOut
{
	float3 color: Color;
	float4 pos: SV_Position;
};

//注意输出语义是SV_Position，输入语义是InputLayout自定义的
VSOut main(float2 pos : Position, float3 color : Color)
{
	VSOut vso;
	vso.pos = float4(pos.x, pos.y, 1.0f, 1.0f); //因为要扩充2个数 所以不能直接=pos
	vso.color = color;   //因为color就是rgb三个值 跟vso.color一样 所以直接=
	return vso;
	//我们给定了顶点的颜色，最后Rasterizer会进行插值 把三角形内部点进行上色
}