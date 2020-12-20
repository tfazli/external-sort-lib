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

You can see an example of using this library in the "examples" folder.


# Build process

## 1. Build dependencies

No special build dependencies are needed. Only standart g++ and cmake are used.

## 2. Build procedure

Here will be the build procedure description.


# Directory Structure

Below is a partial directory structure for this project to understand the general location of the files.

> **`NOTE:`**  BUILD directory should be created by user!

```bash
├── BUILD (user created)
├── Other directories will be explained here
```

