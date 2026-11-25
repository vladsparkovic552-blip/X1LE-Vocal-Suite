@echo off
setlocal
title SoulSound 2.9.0 Installer
color 0B
echo.
echo  ============================================
echo             SOULSOUND 2.9.0
echo          Automatic VST3 Installer
echo  ============================================
echo.
net session >nul 2>&1
if %errorlevel% neq 0 (
  echo Requesting administrator permission...
  powershell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
  exit /b
)
set "URL=https://github.com/vladsparkovic552-blip/X1LE-Vocal-Suite/releases/latest/download/SoulSound_beta_2.9.0_1118_0004.zip"
set "ARCHIVE=%TEMP%\SoulSound_beta_2.9.0_1118_0004.zip"
echo  Downloading SoulSound from GitHub Releases...
powershell -NoProfile -ExecutionPolicy Bypass -Command "$ErrorActionPreference='Stop'; $ProgressPreference='SilentlyContinue'; Invoke-WebRequest -UseBasicParsing -Uri '%URL%' -OutFile '%ARCHIVE%'; $tmp=Join-Path $env:TEMP ('SoulSound_'+[guid]::NewGuid()); try { Expand-Archive -LiteralPath '%ARCHIVE%' -DestinationPath $tmp -Force; $src=Join-Path $tmp 'SoulSound.vst3'; if (!(Test-Path (Join-Path $src 'Contents\x86_64-win\SoulSound.vst3'))) { throw 'Invalid VST3 archive' }; $folder=Join-Path $env:CommonProgramFiles 'VST3'; New-Item -ItemType Directory -Force $folder | Out-Null; $dst=Join-Path $folder 'SoulSound.vst3'; if (Test-Path $dst) { Move-Item $dst ($dst+'.backup-'+(Get-Date -Format yyyyMMdd-HHmmss)) }; Copy-Item $src $dst -Recurse -Force; Write-Host 'SoulSound installed successfully.' -ForegroundColor Green } finally { Remove-Item $tmp -Recurse -Force -ErrorAction SilentlyContinue; Remove-Item '%ARCHIVE%' -Force -ErrorAction SilentlyContinue }"
if errorlevel 1 (
  echo.
  echo  Installation failed. Check that the ZIP is published in GitHub Releases.
  pause
  exit /b 1
)
echo.
echo  Installation complete. Restart FL Studio and rescan plugins.
pause
