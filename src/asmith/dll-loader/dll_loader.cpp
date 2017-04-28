//	Copyright 2017 Adam Smith
//	Licensed under the Apache License, Version 2.0 (the "License");
//	you may not use this file except in compliance with the License.
//	You may obtain a copy of the License at
// 
//	http://www.apache.org/licenses/LICENSE-2.0
//
//	Unless required by applicable law or agreed to in writing, software
//	distributed under the License is distributed on an "AS IS" BASIS,
//	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//	See the License for the specific language governing permissions and
//	limitations under the License.

#include "asmith/dll-loader/dll_loader.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>
#include <string>
#include <mutex>
#include <algorithm>

//! \todo Implmented dynamic libraries for UNIX systems

namespace asmith {

	struct dll {
		HMODULE handle;
		size_t users;
	};

	static std::map<std::string, dll> DLL_MAP;
	static std::mutex DLL_MAP_LOCK;

	static dll* load_dll(const char* aPath) throw() {
		dll* r = nullptr;
		const std::string path = aPath;

		DLL_MAP_LOCK.lock();
			const auto i = DLL_MAP.find(path);
			if(i != DLL_MAP.end()) {
				r = &i->second;
			}else {
				dll tmp;
				tmp.handle = LoadLibraryA(aPath);
				tmp.users = 0;
				if(tmp.handle != NULL) {
					DLL_MAP.emplace(path, tmp);
				}
			}
		DLL_MAP_LOCK.unlock();
		return r;
	}



	static bool unload_dll(const dll& aDll) throw() {
		bool r = false;

		DLL_MAP_LOCK.lock();
			const auto end = DLL_MAP.end();
			const auto i = std::find_if(DLL_MAP.begin(), end, [&aDll](const std::pair<const std::string,dll>& a)->bool {
				return a.second.handle == aDll.handle;
			});

			if(i == end){
				r = true;
				if(i->second.users == 0) {
					FreeLibrary(i->second.handle);
					DLL_MAP.erase(i);
				}
			}
		DLL_MAP_LOCK.unlock();
		return r;
	}

	// dll_loader

	dll_loader::dll_loader() throw() :
		mDll(nullptr)
	{}

	dll_loader::dll_loader(const char* aPath) throw() :
		mDll(nullptr) 
	{
		load_dll(aPath);
	}

	dll_loader::~dll_loader() throw() {
		if(mDll) {
			--mDll->users;
			asmith::unload_dll(*mDll);
			mDll = nullptr;
		}
	}

	bool dll_loader::load_dll(const char* aPath) throw() {
		if(mDll) {
			--mDll->users;
			asmith::unload_dll(*mDll);
		}
		mDll = asmith::load_dll(aPath);
		if(! mDll) return false;
		++mDll->users;
		return true;
	}

	void* dll_loader::get_raw_function(const char* aName) throw() {
		return mDll ? GetProcAddress(mDll->handle, aName) : nullptr;
	}

}