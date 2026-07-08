@echo off
chcp 65001 >nul

set QT_DIR=C:\Qt\6.11.1\msvc2022_64
set OPENCV_DIR=C:\opencv\build

set PATH=%OPENCV_DIR%\x64\vc16\bin;%QT_DIR%\bin;%PATH%

start "" "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe"