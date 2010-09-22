@echo off
rem path=C:\Programs\cygwin\bin;%PATH%
path=C:\Programok\cygwin\bin;%PATH%
rem set TMPDIR=c:\tmp
set WORK_ROOT=%CD%
::set WXDIR=/wxMSW

::C:
::chdir C:\Programs\cygwin\bin

rem start "FESI make" bash --login -i -c "make_fesi"
rem bash --login -i -c "ls -l ma*"
rem bash --login -i -c "cd $WORKROOT|ls"
rem bash --login -i -c "make -C $WORK_ROOT -f makefile.dos %1 %2 %3 %4"
::bash --login -i -c "make -C $WORK_ROOT TARGET=PC WINHOST=y %1 %2 %3 %4"
bash --login -i -c "make -C $WORK_ROOT TARGET=PC WINHOST=y V=2 %1 %2 %3 %4"
::bash --login -i -c "make -C $WORK_ROOT TARGET=CYGWIN WINHOST=y %1 %2 %3 %4"
