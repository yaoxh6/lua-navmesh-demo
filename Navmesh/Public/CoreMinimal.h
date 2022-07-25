#pragma once
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cfloat>
#include <cstdarg>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <new>

#define NAVMESH_API
#define CA_ASSUME(...) 
#define CA_SUPPRESS(...)
#define check(x) assert(x)
#define UE_LOG(...)
#define UE_CLOG(...)
#define int32 int32_t
#define DEFINE_LOG_CATEGORY_STATIC(...)
#define DECLARE_LOG_CATEGORY_EXTERN(...)
#define DEFINE_LOG_CATEGORY(...)
#define TEXT(x) x
#define TCHAR char
#define KINDA_SMALL_NUMBER (1.e-4f)

struct FCStringAnsi {
	static int32 GetVarArgs(char* Dest, size_t DestSize, const char* Fmt, va_list ArgPtr) { return vsnprintf(Dest, DestSize, Fmt, ArgPtr); }
};

#define WITH_NAVMESH_SEGMENT_LINKS 1
#define WITH_NAVMESH_CLUSTER_LINKS 1