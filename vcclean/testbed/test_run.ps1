$root = [System.IO.Path]::GetDirectoryName($myInvocation.MyCommand.Definition);

Start-Process -FilePath ($root + "/vcclean.exe") -ArgumentList ($root + "/vocollect_log.txt") -WorkingDirectory $root;