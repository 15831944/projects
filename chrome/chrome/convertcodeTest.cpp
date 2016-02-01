#include "convertcode.h"

int main()
{
	std::string strUTF8 = "asdf";
	CConvertCode::UTF8ToUnicode(strUTF8, strUTF8.length() );

	std::wstring strUnicode = L"asdf";
	CConvertCode::UnicodeToUTF8(strUnicode, strUnicode.length());

	return 0;
}