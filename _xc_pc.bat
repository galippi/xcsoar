::@echo off
set HOME=D:\users\liptak
del D:\users\liptak\.xcsoar\xcsoar-startup.log
if exist c:\temp\conf.exe del c:\temp\conf.exe
copy output\PC\bin\XCSoar.exe c:\temp\conf.exe
c:\temp\conf.exe
if exist D:\users\liptak\.xcsoar\xcsoar-startup.log start D:\users\liptak\.xcsoar\xcsoar-startup.log
