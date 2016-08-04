#include "PlatformString.h"
#include <clocale>
#include <locale>
#include <irrlicht.h>
#include <codecvt>
using namespace std;

#if _MSC_VER == 1900

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto p = reinterpret_cast<const int16_t *>(utf16_string.data());
	return convert.to_bytes(p, p + utf16_string.size());
}

std::u16string utf8_to_utf16(std::string utf8_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<int16_t>, int16_t> convert;
	auto res = convert.from_bytes(utf8_string);
	std::u16string s(res.begin(), res.end());
	return s;
}

#else

std::string utf16_to_utf8(std::u16string utf16_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.to_bytes(utf16_string);
}

std::u16string utf8_to_utf16(std::string utf8_string)
{
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(utf8_string);
}

#endif


PlatformString::PlatformString()
{

}

bool PlatformString::Compare(const PlatformString& rhs) const
{
	return _utf8str < rhs._utf8str;
}

bool operator <(const PlatformString& lhs, const PlatformString& rhs)
{
	return lhs.Compare(rhs);
}

PlatformString::PlatformString(const PlatformString& old)
{
	_utf8str = old._utf8str;
}

PlatformString::PlatformString(const std::wstring& unicodeText)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conversion;
	_utf8str = conversion.to_bytes(unicodeText);
}

PlatformString::PlatformString(const wchar_t* unicodeText) :
	PlatformString(std::wstring(unicodeText))
{

}

template <class Facet>
class usable_facet
	: public Facet
{
public:
	template <class ...Args>
	usable_facet(Args&& ...args)
		: Facet(std::forward<Args>(args)...) {}
	~usable_facet() {}
};

std::string PlatformString::ConvertAnsiToUTF8(const std::string& ansiText)
{
	std::codecvt<wchar_t, char, std::mbstate_t>* cvt = new std::codecvt_byname<wchar_t, char, std::mbstate_t>(std::locale("").name().c_str());
	typedef usable_facet<std::codecvt<wchar_t, char, std::mbstate_t>> C;
	std::wstring_convert<C> cvt_ansi(new C(std::locale("").name().c_str()));
	std::wstring unicodeText = cvt_ansi.from_bytes(ansiText);
	return FromUnicode(unicodeText)._utf8str;
}

#if _WIN32
PlatformString PlatformString::FromCharString(const std::string& ansiText)
{
	PlatformString str;
	auto utf8str = ConvertAnsiToUTF8(ansiText);
	str._utf8str = utf8str;
	return str;
}
#elif _IRR_ANDROID_PLATFORM_
PlatformString PlatformString::FromCharString(const std::string& utf8Text)
{
	return FromUTF8(utf8Text);
}
#endif

PlatformString PlatformString::FromUTF8(const std::string& utf8Text)
{
	PlatformString str;
	str._utf8str = utf8Text;
	return str;
}

PlatformString PlatformString::FromUnicode(const std::wstring& unicodeText)
{
	PlatformString str;
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conversion;
	str._utf8str = conversion.to_bytes(unicodeText);
	return str;
}

PlatformString PlatformString::operator + (const PlatformString& rhs)
{
	PlatformString str;
	str._utf8str = _utf8str + rhs._utf8str;
	return str;
}

void PlatformString::Append(const PlatformString& rhs)
{
	_utf8str += rhs._utf8str;
}

std::string PlatformString::ToUTF8() const
{
	return _utf8str;
}

// WINDOWS ·µ»Ø±¾µØ×Ö·û¼¯±àÂë×Ö·û´®£¬ ANDROID ·µ»ØUTF8×Ö·û´®
std::string PlatformString::ToString() const
{
#if _WIN32
	std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> cvt_ansi(new std::codecvt<wchar_t, char, std::mbstate_t>(std::locale("").c_str()));//GBK<->Unicode×ª»»Æ÷
	std::string ansiText = cvt_ansi.to_bytes(ToWString());
	return ansiText;
#else
	return _utf8str;
#endif
}

std::wstring PlatformString::ToWString() const
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utf8_wchar_cvt;
	auto s = utf8_wchar_cvt.from_bytes(_utf8str);
	return s;
}

irr::io::path PlatformString::ToPath() const
{
#ifdef _IRR_WCHAR_FILESYSTEM
	return ToWString().c_str();
#else
	return ToString().c_str();
#endif
}

irr::core::stringc PlatformString::ToIrrString() const
{
	return ToString().c_str();
}

irr::core::stringw PlatformString::ToIrrStringW() const
{
	return ToWString().c_str();
}

PlatformString PlatformString::FromUTF16(const std::u16string& utf16Text)
{
	PlatformString str;
	str._utf8str = utf16_to_utf8(utf16Text);
	return str;
}

PlatformString PlatformString::FromPath(const irr::io::path& path)
{

#ifdef _IRR_WCHAR_FILESYSTEM
	return PlatformString::FromUnicode(path.c_str());
#else
	return PlatformString::FromCharString(path.c_str());
#endif
}

std::u16string PlatformString::ToUTF16() const
{
	return utf8_to_utf16(_utf8str);
}