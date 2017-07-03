# README #

## What is this repository for? ##

Virtual 3D sandbox project

## How do I get set up? ##

You need:

* Cmake

* Qt5, for Mac/Unix use a package manager (eg., macports, yum, apt-get, etc...), for Windows download Qt5.5 from http://www.qt.io/download/ and install in C:\

* On Window, you need the kinect v2 sdk https://www.microsoft.com/en-us/download/details.aspx?id=44561

* Modify src/resource/shaders/HeightMap.frag by commenting/uncommenting the first two lines if you have the kinect and you machine supports new shaders

* On windows the kinect is enable by default, if you don't have one, define the macro NO_KINECT


## User Manual ##

Remember to turn off stereo mode from the projector's menu

### Common keys ###
* ESC         exit
* P           save mesh/point cloud
* F5          toggle set-up mode
* -<minus>    turn off visualisation and show only sand

  
### In set-up mode ###

* Shift       slower movement


* F1          terrain 1
* F2          terrain 2
* F3          terrain 3

* 1/2/3/4     select corners
* 5           scale
* 6           move

* W/A/S/D        move the current selection in the 4 directions

* Space       toggle Kinect calibration/projector calibration



* 9           save current calibration to disk
* 0           load calibration from disk

* U/J         increase/decrease the minimum height
* I/K         increase/decrease the maximum height