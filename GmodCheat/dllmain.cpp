#include "stdafx.h"	
#include <windows.h>
#include <detours.h>
#include <iostream>
#include "util.h"
#include "vmthook.h"
#include "LuaBase.h"
#include "common.h"
#include <string>
#include <fstream>


#pragma warning (disable : 4996)

extern "C" {
	# include "lua.h"
	# include "lauxlib.h"
	# include "lualib.h"
}


ILuaBase *MENU;
_luaL_loadbufferx oluaL_loadbufferx;
_luaL_loadbuffer oluaL_loadbuffer;
_luaL_loadfilex oluaL_loadfilex;
_lua_pcall olua_pcallfn;
VMTHook* luaSharedVMTHook;
VMTHook* luaClientVMTHook;
ILuaInterface* ClientLua;

lua_State *_state = 0;

std::string ip = "";

int file_count = 0;



int lua_pcall_hook(lua_State *L, int nargs, int nresults, int errfunc)
{
	if (_state == 0) {
		std::cout << "lua_State location: 0x" << &L << std::endl;
		_state = L;
	}
	
	return olua_pcallfn(L, nargs, nresults, errfunc);;
}





void * __fastcall RunStringExHook(void *thisptr, int edx, const char *fileName, const char *path, const char *stringToRun, bool run, bool showErrors, bool a, bool b)
{
	std::cout << fileName << std::endl;
	std::string testFile = fileName;	
	save_server_file(fileName, stringToRun, _state);
	if (file_count == 70) {
		luaClientVMTHook->GetOriginalFunction<RunStringExHookFn>(RUNSTRINGEX)(thisptr, fileName, path, stringToRun, run, showErrors, a, b);
		LoadCheat();
		InitLuaFunctions();
		file_count++;
	}
	else {
		file_count++;
		return luaClientVMTHook->GetOriginalFunction<RunStringExHookFn>(RUNSTRINGEX)(thisptr, fileName, path, stringToRun, run, showErrors, a, b);
	}	
}


lua_State * __fastcall CreateLuaInterfaceHook(void *thisptr, int edx, unsigned char stateType, bool renew)
{
	lua_State *state = luaSharedVMTHook->GetOriginalFunction<CreateLuaInterfaceHookFn>(CREATELUAINTERFACE)(thisptr, stateType, renew);
	reset_server_ip();

	if (stateType != 0)
		return state;

	_state = state;
	file_count = 0;

	luaClientVMTHook = new VMTHook(state);
	luaClientVMTHook->HookFunction(&RunStringExHook, RUNSTRINGEX);

	return _state;
}


void * __fastcall CloseLuaInterfaceHook(void *thisptr, int edx, lua_State *state)
{
	file_count = 0;
	reset_server_ip();
	ClientLua = NULL;
	if (state == _state)
		_state = NULL;

	return luaSharedVMTHook->GetOriginalFunction<CloseLuaInterfaceHookFn>(CLOSELUAINTERFACE)(thisptr, state);
}




int main()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	HMODULE hmod = GetModuleHandle("lua_shared.dll");
	oluaL_loadbufferx = (_luaL_loadbufferx)GetProcAddress(hmod, "luaL_loadbufferx");
	oluaL_loadbuffer = (_luaL_loadbuffer)GetProcAddress(hmod, "luaL_loadbuffer");
	oluaL_loadfilex = (_luaL_loadfilex)GetProcAddress(hmod, "luaL_loadfilex");
	olua_pcallfn = (_lua_pcall)GetProcAddress(hmod, "lua_pcall");

	auto luaShared = util::GetInterfaceSingle<void *>("lua_shared.dll", "LUASHARED003");

	luaSharedVMTHook = new VMTHook(luaShared);
	luaSharedVMTHook->HookFunction(&CreateLuaInterfaceHook, CREATELUAINTERFACE);
	luaSharedVMTHook->HookFunction(&CloseLuaInterfaceHook, CLOSELUAINTERFACE);

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)olua_pcallfn, lua_pcall_hook);
	DetourTransactionCommit();

	
	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)main, hModule, 0, nullptr);
		main();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}