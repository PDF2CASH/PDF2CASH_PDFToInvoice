#pragma once

#include "poppler-config.h"

#include "../goo/gtypes.h"
#include "../goo/GooString.h"
#include "GlobalParams.h"

class GooString;

class SysFontInfo {
public:

	GooString * name;
	GBool bold;
	GBool italic;
	GBool oblique;
	GBool fixedWidth;
	GooString *path;
	SysFontType type;
	int fontNum;			// for TrueType collections
	GooString *substituteName;

	SysFontInfo(GooString *nameA, GBool boldA, GBool italicA, GBool obliqueA, GBool fixedWidthA,
		GooString *pathA, SysFontType typeA, int fontNumA, GooString *substituteNameA);
	~SysFontInfo();
	SysFontInfo(const SysFontInfo &) = delete;
	SysFontInfo& operator=(const SysFontInfo&) = delete;
	GBool match(SysFontInfo *fi);
	GBool match(GooString *nameA, GBool boldA, GBool italicA, GBool obliqueA, GBool fixedWidthA);
	GBool match(GooString *nameA, GBool boldA, GBool italicA);
};
