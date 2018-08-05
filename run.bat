setlocal

PATH=.\libs;%PATH%
set QT_QPA_PLATFORM_PLUGIN_PATH=.\libs\platforms

start procalc.exe %*
