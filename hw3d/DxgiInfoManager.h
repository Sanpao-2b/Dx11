#pragma once
#include "ChiliWin.h"
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>


class DxgiInfoManager
{
public:
	DxgiInfoManager(); //�ӿ����ҳ�DXGIGetDebugInterface()����ӿڣ���ȡ�����Զ���(���������ڵĵ�����Ϣ��һ������)�������Ƕ����ָ��pDxgiInfoQueueָ����
	~DxgiInfoManager() = default;//�ͷ��Ǹ����Զ���

	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void Set() noexcept; //Set()��ÿ�ε���D3D����֮ǰ����һ�Σ���¼��ǰ��Ϣ�����Ƕ��٣�����¼
	std::vector<std::string> GetMessages() const; //������Ϣ����  ץ���ոյ��õ�D3D���� �������ĵ��������Ϣ����ŵ�һ��string������ ������
private:
	unsigned long long next = 0u;

	//���Ҫ��ȡ������ڵ���Ϣ����Ҫ�������ָ�����������Ϣ���� ������cpp�ļ��п��Կ���
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> pDxgiInfoQueue;
};

