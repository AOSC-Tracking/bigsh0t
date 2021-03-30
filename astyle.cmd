@SETLOCAL
@ECHO OFF

"C:\Program Files\AStyle\bin\astyle.exe" --style=google --indent=spaces=4 --recursive src/main/cpp/*.cpp src/main/cpp/*.hpp src/test/cpp/*.cpp src/test/cpp/*.hpp
del /S src\main\cpp\*.orig
del /S src\test\cpp\*.orig

@ENDLOCAL