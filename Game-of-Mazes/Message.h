#pragma once
#include <locale>
#include <codecvt>
#include <string>

std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter; //required to convert string<->wstring

struct Message
{
	std::wstring text;
	float duration;
	Message(const std::wstring &text, float duration): text(text), duration(duration) {}
	Message(const std::string &text, float duration): text(converter.from_bytes(text)), duration(duration)
	{
		//std::cout<< converter.to_bytes(this->text) << std::endl;system("pause");
	}
};