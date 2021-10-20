
//定义常数缓存，缓存中存放的是 一个float4类型的数组
cbuffer CBuf
{
	float4 face_colors[6];
};


//uint 无符号整型
//SV_PritiveID 是系统变量 原始ID 这会让渲染管线给每个三角形生成原始ID(从0开始到三角形个数-1)
//遍历所有三角形ID 映射到常数缓存中存放的颜色数组中 返回对应颜色 
float4 main( uint tid : SV_PrimitiveID) : SV_Target
{
	return face_colors[tid / 2];// 除以2是因为每个面都有两个三角形 所以让挨着的两个三角形映射到同一个颜色 
}