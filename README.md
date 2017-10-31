# Novelist

[![Build Status](https://travis-ci.org/jan-moeller/novelist.svg?branch=master)](https://travis-ci.org/jan-moeller/novelist)

Novelist aims at becoming an integrated writing environment for authors of novels, shorts, and related prose. It is currently still in an early development phase.

## Build Process
To build Novelist you need to have a recent c++17-enabled compiler, [Qt](https://www.qt.io/) 5.9 or later and CMake 3.8 or later. To build the unit tests, you also need a recent version of the [Catch](https://github.com/philsquared/Catch) unit testing framework.
In terms of compilers, gcc-7 and clang-5 should work. Unfortunately, MSVC 2017 doesn't provide the neccessary c++17 support yet.
Given all these dependencies are in place, you should be able to build novelist using the provided CMake script.

## Project Structure
The project consists of a barebone launcher application which loads shared libraries as plugins. Currently there are two such plugins, the "main" plugin which provides the main application window and much of the base functionality, and a plugin that integrates [LanguageTool](https://www.languagetool.org/) to provide spell and grammar checking. More plugins are planned. Additionally there is a shared "core" library, which provides much the functionality that can be used accross plugins.

## License
Novelist is licensed under GPL-3.0. Please see the [license file](LICENSE). Note that the used external libraries might be licensed under different terms.
