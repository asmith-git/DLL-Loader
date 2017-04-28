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

#include "asmith/dll-loader/dll.hpp"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>
#include <string>
#include <mutex>
#include <algorithm>

//! \todo Implmented dynamic libraries for UNIX systems

namespace asmith {

	struct dll_data_t {
		HMODULE handle;
	};
	
	static std::map<std::string, std::weak_ptr<dll>> DLL_MAP;
	static std::mutex DLL_MAP_LOCK;

	// dll
	std::shared_ptr<dll> dll::load(const char* aPath) throw() {
		std::shared_ptr<dll> r;
		const std::string path = aPath;
		
		DLL_MAP_LOCK.lock();
			const auto i = DLL_MAP.find(path);
			if(i != DLL_MAP.end()) {
				r = i->second.lock();
				if(! r) DLL_MAP.erase(i);
			}else {
				dll_data_t tmp;
				tmp.handle = LoadLibraryA(aPath);
				if(tmp.handle != NULL) {
					r = std::shared_ptr<dll>(new dll(path, new dll_data_t(tmp)));
					DLL_MAP.emplace(path, r);
				}
			}
		DLL_MAP_LOCK.unlock();
		return r;
	}

	dll::dll(const std::string& aPath, dll_data_t* aData) :
		mPath(aPath),
		mData(aData)
	{}

	dll::~dll() {
		if(mData) {
			FreeLibrary(mData->handle);
			delete mData;
		}
	}

	const char* dll::get_path() const throw() {
		return mPath.c_str();
	}
	
	void* dll::get_raw_function(const char* aName) throw() {
		return mData ? GetProcAddress(mData->handle, aName) : nullptr;
	}
}
