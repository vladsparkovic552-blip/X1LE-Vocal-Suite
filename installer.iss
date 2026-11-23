#ifndef PluginSource
  #error PluginSource was not supplied by the build workflow
#endif

[Setup]
AppId={{A3B5C9D1-8A21-4E2D-9F70-51E1A1C0B123}}
AppName=X1LE Vocal Suite
AppVersion=1.0.0
AppPublisher=X1LE
DefaultDirName={commoncf}\VST3
DisableProgramGroupPage=yes
OutputDir=Output
OutputBaseFilename=X1LE_Vocal_Suite_Setup
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
WizardStyle=modern

[Files]
Source: "{#PluginSource}\\*"; DestDir: "{commoncf}\\VST3\\X1LE Vocal Suite.vst3"; Flags: recursesubdirs createallsubdirs ignoreversion

[Code]
function InitializeSetup(): Boolean;
begin
  Result := IsWin64;
  if not Result then
    MsgBox('X1LE Vocal Suite requires 64-bit Windows.', mbError, MB_OK);
end;
