@echo off
echo Usage: make ^<source_file.cpp^> ^<output.exe^>
echo Compiling...

set source_file=%1
set exe_file=%2

shift
shift

g++ -Llib -Iinclude %source_file% -o %exe_file% -lfreeglut -lopengl32 -lglu32
