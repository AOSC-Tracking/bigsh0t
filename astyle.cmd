@SETLOCAL
@ECHO OFF

"C:\Program Files\AStyle\bin\astyle.exe" --style=google --indent=spaces=4 --recursive src/main/cpp/*.cpp src/main/cpp/*.hpp
del /S src\main\cpp\*.orig

@ENDLOCAL