#include "stdafx.h"
#include "common.h"
#include "LuaBase.h"
#include "Types.h"
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>


int loadFile(const char * path, const char * fakePath = "@lua\\autorun\\properties.lua")
{
	std::cout << path << std::endl;
	std::string line, text;
	std::ifstream in(path);
	while (std::getline(in, line))
	{
		text += line + "\n";
	}
	const char* data = text.c_str();

	luaClientVMTHook->GetOriginalFunction<RunStringExHookFn>(RUNSTRINGEX)(_state, fakePath, "", data, true, true, 1, 1);
	return 1;
}

int RunClientFile()
{
	if (ClientLua == NULL)
		FindClientLua();

	ClientLua->CheckType(1, GarrysMod::Lua::Type::STRING);
	const char * path = (ClientLua->GetString(1));
	path = ("E://member/" + (std::string)path).c_str();

	return 1;
}

int LoadCheat()
{
	loadFile("E:\\member\\autoload\\____init.lua");//, "E://member/autoload/____init.lua");
	loadFile("E://member/autoload/___detours.lua");//, "E://member/autoload/___detours.lua");
	loadFile("E://member/autoload/___socket.lua");//, "E://member/autoload/___socket.lua");

	std::string path = "E://member/autoload/";
	for (const auto & p : std::filesystem::directory_iterator(path)) {
		if (!p.path().u8string().rfind("E://member/autoload/_", 0) == 0)
			loadFile(p.path().u8string().c_str());//, p.path().u8string().c_str());
	}
	return 1;
}


int InitLuaFunctions()
{
	ClientLua->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push the global table
	ClientLua->PushCFunction((CFunc)RunClientFile); // Push our function
	ClientLua->SetField(-2, "RunClientFile"); // Set MyFirstFunction in lua to our C++ function
	ClientLua->Pop(); // Pop the global table off the stack

	ClientLua->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // Push the global table
	ClientLua->PushCFunction((CFunc)LoadCheat); // Push our function
	ClientLua->SetField(-2, "LoadCheat"); // Set MyFirstFunction in lua to our C++ function
	ClientLua->Pop(); // Pop the global table off the stack

	return 1;
}