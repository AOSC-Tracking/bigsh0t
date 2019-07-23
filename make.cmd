@SETLOCAL
@ECHO OFF
mkdir Release
cd Release

cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build .


@ENDLOCAL