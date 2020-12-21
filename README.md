# Multithreaded External Sort Library

## Description

It is a multi-threaded external sorting library that allows you to sort in ascending order `**a file of any size**`, no matter how much RAM or how many kernel threads you have on your PC. The user-friendly interface allows you to set the allowed amount of RAM and the number of threads yourself, as well as the data type to read from the file - the library will adjust itself to the specified parameters.

## Available setting options

1. **Flexible control over RAM consumption.** The library user can set the maximum allowed RAM size available to the library.
2. **Flexible control over CPU consumption.** The library user can set the maximum allowed number of CPU threads available to the library.
3. **Flexible control of the input data type.** The library user can set any required data type to read from the file.

## Important notice

> **`NOTE 1:`**  Allowed RAM size is set in **megabytes**. For the best library performance it is important to set allowed RAM size **`more then 1Mb`**!

> **`NOTE 2:`**  Due to the algorithm it is critically important to **`set allowed number of CPU threads as the power of two`**! For example 2, 4, 8, 16 etc.

## How to use this library

> **`NOTE:`** You can see an example of using this library in the "examples" folder.

Using this library is not complicated and comes down to several stages:

1. Include `"ExternalSorter.h"` into your project.
2. Check if the input file exist (using standart tools of STL).
3. Create an `ExternalSorter` class object with the several parameters in the order below:
   - Available RAM size **in megabytes** (size_t type);
   - The number of threads allowed to use (size_t type);
   - Input file name (std::string type);
   - Output file name (std::string type).
4. Call the `sort()` method from the object.

```cpp
ExternalSorter<uint32_t> externalSorter(availableRamMegabytes,
                                        maxSupportedThreads,
                                        inputFileName,
                                        outputFileName);
externalSorter.sort();
```


# Build process

## 1. Build dependencies

No special build dependencies are needed. Only standart g++ and cmake are used.

## 2. Build procedure

1. Clone the repo
2. Create /BUILD directory inside project directory
3. Import `CMakeLists.txt` file from project directory into your IDE. This should create a new project
4. Import the route to /BUILD directory location as your build location inside project settings
5. Import or update some of cmake variables inside project settings. You can use different options to build this library, here is the list:
   - You can set `EXAMPLE_REQUIRED=ON` to build library with all the examples (in the command line use -DEXAMPLE_REQUIRED flag). By default it is turned OFF.
   - set CMAKE_EXE_LINKER_FLAGS=**`"${CMAKE_EXE_LINKER_FLAGS} -pthread -lpthread"`**
6. You are ready to build the project

> **NOTE:** don't forget to use **`CMAKE_BUILD_TYPE=Release`** flag (or release setting in your IDE) in case that you are preparing to release some stable version of the package.


# Directory Structure

Below is a partial directory structure for this project to understand the general location of the files.

> **`NOTE:`**  BUILD directory should be created by user!

```bash
├── BUILD (user created)
├── examples
│   └── (all the usage examples)
├── include
│   └── (all the headers)
├── src
│   └── (all the sources)
├── LICENSE
├── README.md
├── VERSION
└── .gitignore
```

