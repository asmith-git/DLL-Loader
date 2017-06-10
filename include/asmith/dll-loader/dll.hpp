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

#include <memory>

#ifndef ASMITH_DLL_CALLING_CONVENTION
	#define ASMITH_DLL_CALLING_CONVENTION
#endif

namespace asmith {

	/*!
		\brief Helps define the types of functions returned by dynamic libraries
		\tparam R The return type of the function
		\tparam PARAMS The parameter types of the function, empty when there are no parameters
		\see dll
	*/
	template<class R, class... PARAMS>
	using dynamic_function = R(ASMITH_DLL_CALLING_CONVENTION *)(PARAMS...);

	struct dll_data_t;


	/*!
		\author Adam Smith
		\brief A simple class for loading dynamic link libraries (DLL), and extracting function pointers or variables from them.
		Usage Example:
		\code
		std::shared_ptr<dll> myDLL = dynamic_library::load_library("example.dll");

		// Loading a function
		float(*myFunction)(int, int) = mDLL->load_function<float,int,int>("myFunction");

		// Loading a variable
		float myVar = mDLL->get_variable<float>("myFunction");
		\endcode
		\date Created: 28th April 2017 Modified : 10th June 2017
		\version 2.0
	*/
	class dynamic_library {
	private:
		dynamic_library(dynamic_library&&) = delete;
		dynamic_library(const dynamic_library&) = delete;
		dynamic_library& operator=(dynamic_library&&) = delete;
		dynamic_library& operator=(const dynamic_library&) = delete;
	protected:
		dynamic_library() {}
	public:
		static std::shared_ptr<dynamic_library> load_library(const char*) throw();

		/*!
			\brief Destroy the library
		*/
		virtual ~dynamic_library() {}

		/*!
			\brief Get the file path the library was loaded from
			\return The file path
		*/
		virtual const char* get_path() const throw() = 0;

		/*!
			\brief Load an untyped symbol from the library
			\param The name of the symbol
			\return The address of the symbol, or nullptr if the load failed
		*/
		virtual void* load_symbol(const char*) = 0;

		/*!
			\tparam R The return type of the function
			\tparam PARAMS The parameter types of the function, empty when there are no parameters
			\brief Load a function with a known type
			\param The name of the function
			\return The address of the function, or nullptr if the load failed
			\see get_raw_function
			\see dll_function
		*/
		template<class R, class... PARAMS>
		inline dynamic_function<R, PARAMS...> load_function(const char* aPath) throw() {
			return static_cast<dynamic_function<R, PARAMS...>>(load_symbol(aPath));
		}


		/*!
			\tparam T The type of a variable
			\brief Load a variable with a known type, an exception is thrown on load failure
			\param The name of the variable
			\return The variable
			\see get_raw_function
		*/
		template<class T>
		inline T load_variable(const char* aPath) {
			T* const tmp = static_cast<T*>(load_symbol(aPath));
			if(tmp == nullptr) throw std::runtime_error("asmith::dll_loader::load_variable : Variable was not found with the given name");
			return *tmp;
		}
	};
}

#endif
