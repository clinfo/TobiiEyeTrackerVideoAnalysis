echo off
rem ������
set LABEL=
set OUTPUT=
rem ���͗v��
set /P LABEL="�ΏۂƂ��郉�x������͂��Ă��������i��F1_1�j�F"
rem ���͒lecho
set /P OUTPUT="�o�̓t�@�C��������͂��Ă��������i��Foutput.avi�j�F"
rem ���͒lecho
echo ..\x64\Release\VideoTracker.exe --label %LABEL% --output %OUTPUT%
..\x64\Release\VideoTracker.exe --label %LABEL% --output %OUTPUT%


rem ��~������
rem cmd /k