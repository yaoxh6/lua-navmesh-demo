#pragma once

#ifdef __cplusplus
extern "C" {
#endif

enum LUA_LOG_LEVEL {
    LUA_LOG_DEBUG_LEVEL,
    LUA_LOG_INFO_LEVEL,
    LUA_LOG_WARN_LEVEL,
    LUA_LOG_ERR_LEVEL,
};

#ifdef __GNUC__
#define LUA_LOG_FUNCTION __PRETTY_FUNCTION__
#else
#define LUA_LOG_FUNCTION __FUNCTION__
#endif

#define LUA_LOG_DEBUG(fmt, ...)                                                                                                                                                                                                                                                                \
    do {                                                                                                                                                                                                                                                                                       \
        lua_log(LUA_LOG_DEBUG_LEVEL, "%s@%d: " fmt, LUA_LOG_FUNCTION, __LINE__, ##__VA_ARGS__);                                                                                                                                                                                                            \
    } while (0)

#define LUA_LOG_INFO(fmt, ...)                                                                                                                                                                                                                                                                 \
    do {                                                                                                                                                                                                                                                                                       \
        lua_log(LUA_LOG_INFO_LEVEL, "%s@%d: " fmt, LUA_LOG_FUNCTION, __LINE__, ##__VA_ARGS__);                                                                                                                                                                                                             \
    } while (0)

#define LUA_LOG_WARN(fmt, ...)                                                                                                                                                                                                                                                                 \
    do {                                                                                                                                                                                                                                                                                       \
        lua_log(LUA_LOG_WARN_LEVEL, "%s@%d: " fmt, LUA_LOG_FUNCTION, __LINE__, ##__VA_ARGS__);                                                                                                                                                                                                             \
    } while (0)

#define LUA_LOG_ERR(fmt, ...)                                                                                                                                                                                                                                                                  \
    do {                                                                                                                                                                                                                                                                                       \
        lua_log(LUA_LOG_ERR_LEVEL, "%s@%d: " fmt, LUA_LOG_FUNCTION, __LINE__, ##__VA_ARGS__);                                                                                                                                                                                                              \
    } while (0)

void lua_log(int level, const char* fmt, ...);

#ifdef __cplusplus
}
#endif