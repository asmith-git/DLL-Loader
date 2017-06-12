# DLL-Loader
## Description
A simple class for loading dynamic libraries.
The stable branch of the code can be found here : https://github.com/asmith-git/DLL-Loader/releases/tag/v2.0

## Features
1. Loading a dynamic library (.dll or .so) from file
2. Automatic memory management, preventing multiple loadings of the same library
3. Loading symbols with either a known or unknown type


## Code Example
```C++
using namespace asmith;

// Loading the library
std::shared_ptr<dynamic_library> myLib = dynamic_library::load_library("example.dll");

// Loading a symbol without knowing it's type
void* unknownSymbol = myLib->load_symbol("myFunction");

// Loading a symbol with a known type
float(*myFun)(int, int) = myLib->load_symbol<float(*)(int, int)>("myFun");

// Another example, for loading a variable
int myVar = *myLib->load_symbol<int>("myVar");
```

## Installation
Header files are located in 'header/'.
Source files are located in 'src/'.

## License
Code is licensed under the Apache 2.0 licence.

