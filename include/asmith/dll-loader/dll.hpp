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

#ifndef ASMITH_DLL_DLL_HPP
#define ASMITH_DLL_DLL_HPP

#include <string>
#include <memory>

#ifndef ASMITH_DLL_CALLING_CONVENTION
	#define ASMITH_DLL_CALLING_CONVENTION
#endif

namespace asmith {

	/*!
		\brief Helps define the types of functions returned by dynamic libraries
		\tparam R
		\tparam PARAMS
		\see dll
	*/
	template<class R, class... PARAMS>
	using dll_function = R(ASMITH_DLL_CALLING_CONVENTION *)(PARAMS...);

	struct dll_data_t;

	class dll {
	private:
		const std::string mPath;	//! The file path of the library
		dll_data_t* const mData;	//! OS specific handles for the libary

		dll(dll&&) = delete;
		dll(const dll&) = delete;
		dll& operator=(dll&&) = delete;
		dll& operator=(const dll&) = delete;

		dll(const std::string&, dll_data_t*) throw();
	public:
		static std::shared_ptr<dll> load(const char*) throw();

		~dll() throw();

		const char* get_path() const throw();
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
