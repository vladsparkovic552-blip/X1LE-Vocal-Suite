@echo off
setlocal
cd /d "%~dp0"
if not exist "%~dp0SoulSound_beta_2.9.0_1118_0004.zip" (
 echo Place SoulSound_beta_2.9.0_1118_0004.zip next to this file.
 pause
 exit /b 1
)
net session >nul 2>&1
if %errorlevel% neq 0 (
 powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
 exit /b
)
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $tmp=Join-Path $env:TEMP ('SoulSound_'+[guid]::NewGuid()); Expand-Archive -LiteralPath '%~dp0SoulSound_beta_2.9.0_1118_0004.zip' -DestinationPath $tmp -Force; $src=Join-Path $tmp 'SoulSound.vst3'; if (!(Test-Path (Join-Path $src 'Contents\x86_64-win\SoulSound.vst3'))) { throw 'Invalid SoulSound VST3 archive' }; $dst=Join-Path $env:CommonProgramFiles 'VST3\SoulSound.vst3'; New-Item -ItemType Directory -Force (Split-Path $dst) | Out-Null; if (Test-Path $dst) { Move-Item $dst ($dst+'.backup-'+(Get-Date -Format yyyyMMdd-HHmmss)) }; Copy-Item $src $dst -Recurse -Force; Remove-Item $tmp -Recurse -Force; Write-Host 'SoulSound 2.9.0 installed. Rescan plugins in FL Studio.'; pause"
