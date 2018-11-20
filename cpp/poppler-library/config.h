/* config.h.  Generated from config.h.cmake by cmake.  */

/* Build against libcurl. */
//#define ENABLE_LIBCURL 0

/* Use libjpeg instead of builtin jpeg decoder. */
//#define ENABLE_LIBJPEG 0

/* Use libopenjpeg instead of builtin jpeg2000 decoder. */
//#define ENABLE_LIBOPENJPEG 0

/* Build against libtiff. */
//#define ENABLE_LIBTIFF 1

/* Build against libpng. */
//#define ENABLE_LIBPNG 1

/* Do not hardcode the library location */
#define ENABLE_RELOCATABLE 1

/* Build against zlib. */
//#define ENABLE_ZLIB 0

/* Use zlib instead of builtin zlib decoder to uncompress flate streams. */
//#define ENABLE_ZLIB_UNCOMPRESS 0

/* Build against libnss3 for digital signature validation */
//#define ENABLE_NSS3 0

/* Use cairo for rendering. */
#define HAVE_CAIRO 0

/* Do we have any DCT decoder?. */
//#define HAVE_DCT_DECODER 0

/* Do we have any JPX decoder?. */
//#define HAVE_JPX_DECODER 0

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#define HAVE_DIRENT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fseek64' function. */
#define HAVE_FSEEK64 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the `ftell64' function. */
#define HAVE_FTELL64 1

/* Define to 1 if you have the `pread64' function. */
#define HAVE_PREAD64 1

/* Define to 1 if you have the `lseek64' function. */
#define HAVE_LSEEK64 1

/* Defines if gettimeofday is available on your system */
//#define HAVE_GETTIMEOFDAY 1

/* Defines if gmtime_r is available on your system */
#define HAVE_GMTIME_R 1

/* Defines if timegm is available on your system */
#define HAVE_TIMEGM 1

/* Define if you have the iconv() function and it works. */
#define HAVE_ICONV 1

/* Define to 1 if you have the `z' library (-lz). */
#define HAVE_LIBZ 1

/* Defines if localtime_r is available on your system */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if you have the `mkstemp' function. */
#define HAVE_MKSTEMP 1

/* Define to 1 if you have the `strcpy_s' function. */
#define HAVE_STRCPY_S 1

/* Define to 1 if you have the `strcat_s' function. */
#define HAVE_STRCAT_S 1

/* Defines if strtok_r is available on your system */
#define HAVE_STRTOK_R 1

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#define HAVE_NDIR_H 1

/* Define to 1 if you have the `popen' function. */
#define HAVE_POPEN 1

/* Use splash for rendering. */
//#define HAVE_SPLASH 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#define HAVE_SYS_DIR_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#define HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#define HAVE_SYS_NDIR_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <unistd.h> header file. */
//#define HAVE_UNISTD_H 1

/* Define to 1 if you have the <codecvt> header file. */
#define HAVE_CODECVT

/* Define to 1 if you have a big endian machine */
#define WORDS_BIGENDIAN 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST ${ICONV_CONST}

/* Generate OPI comments in PS output. */
#define OPI_SUPPORT 1

/* Name of package */
#define PACKAGE "poppler"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "https://bugs.freedesktop.org/enter_bug.cgi?product=poppler"

/* Define to the full name of this package. */
#define PACKAGE_NAME "poppler"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "poppler ${POPPLER_VERSION}"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "poppler"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "${POPPLER_VERSION}"

/* Poppler data dir */
#define POPPLER_DATADIR "${POPPLER_DATADIR}"

/* Support for curl based doc builder is compiled in. */
#define POPPLER_HAS_CURL_SUPPORT 1

/* Include support for CMYK rasterization */
#define SPLASH_CMYK 1

/* Enable word list support. */
#define TEXTOUT_WORD_LIST 1

/* Defines if use cms */
//#define USE_CMS 1

/* Use fixed point arithmetic in the Splash backend */
#define USE_FIXEDPOINT 1

/* Use single precision arithmetic in the Splash backend */
#define USE_FLOAT 1

/* Version number of package */
#define VERSION "${POPPLER_VERSION}"

/* Use fontconfig font configuration backend */
//#define WITH_FONTCONFIGURATION_FONTCONFIG 1

/* Use win32 font configuration backend */
//#define WITH_FONTCONFIGURATION_WIN32 1

/* OpenJPEG with the OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG flag */
#define WITH_OPENJPEG_IGNORE_PCLR_CMAP_CDEF_FLAG 1

/* MS defined snprintf as deprecated but then added it in Visual Studio 2015. */
#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#endif

//------------------------------------------------------------------------
// popen
//------------------------------------------------------------------------
#if defined(_MSC_VER) || defined(__BORLANDC__)
#define popen _popen
#define pclose _pclose
#endif

//------------------------------------------------------------------------
// strdup
//------------------------------------------------------------------------
#ifdef _MSC_VER
#define strdup _strdup
#endif

//------------------------------------------------------------------------
// stricmp
//------------------------------------------------------------------------
#ifdef _MSC_VER
#define stricmp _stricmp
#endif

//------------------------------------------------------------------------
// strcasecmp
//------------------------------------------------------------------------
#ifdef _MSC_VER
#  define strcasecmp _stricmp
#else
#  include <strings.h>
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
//#define _FILE_OFFSET_BITS @_FILE_OFFSET_BITS@

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* TODO This is wrong, port if needed #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* TODO This is wrong, port if needed #undef _LARGE_FILES */
