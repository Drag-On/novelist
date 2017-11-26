# Novelist

[![Build Status](https://travis-ci.org/jan-moeller/novelist.svg?branch=master)](https://travis-ci.org/jan-moeller/novelist)

Novelist aims to become an integrated writing environment for authors of novels, shorts, and related prose. It is currently still in an early development phase.

## Build Process
To build Novelist you need to have a recent c++17-enabled compiler, [Qt](https://www.qt.io/) 5.9 or later and CMake 3.8 or later. To build the unit tests, you also need a recent version of the [Catch](https://github.com/philsquared/Catch) unit testing framework.
In terms of compilers, gcc-7 and clang-5 should work. Unfortunately, MSVC 2017 doesn't provide the neccessary c++17 support yet.
Given all these dependencies are in place, you should be able to build novelist using the provided CMake script. For instructions, see [this](https://github.com/jan-moeller/novelist/wiki/Building-from-Source) Wiki page.

## Project Structure
The project consists of a barebone launcher application which loads shared libraries as plugins. Currently, the following plugins are implemented:
- main: provides the main application window and much of the base functionality.
- languagetool: integrates [LanguageTool](https://www.languagetool.org/) to provide spell and grammar checking.
- stats: provides some interesting statistics about the project, e.g. how the word count developed over time.

Additionally there is a shared "core" library, which provides much of the functionality that can be used accross plugins.

## License
Novelist is licensed under GPLv3. Please see the [license file](LICENSE). Note that the used external libraries might be licensed under different terms.
