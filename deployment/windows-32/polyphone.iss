; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[InstallDelete]  
Type: files; Name: "{app}\icudt52.dll" 
Type: files; Name: "{app}\icuin52.dll"
Type: files; Name: "{app}\icuuc52.dll"
Type: files; Name: "{app}\libgcc_s_dw2-1.dll" 
Type: files; Name: "{app}\libjack.dll"
Type: files; Name: "{app}\libstdc++-6.dll"
Type: files; Name: "{app}\libwinpthread-1.dll" 
Type: files; Name: "{app}\mingwm10.dll"
Type: files; Name: "{app}\Qt5Core.dll"
Type: files; Name: "{app}\Qt5Gui.dll" 
Type: files; Name: "{app}\Qt5PrintSupport.dll"
Type: files; Name: "{app}\Qt5Svg.dll"
Type: files; Name: "{app}\Qt5Widgets.dll"  
Type: files; Name: "{app}\zlib1.dll"
Type: files; Name: "{app}\platforms\qwindows.dll"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{B0A96B4A-783D-4090-89D6-0B66C8D71B42}
AppName=Polyphone
AppVersion=2.0
AppPublisher=Davy Triponney
AppPublisherURL=https://www.polyphone-soundfonts.com
AppSupportURL=https://www.polyphone-soundfonts.com
AppUpdatesURL=https://www.polyphone-soundfonts.com
ChangesAssociations=yes
DefaultDirName={pf}\Polyphone
DefaultGroupName=Polyphone
LicenseFile=license.txt
OutputBaseFilename=Polyphone_v2.0_win32
SetupIconFile=polyphone.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "czech";  MessagesFile: "compiler:Languages\Czech.isl"
Name: "french";  MessagesFile: "compiler:Languages\French.isl"
Name: "german";  MessagesFile: "compiler:Languages\German.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "serbian"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "polyphone.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\icudt53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\icuin53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\icuuc53.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\libcryptoMD.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\libeay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\libogg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\libvorbis.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\libvorbisfile.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\msvcp120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\msvcr120.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\portaudio_x86.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\Qt5Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\Qt5Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\Qt5Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\Qt5Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\Qt5Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\qwindows.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "lib\ssleay32.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "lib\zlib1.dll"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Polyphone"; Filename: "{app}\polyphone.exe"
Name: "{commondesktop}\Polyphone"; Filename: "{app}\polyphone.exe"; Tasks: desktopicon

[Registry] 
Root: HKCR; Subkey: ".sf2"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sf3"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sfz"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sfArk"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue 
Root: HKCR; Subkey: "PolyphoneFile"; ValueType: string; ValueName: ""; ValueData: "soundfont"; Flags: uninsdeletekey 
Root: HKCR; Subkey: "PolyphoneFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\polyphone.exe,1" 
Root: HKCR; Subkey: "PolyphoneFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\polyphone.exe"" ""%1""" 

[Run]
Filename: "{app}\polyphone.exe"; Description: "{cm:LaunchProgram,Polyphone}"; Flags: nowait postinstall skipifsilent

