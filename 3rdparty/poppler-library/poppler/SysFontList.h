#pragma once

#if defined (_MSC_VER)
#include "../config.h"
#else
#include "../config.h"
#endif

#include "../goo/gtypes.h"
#include "../goo/GooString.h"

class SysFontInfo;
class GooString;
class GooList;

class SysFontList {
public:

	SysFontList();
	~SysFontList();
	SysFontList(const SysFontList &) = delete;
	SysFontList& operator=(const SysFontList &) = delete;
	SysFontInfo *find(const GooString *name, GBool isFixedWidth, GBool exact);

#ifdef _WIN32
	void scanWindowsFonts(GooString *winFontDir);
#endif
#ifdef WITH_FONTCONFIGURATION_FONTCONFIG
	void addFcFont(SysFontInfo *si) { fonts->push_back(si); }
#endif
private:

#ifdef _WIN32
	SysFontInfo *makeWindowsFont(const char *name, int fontNum,
		const char *path);
#endif

	GooList *fonts;			// [SysFontInfo]
};
