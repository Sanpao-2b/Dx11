#pragma once


#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, hr)

#ifndef NDEBUG //����ǵ���ģʽ ������������ ����ģʽ�ȷ���ģʽ�������infoManager��ץ���Ķ����������ڵ���ϸ������Ϣ
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__, __FILE__, (hr), infoManager.GetMessages())  //���մ�����룬����Exception����Ĺ��캯�������Ҵ�����infoManager��ץ����������ڵ���Ϣ
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPT(hr)	//ע���������˼�ǣ�����D3D����ǰ �ȵ���Set() ֮���ֻ���ȡ���й�������õ������������Ϣ
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.GetMessages()) //����Ҳͬ�ϣ����⴫��ոմ����õ� InfoManager��ץ��������Ϣ
//Ҳ�ǽ���infoManager���е�ץ���ڵķ�����ץ�ĺ�����ͬ ����ץ��Щ������HRESULT �ĺ���
//���̺��������������Ϊ����ֵ�� ֱ���滻�ɺ�����һ�󴮣����ú���ǰ��set()��¼������ڵĵ�ǰ��Ϣ��->���ô���ĺ���->��v��ȡ��ǰ��������²�������Ϣ->if�ж�����²�������Ϣ��˵������->�׳��쳣
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else          //������Ƿ���ģʽ��������������
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILE__, (hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

// ����ĺ����ڵ���infomanager ������������
// this.GetInfoManager(Graphics& gfx) ����д����
#ifdef NDEBUG
#define INFOMAN(gfx) HRESULT hr
#else //����ǵ���ģʽ�� �㵽Graphics�е� infoManager����
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif