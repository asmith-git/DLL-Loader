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

#include "asmith/dll-loader/dynamic_library.hpp"

#include <map>
#include <mutex>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <dlfcn.h>
#endif

namespace asmith {
	
	static std::map<std::string, std::weak_ptr<dynamic_library>> DLL_MAP;
	static std::mutex DLL_MAP_LOCK;

	class os_dynamic_library : public dynamic_library {
	private:
		const std::string mPath;
#ifdef WIN32
		HMODULE mHandle;
#else
		void* mHandle;
#endif
	public:
		os_dynamic_library(const char* const aPath) :
			mPath(aPath)
		{
			#ifdef WIN32
				mHandle = LoadLibraryA(aPath);
				if(mHandle == NULL) throw library_load_exception(aPath);
			#else
				mHandle = dlopen(aPath, RTLD_LAZY);
				if(dlerror() != nullptr) throw library_load_exception(aPath);
			#endif
		}

		~os_dynamic_library() {
			#ifdef WIN32
				if(mHandle != NULL) {
					if(! FreeLibrary(mHandle)) throw library_close_exception(mPath.c_str());
					mHandle = NULL;
				}
			#else
				if(mHandle != nullptr) {
					dlclose(mHandle);
					if(dlerror() != nullptr) throw library_close_exception(mPath.c_str());
					mHandle = nullptr;
				}
			#endif
		}

		// Inherited from dynamic_library

		const char* get_path() const throw() override {
			return mPath.c_str();
		}

		void* load_symbol(const char* aName) {
			#ifdef WIN32
				void* const symbol = GetProcAddress(mHandle, aName);
				if(symbol == NULL) throw symbol_not_found_exception(aName, shared_from_this());
				return symbol;
			#else
				void* const symbol = dlsym(mHandle, aName);
				if(dlerror() != nullptr) throw symbol_not_found_exception(aName, shared_from_this());
				return symbol;
			#endif
		}
	};

	// dynamic_library

	/*!
		\brief Load a dynamic link library from file
		\param aPath The file path of the library
		\return The libary object, null if the library failed to load.
	*/
	std::shared_ptr<dynamic_library> dynamic_library::load_library(const char* aPath) {
		std::shared_ptr<dynamic_library> r;
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
					r = load_library(aPath);
				}
			}else {
				// Load the library from file
				r = std::shared_ptr<dynamic_library>(new os_dynamic_library(aPath));
				
				// Cache the library
				DLL_MAP.emplace(path, r);
			}
		DLL_MAP_LOCK.unlock();
		return r;
	}

	// dynamic_symbol_not_found

	symbol_not_found_exception::symbol_not_found_exception(const char* aName, std::shared_ptr<dynamic_library> aLibrary) :
		mMessage("The symbol '" + std::string(aName) + "' could not be found"),
		symbol(aName),
		library(aLibrary)
	{}

	const char* symbol_not_found_exception::what() const throw() {
		return mMessage.c_str();
	}

	// library_load_exception
	
	library_load_exception::library_load_exception(const char* aPath) :
		mMessage("The dynamic library '" + std::string(aPath) + "' could not be loaded"),
		path(aPath)
	{}

	const char* library_load_exception::what() const throw() {
		return mMessage.c_str();
	}

	// library_close_exception

	library_close_exception::library_close_exception(const char* aPath) :
		mMessage("The dynamic library '" + std::string(aPath) + "' could not be closed"),
		path(aPath)
	{}

	const char* library_close_exception::what() const throw() {
		return mMessage.c_str();
	}
}
