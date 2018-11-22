/* Written by Krzysztof Kowalczyk (http://blog.kowalczyk.info)
   but mostly based on xpdf code.
   
   // Copyright (C) 2010, 2012 Hib Eris <hib@hiberis.nl>
   // Copyright (C) 2012, 2013 Thomas Freitag <Thomas.Freitag@alfa.de>
   // Copyright (C) 2012 Suzuki Toshiya <mpsuzuki@hiroshima-u.ac.jp>
   // Copyright (C) 2012, 2017 Adrian Johnson <ajohnson@redneon.com>
   // Copyright (C) 2012 Mark Brand <mabrand@mabrand.nl>
   // Copyright (C) 2013, 2018 Adam Reichold <adamreichold@myopera.com>
   // Copyright (C) 2013 Dmytro Morgun <lztoad@gmail.com>
   // Copyright (C) 2017 Christoph Cullmann <cullmann@kde.org>
   // Copyright (C) 2017, 2018 Albert Astals Cid <aacid@kde.org>
   // Copyright (C) 2018 Klarälvdalens Datakonsult AB, a KDAB Group company, <info@kdab.com>. Work sponsored by the LiMux project of the city of Munich

TODO: instead of a fixed mapping defined in displayFontTab, it could
scan the whole fonts directory, parse TTF files and build font
description for all fonts available in Windows. That's how MuPDF works.
*/

#ifndef PACKAGE_NAME
#if defined (_MSC_VER)
#include "../config.h"
#else
#include "../config.h"
#endif
#endif

#if defined(_WIN32)

#include <windows.h>
#if !(_WIN32_IE >= 0x0500)
#error "_WIN32_IE must be defined >= 0x0500 for SHGFP_TYPE_CURRENT from shlobj.h"
#endif
#include <shlobj.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

#include "../goo/gmem.h"
#include "../goo/GooString.h"
#include "../goo/GooList.h"
#include "../goo/gfile.h"
#include "Error.h"
#include "NameToCharCode.h"
#include "CharCodeToUnicode.h"
#include "UnicodeMap.h"
#include "CMap.h"
#include "BuiltinFontTables.h"
#include "FontEncodingTables.h"
#include "GlobalParams.h"
#include "GfxFont.h"
#include <sys/stat.h>
#include "Object.h"
#include "Stream.h"
#include "Lexer.h"
#include "Parser.h"
#include "SysFontInfo.h"
#include "SysFontList.h"

#define DEFAULT_SUBSTITUTE_FONT "Helvetica"
#define DEFAULT_CID_FONT_AC1_MSWIN "MingLiU"   /* Adobe-CNS1 for Taiwan, HongKong */
#define DEFAULT_CID_FONT_AG1_MSWIN "SimSun"    /* Adobe-GB1 for PRC, Singapore */
#define DEFAULT_CID_FONT_AJ1_MSWIN "MS-Mincho" /* Adobe-Japan1 */
#define DEFAULT_CID_FONT_AJ2_MSWIN "MS-Mincho" /* Adobe-Japan2 (legacy) */
#define DEFAULT_CID_FONT_AK1_MSWIN "Batang"    /* Adobe-Korea1 */
#define DEFAULT_CID_FONT_MSWIN "ArialUnicode"  /* Unknown */

static const struct {
    const char *name;
    const char *t1FileName;
    const char *ttFileName;
    GBool warnIfMissing;
} displayFontTab[] = {
    {"Courier",               "n022003l.pfb", "cour.ttf", gTrue},
    {"Courier-Bold",          "n022004l.pfb", "courbd.ttf", gTrue},
    {"Courier-BoldOblique",   "n022024l.pfb", "courbi.ttf", gTrue},
    {"Courier-Oblique",       "n022023l.pfb", "couri.ttf", gTrue},
    {"Helvetica",             "n019003l.pfb", "arial.ttf", gTrue},
    {"Helvetica-Bold",        "n019004l.pfb", "arialbd.ttf", gTrue},
    {"Helvetica-BoldOblique", "n019024l.pfb", "arialbi.ttf", gTrue},
    {"Helvetica-Oblique",     "n019023l.pfb", "ariali.ttf", gTrue},
    // TODO: not sure if "symbol.ttf" is right
    {"Symbol",                "s050000l.pfb", "symbol.ttf", gTrue},
    {"Times-Bold",            "n021004l.pfb", "timesbd.ttf", gTrue},
    {"Times-BoldItalic",      "n021024l.pfb", "timesbi.ttf", gTrue},
    {"Times-Italic",          "n021023l.pfb", "timesi.ttf", gTrue},
    {"Times-Roman",           "n021003l.pfb", "times.ttf", gTrue},
    // TODO: not sure if "wingding.ttf" is right
    {"ZapfDingbats",          "d050000l.pfb", "wingding.ttf", gTrue},

    // those seem to be frequently accessed by PDF files and I kind of guess
    // which font file do the refer to
    {"Palatino", nullptr, "pala.ttf", gTrue},
    {"Palatino-Roman", nullptr, "pala.ttf", gTrue},
    {"Palatino-Bold", nullptr, "palab.ttf", gTrue},
    {"Palatino-Italic", nullptr, "palai.ttf", gTrue},
    {"Palatino,Italic", nullptr, "palai.ttf", gTrue},
    {"Palatino-BoldItalic", nullptr, "palabi.ttf", gTrue},

    {"ArialBlack",        nullptr, "arialbd.ttf", gTrue},

    {"ArialNarrow", nullptr, "arialn.ttf", gTrue},
    {"ArialNarrow,Bold", nullptr, "arialnb.ttf", gTrue},
    {"ArialNarrow,Italic", nullptr, "arialni.ttf", gTrue},
    {"ArialNarrow,BoldItalic", nullptr, "arialnbi.ttf", gTrue},
    {"ArialNarrow-Bold", nullptr, "arialnb.ttf", gTrue},
    {"ArialNarrow-Italic", nullptr, "arialni.ttf", gTrue},
    {"ArialNarrow-BoldItalic", nullptr, "arialnbi.ttf", gTrue},

    {"HelveticaNarrow", nullptr, "arialn.ttf", gTrue},
    {"HelveticaNarrow,Bold", nullptr, "arialnb.ttf", gTrue},
    {"HelveticaNarrow,Italic", nullptr, "arialni.ttf", gTrue},
    {"HelveticaNarrow,BoldItalic", nullptr, "arialnbi.ttf", gTrue},
    {"HelveticaNarrow-Bold", nullptr, "arialnb.ttf", gTrue},
    {"HelveticaNarrow-Italic", nullptr, "arialni.ttf", gTrue},
    {"HelveticaNarrow-BoldItalic", nullptr, "arialnbi.ttf", gTrue},

    {"BookAntiqua", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua,Bold", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua,Italic", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua,BoldItalic", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua-Bold", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua-Italic", nullptr, "bkant.ttf", gTrue},
    {"BookAntiqua-BoldItalic", nullptr, "bkant.ttf", gTrue},

    {"Verdana", nullptr, "verdana.ttf", gTrue},
    {"Verdana,Bold", nullptr, "verdanab.ttf", gTrue},
    {"Verdana,Italic", nullptr, "verdanai.ttf", gTrue},
    {"Verdana,BoldItalic", nullptr, "verdanaz.ttf", gTrue},
    {"Verdana-Bold", nullptr, "verdanab.ttf", gTrue},
    {"Verdana-Italic", nullptr, "verdanai.ttf", gTrue},
    {"Verdana-BoldItalic", nullptr, "verdanaz.ttf", gTrue},

    {"Tahoma", nullptr, "tahoma.ttf", gTrue},
    {"Tahoma,Bold", nullptr, "tahomabd.ttf", gTrue},
    {"Tahoma,Italic", nullptr, "tahoma.ttf", gTrue},
    {"Tahoma,BoldItalic", nullptr, "tahomabd.ttf", gTrue},
    {"Tahoma-Bold", nullptr, "tahomabd.ttf", gTrue},
    {"Tahoma-Italic", nullptr, "tahoma.ttf", gTrue},
    {"Tahoma-BoldItalic", nullptr, "tahomabd.ttf", gTrue},

    {"CCRIKH+Verdana", nullptr, "verdana.ttf", gTrue},
    {"CCRIKH+Verdana,Bold", nullptr, "verdanab.ttf", gTrue},
    {"CCRIKH+Verdana,Italic", nullptr, "verdanai.ttf", gTrue},
    {"CCRIKH+Verdana,BoldItalic", nullptr, "verdanaz.ttf", gTrue},
    {"CCRIKH+Verdana-Bold", nullptr, "verdanab.ttf", gTrue},
    {"CCRIKH+Verdana-Italic", nullptr, "verdanai.ttf", gTrue},
    {"CCRIKH+Verdana-BoldItalic", nullptr, "verdanaz.ttf", gTrue},

    {"Georgia", nullptr, "georgia.ttf", gTrue},
    {"Georgia,Bold", nullptr, "georgiab.ttf", gTrue},
    {"Georgia,Italic", nullptr, "georgiai.ttf", gTrue},
    {"Georgia,BoldItalic", nullptr, "georgiaz.ttf", gTrue},
    {"Georgia-Bold", nullptr, "georgiab.ttf", gTrue},
    {"Georgia-Italic", nullptr, "georgiai.ttf", gTrue},
    {"Georgia-BoldItalic", nullptr, "georgiaz.ttf", gTrue},

    // fallback for Adobe CID fonts:
    {"MingLiU", nullptr, "mingliu.ttf", gFalse},
    {"SimSun", nullptr, "simsun.ttf", gFalse},
    {"MS-Mincho", nullptr, "msmincho.ttf", gFalse},
    {"Batang", nullptr, "batang.ttf", gFalse},
    {"ArialUnicode", nullptr, "arialuni.ttf", gTrue},
    { }
};

#define FONTS_SUBDIR "\\fonts"

static void GetWindowsFontDir(char *winFontDir, int cbWinFontDirLen)
{
    BOOL (__stdcall *SHGetSpecialFolderPathFunc)(HWND  hwndOwner,
                                                  LPSTR lpszPath,
                                                  int    nFolder,
                                                  BOOL  fCreate);
    HRESULT (__stdcall *SHGetFolderPathFunc)(HWND  hwndOwner,
                                              int    nFolder,
                                              HANDLE hToken,
                                              DWORD  dwFlags,
                                              LPSTR pszPath);

    // SHGetSpecialFolderPath isn't available in older versions of shell32.dll (Win95 and
    // WinNT4), so do a dynamic load of ANSI versions.
    winFontDir[0] = '\0';

    HMODULE hLib = LoadLibraryA("shell32.dll");
    if (hLib) {
        SHGetFolderPathFunc = (HRESULT (__stdcall *)(HWND, int, HANDLE, DWORD, LPSTR))
                              GetProcAddress(hLib, "SHGetFolderPathA");
        if (SHGetFolderPathFunc)
            (*SHGetFolderPathFunc)(nullptr, CSIDL_FONTS, nullptr, SHGFP_TYPE_CURRENT, winFontDir);

        if (!winFontDir[0]) {
            // Try an older function
            SHGetSpecialFolderPathFunc = (BOOL (__stdcall *)(HWND, LPSTR, int, BOOL))
                                          GetProcAddress(hLib, "SHGetSpecialFolderPathA");
            if (SHGetSpecialFolderPathFunc)
                (*SHGetSpecialFolderPathFunc)(nullptr, winFontDir, CSIDL_FONTS, FALSE);
        }
        FreeLibrary(hLib);
    }
    if (winFontDir[0])
        return;

    // Try older DLL
    hLib = LoadLibraryA("SHFolder.dll");
    if (hLib) {
        SHGetFolderPathFunc = (HRESULT (__stdcall *)(HWND, int, HANDLE, DWORD, LPSTR))
                              GetProcAddress(hLib, "SHGetFolderPathA");
        if (SHGetFolderPathFunc)
            (*SHGetFolderPathFunc)(nullptr, CSIDL_FONTS, nullptr, SHGFP_TYPE_CURRENT, winFontDir);
        FreeLibrary(hLib);
    }
    if (winFontDir[0])
        return;

    // Everything else failed so the standard fonts directory.
    GetWindowsDirectoryA(winFontDir, cbWinFontDirLen);
    if (winFontDir[0]) {
        strncat(winFontDir, FONTS_SUBDIR, cbWinFontDirLen);
        winFontDir[cbWinFontDirLen-1] = 0;
    }
}

static bool FileExists(const char *path)
{
    FILE * f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return true;
    }
    return false;
}

static GooString* replaceSuffix(GooString *path,
                                const char* suffixA, const char* suffixB)
{
  int suffLenA = strlen(suffixA);
  int suffLenB = strlen(suffixB);
  int baseLenA = path->getLength() - suffLenA;
  int baseLenB = path->getLength() - suffLenB;

  if (!strcasecmp(path->getCString()+baseLenA, suffixA)) {
    path->del(baseLenA,suffLenA)->append(suffixB);
  } else if (!strcasecmp(path->getCString()+baseLenB, suffixB)) {
    path->del(baseLenB,suffLenB)->append(suffixA);
  }

  return path;
}

static const char *findSubstituteName(GfxFont *font, const std::unordered_map<std::string, std::string>& fontFiles,
                                      const std::unordered_map<std::string, std::string>& substFiles,
                                      const char *origName)
{
    assert(origName);
    if (!origName) return nullptr;
    GooString *name2 = new GooString(origName);
    int n = strlen(origName);
    // remove trailing "-Identity-H"
    if (n > 11 && !strcmp(name2->getCString() + n - 11, "-Identity-H")) {
      name2->del(n - 11, 11);
      n -= 11;
    }
    // remove trailing "-Identity-V"
    if (n > 11 && !strcmp(name2->getCString() + n - 11, "-Identity-V")) {
      name2->del(n - 11, 11);
      n -= 11;
    }
    const auto substFile = substFiles.find(name2->getCString());
    if (substFile != substFiles.end()) {
      delete name2;
      return substFile->second.c_str();
    }

    /* TODO: try to at least guess bold/italic/bolditalic from the name */
    delete name2;
    if (font->isCIDFont()) {
      const GooString *collection = ((GfxCIDFont *)font)->getCollection();

      const char* name3 = nullptr;
      if ( !collection->cmp("Adobe-CNS1") )
        name3 = DEFAULT_CID_FONT_AC1_MSWIN;
      else if ( !collection->cmp("Adobe-GB1") )
        name3 = DEFAULT_CID_FONT_AG1_MSWIN;
      else if ( !collection->cmp("Adobe-Japan1") )
        name3 = DEFAULT_CID_FONT_AJ1_MSWIN;
      else if ( !collection->cmp("Adobe-Japan2") )
        name3 = DEFAULT_CID_FONT_AJ2_MSWIN;
      else if ( !collection->cmp("Adobe-Korea1") )
        name3 = DEFAULT_CID_FONT_AK1_MSWIN;

      if (name3 && fontFiles.count(name3) != 0)
        return name3;

      if (fontFiles.count(DEFAULT_CID_FONT_MSWIN) != 0)
        return DEFAULT_CID_FONT_MSWIN;
    } 
    return DEFAULT_SUBSTITUTE_FONT;
}
#endif
