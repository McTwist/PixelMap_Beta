

#include "Computer.hpp"

#ifdef WIN32
#include <windows.h>
#elif defined(UNIX)
#include <unistd.h>
// Compatibility with other systems
#ifdef _SC_PAGESIZE
#define SIZE_OF_PAGE _SC_PAGESIZE
#else
#define SIZE_OF_PAGE _SC_PAGE_SIZE
#endif
#endif

unsigned long Computer::GetAvailableMemory()
{
#ifdef WIN32
	MEMORYSTATUS info;
	GlobalMemoryStatus(&info);
	return info.dwAvailPhys;
#elif defined(UNIX)
	return sysconf(_SC_AVPHYS_PAGES) * sysconf(SIZE_OF_PAGE);
#endif
}

unsigned long Computer::GetTotalMemory()
{
#ifdef WIN32
	MEMORYSTATUS info;
	GlobalMemoryStatus(&info);
	return info.dwTotalPhys;
#elif defined(UNIX)
	return sysconf(_SC_PHYS_PAGES) * sysconf(SIZE_OF_PAGE);
#endif
}

unsigned long Computer::GetLogicalCores()
{
#ifdef WIN32
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return info.dwNumberOfProcessors;
#elif defined(UNIX)
	return sysconf(_SC_NPROCESSORS_ONLN);
#endif
}