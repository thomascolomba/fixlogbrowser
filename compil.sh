#!/bin/bash

#we build our plugin

#building .o
wine mingw32-c++.exe -c NppPluginDemo.cpp PluginDefinition.cpp -DUNICODE

#building .dll
wine mingw32-c++.exe -shared -o my_plugin_dll.dll NppPluginDemo.o PluginDefinition.o -Wl,--out-implib,my_plugin_dll.a

#building a program using our dll
#wine mingw32-c++.exe -c example_exe.cpp
#wine mingw32-c++.exe -o example_exe.exe example_exe.o -L. -lexample_dll

