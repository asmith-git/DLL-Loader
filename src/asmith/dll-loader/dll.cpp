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

	/*!
		\class dll
		\author Adam Smith
		\brief A simple class for loading dynamic link libraries (DLL), and extracting function pointers or variables from them.
		\detail Currently only implemented for Windows OS
		Usage Example:
		\code
		std::shared_ptr<dll> myDLL = dll::load("example.dll");

		// Loading a function
		float(*myFunction)(int, int) = mDLL->get_functon<float,int,int>("myFunction");

		// Loading a variable
		float myVar = mDLL->get_variable<float>("myFunction");
		\endcode
		\date Created: 28th April 2017 Modified : 28th April 2017
	*/

	struct dll_data_t {
		HMODULE handle;
	}; //!< \brief Contains OS specific handles for dynamic libraries, for internal use only.
	
	static std::map<std::string, std::weak_ptr<dll>> DLL_MAP;
	static std::mutex DLL_MAP_LOCK;

	// dll

	/*!
		\brief Load a dynamic link library from file
		\param aPath The file path of the library
		\return The libary object, null if the library failed to load.
	*/
	std::shared_ptr<dll> dll::load(const char* aPath) throw() {
		std::shared_ptr<dll> r;
		const std::string path = aPath;
		
		// Tread safe access to the cache
		DLL_MAP_LOCK.lock();
			// Check if the library has already beel loaded
			const auto i = DLL_MAP.find(path);
			if(i != DLL_MAP.end()) {
				// Load the cached version of the library
				r = i->second.lock();

				// If the cached version has been deleted then reload it
				if(!r) {
					DLL_MAP.erase(i);
					r = load(aPath);
				}
			}else {
				// Load the library from file
				dll_data_t tmp;
				tmp.handle = LoadLibraryA(aPath);
				if(tmp.handle != NULL) {
					r = std::shared_ptr<dll>(new dll(path, new dll_data_t(tmp)));
					// Cache the library
					DLL_MAP.emplace(path, r);
				}
			}
		DLL_MAP_LOCK.unlock();
		return r;
	}

	/*!
		\brief Create a new library
		\param aPath The file path of the library
		\param aData The OS specific handles for the library
	*/
	dll::dll(const std::string& aPath, dll_data_t* aData) :
		mPath(aPath),
		mData(aData)
	{}

	/*!
		\brief Destroy the library
	*/
	dll::~dll() {
		if(mData) {
			FreeLibrary(mData->handle);
			delete mData;
		}
	}

	/*!
		\brief Get the file path the library was loaded from
		\return The file path
	*/
	const char* dll::get_path() const throw() {
		return mPath.c_str();
	}

	/*!
		\brief Load an untyped function from the library
		\param The name of the function
		\return The address of the function, or nullptr if the load failed
	*/
	void* dll::get_raw_function(const char* aName) throw() {
		return mData ? GetProcAddress(mData->handle, aName) : nullptr;
	}
}
