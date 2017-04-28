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

#ifndef ASMITH_DLL_DLL_LOADER_HPP
#define ASMITH_DLL_DLL_LOADER_HPP

namespace asmith {

	template<class R, class... PARAMS>
	using dll_function = R(__stdcall *)(PARAMS...);

	struct dll;

	class dll_loader {
	private:
		dll* mDll;

		dll_loader(dll_loader&&) = delete;
		dll_loader(const dll_loader&) = delete;
		dll_loader& operator=(dll_loader&&) = delete;
		dll_loader& operator=(const dll_loader&) = delete;
	public:
		dll_loader() throw();
		dll_loader(const char*) throw();
		~dll_loader() throw();

		bool load_dll(const char*) throw();

		void* get_raw_function(const char*) throw();

		template<class R, class... PARAMS>
		inline dll_function<R, PARAMS...> get_function(const char* aPath) throw() { 
			return static_cast<dll_function<R, PARAMS...>>(get_raw_function(aPath)); 
		}

		template<class T>
		inline T get_variable(const char* aPath) {
			T* const tmp = static_cast<T*>(get_raw_function(aPath));
			if(tmp == nullptr) throw std::runtime_error("asmith::dll_loader::get_variable : Variable was not found with the given name");
			return *tmp;
		}
	};

}

#endif