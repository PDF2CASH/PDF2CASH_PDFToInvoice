#if defined (_MSC_VER)
#include "../config.h"
#else
#include "../config.h"
#endif

#include "poppler-config.h"
#include <stdio.h>
#include "../goo/gtypes.h"
#include "CharTypes.h"

#include "SysFontInfo.h"

SysFontInfo::SysFontInfo(GooString *nameA, GBool boldA, GBool italicA, GBool obliqueA, GBool fixedWidthA,
	GooString *pathA, SysFontType typeA, int fontNumA, GooString *substituteNameA) {
	name = nameA;
	bold = boldA;
	italic = italicA;
	oblique = obliqueA;
	fixedWidth = fixedWidthA;
	path = pathA;
	type = typeA;
	fontNum = fontNumA;
	substituteName = substituteNameA;
}

SysFontInfo::~SysFontInfo() {
	delete name;
	delete path;
	delete substituteName;
}

GBool SysFontInfo::match(SysFontInfo *fi) {
	return !strcasecmp(name->getCString(), fi->name->getCString()) &&
		bold == fi->bold && italic == fi->italic && oblique == fi->oblique && fixedWidth == fi->fixedWidth;
}

GBool SysFontInfo::match(GooString *nameA, GBool boldA, GBool italicA, GBool obliqueA, GBool fixedWidthA) {
	return !strcasecmp(name->getCString(), nameA->getCString()) &&
		bold == boldA && italic == italicA && oblique == obliqueA && fixedWidth == fixedWidthA;
}

GBool SysFontInfo::match(GooString *nameA, GBool boldA, GBool italicA) {
	return !strcasecmp(name->getCString(), nameA->getCString()) &&
		bold == boldA && italic == italicA;
}