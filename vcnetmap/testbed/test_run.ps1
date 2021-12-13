$root = [System.IO.Path]::GetDirectoryName($myInvocation.MyCommand.Definition);

$args = ($root + "/aisles.txt") + " " + ($root + "/access_points.txt")  + " " +  ($root + "/pick_events.txt")
Start-Process -FilePath ($root + "/vcnetmap.exe") -ArgumentList $args -WorkingDirectory $root;