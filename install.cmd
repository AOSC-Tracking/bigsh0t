@SETLOCAL
@ECHO OFF
SET SHOTCUT_HOME="C:\Program Files\Shotcut"
SET PACKAGE_NAME=bigsh0t-2.1.1-win
xcopy /Y /S Release\%PACKAGE_NAME%\shotcut\share\shotcut\qml\filters %SHOTCUT_HOME%\share\shotcut\qml\filters
xcopy /Y /S Release\%PACKAGE_NAME%\lib\frei0r-1 %SHOTCUT_HOME%\lib\frei0r-1
@ENDLOCAL