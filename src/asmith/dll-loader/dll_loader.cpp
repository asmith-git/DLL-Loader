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

//! \todo Implmented dynamic libraries for UNIX systems

namespace asmith {

	struct dll {
		HMODULE handle;
	};

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
			FreeLibrary(mDll->handle);
			delete mDll;
		}
	}

	bool dll_loader::load_dll(const char* aPath) throw() {
		HMODULE tmp = LoadLibraryA(aPath);
		if(tmp == NULL) return false;

		if(mDll) delete static_cast<dll*>(mDll);
		mDll = new dll();
		mDll->handle = tmp;

		return true;
	}

	void* dll_loader::get_raw_function(const char* aName) throw() {
		return GetProcAddress(mDll->handle, aName);
	}

}