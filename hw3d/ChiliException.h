#pragma once
#include <exception>
#include <string>
//�Զ����һ���쳣����
class ChiliException :public std::exception
{
public:
	ChiliException(int line, const char* file) noexcept;
	const char* what() const noexcept override;  //���س������Ϣ����
	virtual const char* GetType() const noexcept;//��������
	int GetLine() const noexcept;                //������
	const std::string& GetFile() const noexcept; //�����ļ�
	std::string GetOriginString() const noexcept;//�Ѵ������͡��С��ļ� ��ʽ�����ַ��� 
private:
	int line;        //���������
	std::string file;//������ļ�
protected: //���ӿ��Է���
	mutable std::string whatBuffer; 
};

