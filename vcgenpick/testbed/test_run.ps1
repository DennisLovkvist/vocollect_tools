$root = [System.IO.Path]::GetDirectoryName($myInvocation.MyCommand.Definition);

Start-Process -FilePath ($root + "/vcgenpick.exe") -ArgumentList ($root + "/optiscan_log.txt") -WorkingDirectory $root;