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

#include <map>
#include <string>
#include <mutex>
#include <algorithm>
#include <string>

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#else
	#include <sys/types.h>
	#include <dlfcn.h>
#endif

namespace asmith {

	struct dll_data_t {
	}; //!< \brief Contains OS specific handles for dynamic libraries, for internal use only.
	
	static std::map<std::string, std::weak_ptr<dynamic_library>> DLL_MAP;
	static std::mutex DLL_MAP_LOCK;

	class os_dynamic_library : public dynamic_library {
	private:
#ifdef WIN32
		typedef HMODULE handle_t;
#else
		typedef void* handle_t;
#endif

		const std::string mPath;
		handle_t mHandle;
	public:
		os_dynamic_library(const char* const aPath) :
			mPath(aPath)
		{
			#ifdef WIN32
				mHandle = LoadLibraryA(aPath);
			#else
				mHandle = dlopen(aPath, RTLD_LAZY);
			#endif
		}

		~os_dynamic_library() {
			#ifdef WIN32
				FreeLibrary(mHandle);
			#else
				dlclose(mHandle);
			#endif
		}

		// Inherited from dynamic_library

		const char* get_path() const throw() override {
			return mPath.c_str();
		}

		void* load_symbol(const char* aName) {
			return 
			#ifdef WIN32
				GetProcAddress
			#else
				dlsym
			#endif
				(mHandle, aName);
		}
	};

	// dynamic_library

	/*!
		\brief Load a dynamic link library from file
		\param aPath The file path of the library
		\return The libary object, null if the library failed to load.
	*/
	std::shared_ptr<dynamic_library> dynamic_library::load_library(const char* aPath) throw() {
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
}
