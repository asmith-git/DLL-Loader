# DLL-Loader
## Description
A simple class for loading dynamic link libraries (DLL). Currently only implemented for Windows OS.
The stable branch of the code can be found here : https://github.com/asmith-git/DLL-Loader/tree/Stable-1.0

## Features
1. Loading DLL from file
2. Automatic memory management, including prevention of a single library being duplicated multiple times
3. Loading function pointers with either known or unknown types
4. Loading variables with known types


## Code Example
```C++
using namespace asmith;

// Loading the library
std::shared_ptr<dll> myDLL = dll::load("example.dll");

// Checking the library has loaded
if(! myDLL) std::cerr << "Help, myDLL failed to load!" << std::endl;

// Loading an unknown function
void* unknownFunction = mDLL->get_raw_function("myFunction");

// Loading a known function
float(ASMITH_DLL_CALLING_CONVENTION *knownFunction)(int, int) = mDLL->get_functon<float,int,int>("myFunction");

// Alternatively...
dll_function<float, int, int> knownFunction2 = mDLL->get_functon<float,int,int>("myFunction");

// Loading a variable
float myVariable = mDLL->get_variable<float>("myFunction");
```

## Installation
Header files are located in 'header/'.
Source files are located in 'src/'.

## License
Code is licensed under the Apache 2.0 licence.

