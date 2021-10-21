#pragma once


#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG //如果是调试模式 采用这三个宏 调试模式比发布模式多出了由infoManager类抓到的额外的输出窗口的详细错误信息
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__, __FILE__, (hr), infoManager.GetMessages())  //接收错误代码，传给Exception基类的构造函数，并且传入由infoManager类抓到的输出窗口的信息
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)	//注意这里的意思是：包裹D3D函数前 先调用Set() 之后就只会获取到有关最近调用的这个函数的信息
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages()) //这里也同上，额外传入刚刚创建好的 InfoManager类抓出来的信息
//也是借用infoManager类中的抓窗口的方法，抓的函数不同 这里抓那些不返回HRESULT 的函数
//流程宏接收整个函数作为输入值， 直接替换成后面这一大串：调用函数前先set()记录输出窗口的当前信息数->调用传入的函数->用v获取当前输出窗口新产生的消息->if判断如果新产生了消息则说明出错->抛出异常
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else          //否则就是发布模式，采用这三个宏
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// 下面的宏用于导入infomanager 到本作用域中
// this.GetInfoManager(Graphics& gfx) 必须写出来
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else //如果是调试模式下 搞到Graphics中的 infoManager变量
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif