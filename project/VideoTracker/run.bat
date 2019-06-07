echo off
rem 初期化
set LABEL=
set OUTPUT=
rem 入力要求
set /P LABEL="対象とするラベルを入力してください（例：1_1）："
rem 入力値echo
set /P OUTPUT="出力ファイル名を入力してください（例：output.avi）："
rem 入力値echo
echo ..\x64\Release\VideoTracker.exe --label %LABEL% --output %OUTPUT%
..\x64\Release\VideoTracker.exe --label %LABEL% --output %OUTPUT%


rem 停止させる
rem cmd /k