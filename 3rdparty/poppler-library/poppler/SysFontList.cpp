
#if defined (_MSC_VER)
#include "../config.h"
#else
#include "../config.h"
#endif

#include "../goo/gtypes.h"
#include "../goo/GooList.h"

#include "SysFontList.h"
#include "SysFontInfo.h"

#if defined(_WIN32)
#include <windows.h>
#if !(_WIN32_IE >= 0x0500)
#error "_WIN32_IE must be defined >= 0x0500 for SHGFP_TYPE_CURRENT from shlobj.h"
#endif
#endif

#if !defined(_WIN32)
#include <stdio.h>
#include <string.h>
#endif

SysFontList::SysFontList() {
	fonts = new GooList();
}

SysFontList::~SysFontList() {
	deleteGooList<SysFontInfo>(fonts);
}

SysFontInfo *SysFontList::find(const GooString *name, GBool fixedWidth, GBool exact) {
	GooString *name2;
	GBool bold, italic, oblique;
	SysFontInfo *fi;
	char c;
	int n, i;

	name2 = name->copy();

	// remove space, comma, dash chars
	i = 0;
	while (i < name2->getLength()) {
		c = name2->getChar(i);
		if (c == ' ' || c == ',' || c == '-') {
			name2->del(i);
		}
		else {
			++i;
		}
	}
	n = name2->getLength();

	// remove trailing "MT" (Foo-MT, Foo-BoldMT, etc.)
	if (n > 2 && !strcmp(name2->getCString() + n - 2, "MT")) {
		name2->del(n - 2, 2);
		n -= 2;
	}

	// look for "Regular"
	if (n > 7 && !strcmp(name2->getCString() + n - 7, "Regular")) {
		name2->del(n - 7, 7);
		n -= 7;
	}

	// look for "Italic"
	if (n > 6 && !strcmp(name2->getCString() + n - 6, "Italic")) {
		name2->del(n - 6, 6);
		italic = gTrue;
		n -= 6;
	}
	else {
		italic = gFalse;
	}

	// look for "Oblique"
	if (n > 6 && !strcmp(name2->getCString() + n - 7, "Oblique")) {
		name2->del(n - 7, 7);
		oblique = gTrue;
		n -= 6;
	}
	else {
		oblique = gFalse;
	}

	// look for "Bold"
	if (n > 4 && !strcmp(name2->getCString() + n - 4, "Bold")) {
		name2->del(n - 4, 4);
		bold = gTrue;
		n -= 4;
	}
	else {
		bold = gFalse;
	}

	// remove trailing "MT" (FooMT-Bold, etc.)
	if (n > 2 && !strcmp(name2->getCString() + n - 2, "MT")) {
		name2->del(n - 2, 2);
		n -= 2;
	}

	// remove trailing "PS"
	if (n > 2 && !strcmp(name2->getCString() + n - 2, "PS")) {
		name2->del(n - 2, 2);
		n -= 2;
	}

	// remove trailing "IdentityH"
	if (n > 9 && !strcmp(name2->getCString() + n - 9, "IdentityH")) {
		name2->del(n - 9, 9);
		n -= 9;
	}

	// search for the font
	fi = nullptr;
	for (i = 0; i < fonts->getLength(); ++i) {
		fi = (SysFontInfo *)fonts->get(i);
		if (fi->match(name2, bold, italic, oblique, fixedWidth)) {
			break;
		}
		fi = nullptr;
	}
	if (!fi && !exact && bold) {
		// try ignoring the bold flag
		for (i = 0; i < fonts->getLength(); ++i) {
			fi = (SysFontInfo *)fonts->get(i);
			if (fi->match(name2, gFalse, italic)) {
				break;
			}
			fi = nullptr;
		}
	}
	if (!fi && !exact && (bold || italic)) {
		// try ignoring the bold and italic flags
		for (i = 0; i < fonts->getLength(); ++i) {
			fi = (SysFontInfo *)fonts->get(i);
			if (fi->match(name2, gFalse, gFalse)) {
				break;
			}
			fi = nullptr;
		}
	}

	delete name2;
	return fi;
}

#ifdef _WIN32
void SysFontList::scanWindowsFonts(GooString *winFontDir) {
	OSVERSIONINFO version;
	const char *path;
	DWORD idx, valNameLen, dataLen, type;
	HKEY regKey;
	char valName[1024], data[1024];
	int n, fontNum;
	char *p0, *p1;
	GooString *fontPath;

	version.dwOSVersionInfoSize = sizeof(version);

#pragma warning(push)
#pragma warning(disable: 4996)
	GetVersionEx(&version);
#pragma warning(pop)

	if (version.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		path = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts\\";
	}
	else {
		path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Fonts\\";
	}
	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, path, 0,
		KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,
		&regKey) == ERROR_SUCCESS) {
		idx = 0;
		while (1) {
			valNameLen = sizeof(valName) - 1;
			dataLen = sizeof(data) - 1;
			if (RegEnumValueA(regKey, idx, valName, &valNameLen, nullptr,
				&type, (LPBYTE)data, &dataLen) != ERROR_SUCCESS) {
				break;
			}
			if (type == REG_SZ &&
				valNameLen > 0 && valNameLen < sizeof(valName) &&
				dataLen > 0 && dataLen < sizeof(data)) {
				valName[valNameLen] = '\0';
				data[dataLen] = '\0';
				n = strlen(data);
				if (!strcasecmp(data + n - 4, ".ttf") ||
					!strcasecmp(data + n - 4, ".ttc") ||
					!strcasecmp(data + n - 4, ".otf")) {
					fontPath = new GooString(data);
					if (!(dataLen >= 3 && data[1] == ':' && data[2] == '\\')) {
						fontPath->insert(0, '\\');
						fontPath->insert(0, winFontDir);
						fontPath->append('\0');
					}
					p0 = valName;
					fontNum = 0;
					while (*p0) {
						p1 = strstr(p0, " & ");
						if (p1) {
							*p1 = '\0';
							p1 = p1 + 3;
						}
						else {
							p1 = p0 + strlen(p0);
						}
						fonts->push_back(makeWindowsFont(p0, fontNum,
							fontPath->getCString()));
						p0 = p1;
						++fontNum;
					}
					delete fontPath;
				}
			}
			++idx;
		}
		RegCloseKey(regKey);
	}
}
#endif

#ifdef _WIN32
SysFontInfo *SysFontList::makeWindowsFont(const char *name, int fontNum,
	const char *path) {
	int n;
	GBool bold, italic, oblique, fixedWidth;
	GooString *s;
	char c;
	int i;
	SysFontType type;
	GooString substituteName;

	n = strlen(name);
	bold = italic = oblique = fixedWidth = gFalse;

	// remove trailing ' (TrueType)'
	if (n > 11 && !strncmp(name + n - 11, " (TrueType)", 11)) {
		n -= 11;
	}

	// remove trailing ' (OpenType)'
	if (n > 11 && !strncmp(name + n - 11, " (OpenType)", 11)) {
		n -= 11;
	}

	// remove trailing ' Italic'
	if (n > 7 && !strncmp(name + n - 7, " Italic", 7)) {
		n -= 7;
		italic = gTrue;
	}

	// remove trailing ' Oblique'
	if (n > 7 && !strncmp(name + n - 8, " Oblique", 8)) {
		n -= 8;
		oblique = gTrue;
	}

	// remove trailing ' Bold'
	if (n > 5 && !strncmp(name + n - 5, " Bold", 5)) {
		n -= 5;
		bold = gTrue;
	}

	// remove trailing ' Regular'
	if (n > 5 && !strncmp(name + n - 8, " Regular", 8)) {
		n -= 8;
	}

	// the familyname cannot indicate whether a font is fixedWidth or not.
	// some well-known fixedWidth typeface family names or keyword are checked.
	if (strstr(name, "Courier") ||
		strstr(name, "Fixed") ||
		(strstr(name, "Mono") && !strstr(name, "Monotype")) ||
		strstr(name, "Typewriter"))
		fixedWidth = gTrue;
	else
		fixedWidth = gFalse;


	//----- normalize the font name
	s = new GooString(name, n);
	i = 0;
	while (i < s->getLength()) {
		c = s->getChar(i);
		if (c == ' ' || c == ',' || c == '-') {
			s->del(i);
		}
		else {
			++i;
		}
	}

	if (!strcasecmp(path + strlen(path) - 4, ".ttc")) {
		type = sysFontTTC;
	}
	else {
		type = sysFontTTF;
	}

	return new SysFontInfo(s, bold, italic, oblique, fixedWidth,
		new GooString(path), type, fontNum, substituteName.copy());
}
#endif
