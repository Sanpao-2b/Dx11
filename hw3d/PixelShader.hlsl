/*
	记得改执行文件输出目录，和着色器类型
	输出：每个像素颜色
	输入：不需要，因为光栅化阶段已经确定每个像素的位置，然后像素着色器在每个像素上进行颜色指定
*/
//我们要用自己在外面定义的颜色 所以要接收输入值
float4 main(float3 color : Color) : SV_Target
{
	return float4(color, 1.0f); //这个颜色是光栅化器插值过后的颜色
}