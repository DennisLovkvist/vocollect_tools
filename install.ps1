$root = [System.IO.Path]::GetDirectoryName($myInvocation.MyCommand.Definition);

$install_path = ($env:ProgramFiles + "\vocollect_tools");

if(![System.IO.Directory]::Exists($install_path))
{
    New-Item -path $install_path -ItemType Directory;
}

cp ($root + "\vcclean\build\vcclean.exe") $install_path;
cp ($root + "\vcgenpick\build\vcgenpick.exe") $install_path;
cp ($root + "\vcnetmap\build\vcnetmap.exe") $install_path;
cp ($root + "\vcnetstat\build\vcnetstat.exe") $install_path;

cp ($root + "\vcnetmap\build\arial.ttf") $install_path;
cp ($root + "\vcnetmap\build\sfml-graphics-2.dll") $install_path;
cp ($root + "\vcnetmap\build\sfml-system-2.dll") $install_path;
cp ($root + "\vcnetmap\build\sfml-window-2.dll") $install_path;


