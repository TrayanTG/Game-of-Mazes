#pragma once
#include <locale>
#include <codecvt>
#include <string>

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

struct Message
{
private:

public:
	std::wstring text;
	float durotation;
	Message(const std::wstring &text, float durotation) : text(text), durotation(durotation) {}
	Message(const std::string &text, float durotation) : text(converter.from_bytes(text)), durotation(durotation)
	{
		//std::cout<< converter.to_bytes(this->text) << std::endl;system("pause");
	}
};