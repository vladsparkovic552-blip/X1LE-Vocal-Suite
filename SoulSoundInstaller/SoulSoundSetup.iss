[Setup]
AppId={{8F41E4B2-6B17-4B91-9C2A-2D3E7F4A2900}}
AppName=SoulSound
AppVersion=2.9.0
AppPublisher=X1LE
DefaultDirName={commoncf}\VST3
DisableProgramGroupPage=yes
Uninstallable=no
OutputDir=Output
OutputBaseFilename=SoulSound_Setup_2.9.0
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
WizardStyle=modern
WizardSizePercent=110
SetupLogging=yes

[Languages]
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Code]
const
  PluginUrl = 'https://github.com/vladsparkovic552-blip/X1LE-Vocal-Suite/releases/latest/download/SoulSound_beta_2.9.0_1118_0004.zip';

function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  ScriptPath: String;
  ScriptText: String;
  ResultCode: Integer;
begin
  Result := '';
  ScriptPath := ExpandConstant('{tmp}\install-soulsound.ps1');

  ScriptText :=
    '$ErrorActionPreference = "Stop"' + #13#10 +
    '$ProgressPreference = "SilentlyContinue"' + #13#10 +
    '$archive = Join-Path $env:TEMP "SoulSound_beta_2.9.0_1118_0004.zip"' + #13#10 +
    '$work = Join-Path $env:TEMP ("SoulSound_Install_" + [guid]::NewGuid().ToString())' + #13#10 +
    'try {' + #13#10 +
    '  Invoke-WebRequest -UseBasicParsing -Uri "' + PluginUrl + '" -OutFile $archive' + #13#10 +
    '  New-Item -ItemType Directory -Path $work -Force | Out-Null' + #13#10 +
    '  Expand-Archive -LiteralPath $archive -DestinationPath $work -Force' + #13#10 +
    '  $source = Join-Path $work "SoulSound.vst3"' + #13#10 +
    '  $binary = Join-Path $source "Contents\x86_64-win\SoulSound.vst3"' + #13#10 +
    '  if (-not (Test-Path -LiteralPath $binary -PathType Leaf)) { throw "The downloaded archive does not contain a valid x64 VST3 plugin." }' + #13#10 +
    '  $folder = Join-Path $env:CommonProgramFiles "VST3"' + #13#10 +
    '  New-Item -ItemType Directory -Path $folder -Force | Out-Null' + #13#10 +
    '  $destination = Join-Path $folder "SoulSound.vst3"' + #13#10 +
    '  if (Test-Path -LiteralPath $destination) {' + #13#10 +
    '    $backup = $destination + ".backup-" + (Get-Date -Format "yyyyMMdd-HHmmss")' + #13#10 +
    '    Move-Item -LiteralPath $destination -Destination $backup' + #13#10 +
    '  }' + #13#10 +
    '  Copy-Item -LiteralPath $source -Destination $destination -Recurse -Force' + #13#10 +
    '  Write-Output "SoulSound 2.9.0 installed successfully."' + #13#10 +
    '} finally {' + #13#10 +
    '  if (Test-Path -LiteralPath $archive) { Remove-Item -LiteralPath $archive -Force -ErrorAction SilentlyContinue }' + #13#10 +
    '  if (Test-Path -LiteralPath $work) { Remove-Item -LiteralPath $work -Recurse -Force -ErrorAction SilentlyContinue }' + #13#10 +
    '}' + #13#10;

  if not SaveStringToFile(ScriptPath, ScriptText, False) then
  begin
    Result := 'Could not prepare the SoulSound installation script.';
    Exit;
  end;

  if not Exec(ExpandConstant('{sys}\WindowsPowerShell\v1.0\powershell.exe'),
    '-NoProfile -ExecutionPolicy Bypass -File "' + ScriptPath + '"',
    '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
  begin
    Result := 'Could not start Windows PowerShell. Please try again.';
    Exit;
  end;

  if ResultCode <> 0 then
    Result := 'Installation failed. Check your internet connection and make sure the SoulSound ZIP is published in GitHub Releases as "' +
      'SoulSound_beta_2.9.0_1118_0004.zip".' + #13#10 +
      'You can also review the setup log for details.';
end;

function InitializeSetup(): Boolean;
begin
  Result := IsWin64;
  if not Result then
    MsgBox('SoulSound requires 64-bit Windows.', mbError, MB_OK);
end;
