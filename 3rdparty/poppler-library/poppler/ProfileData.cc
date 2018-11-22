//========================================================================
//
// ProfileData.cc
//
// Copyright 2005 Jonathan Blandford <jrb@gnome.org>
// Copyright 2018 Adam Reichold <adam.reichold@t-online.de>
//
//========================================================================

#if defined (_MSC_VER)
#include "../config.h"
#else
#include "../config.h"
#endif

#include "ProfileData.h"

//------------------------------------------------------------------------
// ProfileData
//------------------------------------------------------------------------

void
ProfileData::addElement (double elapsed) {
	if (count == 0) {
		min = elapsed;
		max = elapsed;
	} else {
		if (elapsed < min)
			min = elapsed;
		if (elapsed > max)
			max = elapsed;
	}
	total += elapsed;
	count ++;
}

