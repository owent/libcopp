chcp 65001

where pwsh

if not ERRORLEVEL 0 (
    goto USE_POWERSHELL
)

:USE_POWERSHELL_CORE

@pwsh.exe -NoProfile -InputFormat None -ExecutionPolicy Bypass -Interactive -NoExit -File setup-sphinx.ps1

pause

exit %ERRORLEVEL%

:USE_POWERSHELL

@"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -InputFormat None -ExecutionPolicy Bypass -Interactive -NoExit -File setup-sphinx.ps1

pause
