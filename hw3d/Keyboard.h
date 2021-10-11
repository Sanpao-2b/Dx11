#pragma once
#include <queue>
#include <bitset>



//������ʵ������Window���һ����չ�� ���Լ��̰Ѵ��ڿ�����Ԫ ������Ϊ����Ϊ���е���� 
class Keyboard
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept
			:
			type(type),
			code(code)
		{}
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default; //Ĭ�Ϲ��캯��
	Keyboard(const Keyboard&) = delete;
	Keyboard operator=(const Keyboard&) = delete;
	
	//Key Event stuff
	bool KeyIsPressed(unsigned char keycode) const noexcept; //����һ�����룬�������Ƿ񱻰���
	Event ReadKey() noexcept;          //��ȡ"�����¼�����"�е��¼�
	bool KeyIsEmpty() const noexcept;  //��顰�����¼����С��Ƿ�Ϊ��
	void FlushKey() noexcept;          //�������
	
	//Char Event stuff
	char ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	
	void Flush() noexcept; //�൱��FlushKeyandChar()
	
	//autorepeat control  �Զ����ƺ���
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
	
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;    

	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;

private:
	static constexpr unsigned int nKeys = 256u; 
	static constexpr unsigned int bufferSize = 16u;
	
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates; //����256λ��bool
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;
};
