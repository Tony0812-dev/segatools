@echo off

pushd %~dp0

start /min inject -d -k mu3hook.dll amdaemon.exe -f -c config_common.json config_server.json config_client.json
inject -d -k mu3hook.dll mu3 -screen-fullscreen 0
taskkill /f /im amdaemon.exe > nul 2>&1

echo.
echo Game processes have terminated
pause