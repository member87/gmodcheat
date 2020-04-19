#pragma once
#include "GLua.h"
#include "vmthook.h"

#define CREATELUAINTERFACE 4
#define CLOSELUAINTERFACE 5
#define RUNSTRINGEX 111





void save_server_file(const char *, const char *, lua_State*);
void reset_server_ip(void);
int FindClientLua(void);
int InitLuaFunctions(void);
int LoadCheat(void);
typedef void *(__thiscall *RunStringExHookFn)(void *thisptr, const char *fileName, const char *path, const char *x, bool run, bool showErrors, bool, bool);
typedef void*(__cdecl *_luaL_loadbufferx)(void* state, const char* buff, size_t sz, const char* name, int shit);
typedef void*(__cdecl *_luaL_loadbuffer)(lua_State *L, const char *buff, size_t size, const char *name);
typedef int(*_lua_pcall)(lua_State *L, int nargs, int nresults, int errfunc);
typedef int(*_luaL_loadfilex)(lua_State *L, const char *filename, const char *mode);
typedef lua_State *(__thiscall *CreateLuaInterfaceHookFn)(void *thisptr, unsigned char stateType, bool renew);
typedef void *(__thiscall *CloseLuaInterfaceHookFn)(void *thisptr, void *state);
typedef void* (*CreateInterfaceFn)(const char *name, int *returncode);

extern _luaL_loadbufferx oluaL_loadbufferx;
extern _luaL_loadbuffer oluaL_loadbuffer;
extern _luaL_loadfilex oluaL_loadfilex;
extern _lua_pcall olua_pcallfn;
extern VMTHook* luaSharedVMTHook;
extern VMTHook* luaClientVMTHook;
extern ILuaInterface* ClientLua;
extern lua_State *_state;