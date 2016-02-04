# README #

Brief tutorial on how to make it compile

### What is this repository for? ###

Virtual 3D sandbox project

### How do I get set up? ###

You need:
* Cmake

* Qt5, for Mac/Unix use a package manager (eg., macports, yum, apt-get, etc...), for Windows download Qt5.5 from http://www.qt.io/download/ and install in C:\

* On Window, you need the kinect v2 sdk https://www.microsoft.com/en-us/download/details.aspx?id=44561

* Modify src/resource/shaders/HeightMap.frag by commenting/uncommenting the first two lines if you have the kinect and you machine supports new shaders

* On windows the kinect is enable by default, if you don't have one, define the macro NO_KINECT