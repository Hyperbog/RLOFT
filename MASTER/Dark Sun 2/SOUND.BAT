@echo off
echo Copying sound information, please wait ...
COPY D:\*.ADV > NUL
SOUND_DS
DEL *.ADV > NUL
exit
