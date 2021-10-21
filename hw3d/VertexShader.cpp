#include "VertexShader.h"
#include "GraphicsThrowMacros.h"
VertexShader::VertexShader(Graphics & gfx, const std::wstring & path)
{
	INFOMAN(gfx); //这个Graphics中的infoManager变量取到了， 下面的抛出异常宏才能正常工作

	GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBytecodeBlob));
	GFX_THROW_INFO(GetDevice(gfx)->CreateVertexShader(
		pBytecodeBlob->GetBufferPointer(),
		pBytecodeBlob->GetBufferSize(),
		nullptr,
		&pVertexShader
	));
}

void VertexShader::Bind(Graphics & gfx) noexcept
{
	GetContext(gfx)->VSSetShader(pVertexShader.Get(), nullptr, 0u);
}

ID3DBlob * VertexShader::GetBytecode() const noexcept
{
	return pBytecodeBlob.Get();
}
