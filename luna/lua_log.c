#include "lua_log.h"
#include "lua.h"
#include <stdio.h>
#include <stdlib.h>

static lua_State* sL = NULL;
static char log_buffer[64 * 1024];

void cache_lua_state(lua_State* L)
{
    if (sL && L && sL != L) {
        fprintf(stderr, "sL is not NULL and is not the same as L");
        abort();
    }
    sL = L;
}

static const char* log_level_str(int level)
{
    switch (level) {
    case LUA_LOG_DEBUG_LEVEL:
        return "log_debug";

    case LUA_LOG_INFO_LEVEL:
        return "log_info";

    case LUA_LOG_WARN_LEVEL:
        return "log_warn";

    case LUA_LOG_ERR_LEVEL:
        return "log_err";

    default:
        return NULL;
    }
}

static void lua_log_print(int level, const char* txt)
{
    do {
        if (!sL)
            break;

        const char* level_str = log_level_str(level);
        if (!level_str)
            break;

        int top = lua_gettop(sL);

        int fn = lua_getglobal(sL, level_str);
        if (fn != LUA_TFUNCTION) {
            lua_settop(sL, top);
            break;
        }

        lua_pushstring(sL, txt);
        int status = lua_pcall(sL, 1, 0, 0);
        if (status != LUA_OK) {
            lua_settop(sL, top);
            break;
        }

        lua_settop(sL, top);
        return;
    } while (0);

    const char* level_str = log_level_str(level);
    level_str = level_str ? level_str : "unknown";
    printf("%s: %s\n", level_str, txt);
}

void lua_log(int level, const char* fmt, ...)
{
    va_list marker;
    va_start(marker, fmt);
    int length = vsnprintf(log_buffer, sizeof(log_buffer), fmt, marker);
    va_end(marker);

    if (length < 0)
        return;

    log_buffer[sizeof(log_buffer) - 1] = '\0';
    lua_log_print(level, log_buffer);
}
