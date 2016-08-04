#pragma once
#include <path.h>
#include <string>
#include <cstdint>
class PlatformString
{
	std::string _utf8str;

	static std::string ConvertAnsiToUTF8(const std::string& ansiText);
public:
	PlatformString();
	PlatformString(const PlatformString& old);
	PlatformString(const std::wstring& unicodeText);
	PlatformString(const wchar_t* unicodeText);
	

#if _WIN32
	static PlatformString FromCharString(const std::string& ansiText);
#elif _IRR_ANDROID_PLATFORM_
	static PlatformString FromCharString(const std::string& utf8Text);
#endif

	static PlatformString FromUTF8(const std::string& utf8Text);

	static PlatformString FromUnicode(const std::wstring& unicodeText);

	static PlatformString FromUTF16(const std::u16string& utf16Text);

	static PlatformString FromPath(const irr::io::path& path);

	PlatformString operator + (const PlatformString& rhs);

	void Append(const PlatformString& rhs);

	std::string ToUTF8() const;

	// WINDOWS ·µ»Ø±¾µØ×Ö·û¼¯±àÂë×Ö·û´®£¬ ANDROID ·µ»ØUTF8×Ö·û´®
	std::string ToString() const;

	std::wstring ToWString() const;

	irr::io::path ToPath() const;

	irr::core::stringc ToIrrString() const;

	irr::core::stringw ToIrrStringW() const;

	std::u16string ToUTF16() const;

	bool Compare(const PlatformString& rhs) const;
};

bool operator <(const PlatformString& lhs, const PlatformString& rhs);