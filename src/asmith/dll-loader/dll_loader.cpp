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

namespace asmith {

	struct dll {

	};

	// dll_loader

	dll_loader::dll_loader() throw() :
		mDll(nullptr)
	{}

	dll_loader::dll_loader(const char* aPath) throw() :
		mDll(nullptr) 
	{}

	dll_loader::~dll_loader() throw() {

	}

	bool dll_loader::load_dll(const char* aPath) throw() {

	}

	void* dll_loader::get_raw_function(const char* aName) throw() {
		return nullptr;
	}

}