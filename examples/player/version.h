#ifndef VERSION_H
#define VERSION_H

#define XUNO_QTAV_MAJOR 0    //((XUNO_QTAV_VERSION&0xff0000)>>16)
#define XUNO_QTAV_MINOR 2    //((XUNO_QTAV_VERSION&0xff00)>>8)
#define XUNO_QTAV_PATCH 3    //(XUNO_QTAV_VERSION&0xff)

#ifdef WIN64
#define QTAV_PLATFORM x64
#endif

#ifdef QTAV_PLATFORM
#define XUNO_QTAV_VERSION_STR       "v" TOSTR(XUNO_QTAV_MAJOR) "." TOSTR(XUNO_QTAV_MINOR) "." TOSTR(XUNO_QTAV_PATCH) " ("TOSTR(QTAV_PLATFORM)")"
#else
#define XUNO_QTAV_VERSION_STR       "v." TOSTR(XUNO_QTAV_MAJOR) "." TOSTR(XUNO_QTAV_MINOR) TOSTR(XUNO_QTAV_PATCH)
#endif

#define XUNO_QTAV_VERSION_STR_LONG   XUNO_QTAV_VERSION_STR " (" __DATE__ ", " __TIME__ ")"

#endif // VERSION_H

