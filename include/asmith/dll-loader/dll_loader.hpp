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

#ifndef ASMITH_DLL_DLL_LOADER
#define ASMITH_DLL_DLL_LOADER

namespace asmith {

	class dll_loader {
	private:
		void* mDll;
	public:
		dll_loader() throw();
		dll_loader(const char*) throw();
		~dll_loader() throw();

		void load_dll(const char*) throw();

		void* get_raw_function(const char*) throw();
	};

}

#endif