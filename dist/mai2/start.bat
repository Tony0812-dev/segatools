@echo off

pushd %~dp0

start /min inject -d -k mai2hook.dll amdaemon.exe -c config_common.json config_server.json config_client.json
inject -d -k mai2hook.dll sinmai -screen-fullscreen 0
taskkill /f /im amdaemon.exe > nul 2>&1

echo.
echo Game processes have terminated
pause