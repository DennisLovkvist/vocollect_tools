# vocollect_tools
Tools for analyzing and visualizing vocollect log files.



# vocollect_tools
## Description
Tools for analyzing and visualizing vocollect log files.

Language: c++

Dependencies: sfml

## Installation
### Install prerequisites
* gcc
* sfml

### Build
```
$ git clone https://github.com/DennisLovkvist/vocollect_tools.git
$ cd vocollect_tools

$ ./vcclean/build.ps1
$ ./vcgenpick/build.ps1
$ ./vcnetstat/build.ps1
$ ./vcnetmap/build.ps1

$ ./install.sh
```
### Usage

Open terminal and change directory to vocollect log file.
```
$ vcclean "log.txt"
$ vcgenpick "log_parsed.txt"
$ vcnetstat "pickresults.txt"
```
![Alt text](screenshot_0.png?raw=true "Screenshot")

Run vcnetmap with external aisle and access point definitions
```
$ vcnetmap "pickresults.txt" "aisle_data.txt" "access_point_data.txt"
```

Run vcnetmap with default aisle and access point definitions
```
$ vcnetmap "pickresults.txt"
```
![Alt text](screenshot_1.png?raw=true "Screenshot")



