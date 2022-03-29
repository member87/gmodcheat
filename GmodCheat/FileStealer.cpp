#include "stdafx.h"
#include "common.h"
#include <windows.h>
#include "GLua.h"
#include "LuaBase.h"
#include <algorithm>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include <iterator>
#include <filesystem>


const char * ip = "";
std::string DIR = "E://member/steal/";


int FindClientLua()
{
	HMODULE LuaShared_modhandle = GetModuleHandle(TEXT("lua_shared.dll"));
	if (LuaShared_modhandle != NULL)
	{
		CreateInterfaceFn LuaShared_createinter = (CreateInterfaceFn)GetProcAddress(LuaShared_modhandle, "CreateInterface");
		if (LuaShared_createinter != NULL)
		{
			CLuaShared* LuaShared = (CLuaShared*)LuaShared_createinter("LUASHARED003", NULL);
			if (LuaShared != NULL)
			{
				ClientLua = LuaShared->GetLuaInterface(LUA_CLIENT);
				if (ClientLua != NULL)
				{
					// lua_State* state = _state;

					// MENU = LUA;

					return 1;
				}
			}
		}
	}
	return 0;
}


void get_server_ip(lua_State* _state)
{
	if (ClientLua == NULL)
		FindClientLua();

	ClientLua->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	ClientLua->GetField(-1, "game");
	ClientLua->GetField(-1, "GetIPAddress");
	ClientLua->PushNil();
	ClientLua->Call(1, 1);
	ip = ClientLua->GetString(-1);
	ClientLua->Pop(3);
}



// CHECK IF FILE EXISTS
inline bool file_exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

void eraseSubStr(std::string & mainStr, const std::string & toErase)
{
	size_t pos = mainStr.find(toErase);

	if (pos != std::string::npos)
	{
		mainStr.erase(pos, toErase.length());
	}
}


void save_server_file(const char *fileName, const char *stringToRun, lua_State* state)
{
	std::string FILE_PATH = fileName;
	eraseSubStr(FILE_PATH, "../");
	eraseSubStr(FILE_PATH, "..\\");


	get_server_ip(state);
	
	std::string new_ip = ip;
	std::replace(new_ip.begin(), new_ip.end(), ':', '_');

	

	// MAKE SURE SERVER FOLDER EXISTS
	std::string target = DIR + new_ip + "/" + FILE_PATH;


	// MAKE SURE ALL FILES HAVE .LUA ON THE END
	std::string file_type = ".lua";
	if (!std::equal(file_type.rbegin(), file_type.rend(), target.rbegin()))
		target += file_type;


	// MAKE SURE THE DIR THE FILE IS IN EXISTS
	namespace fs = std::filesystem;
	std::size_t found = target.find_last_of("/\\");
	fs::create_directories(target.substr(0, found));

	std::string FILE_NAME = target.substr(found + 1);


	if (FILE_NAME == "LuaCmd.lua" && file_exists(target))
	{
		std::ofstream outfile;
		outfile.open(target, std::ios_base::app); // append instead of overwrite
		outfile << stringToRun << std::endl;
		outfile.close();
	}
	else
	{
		std::ofstream myfile;
		myfile.open(target);
		myfile << stringToRun;
		myfile.close();
	}
}

void reset_server_ip()
{
	ip = "";
	return;
}