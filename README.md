# Virtual 3D sandbox project

## How do I get set up?

You need:

* C++ compiler
* cmake
* Qt5
* OpenCV
* Kinect v2 SDK

Notes:

* You can modify `src/resource/shaders/HeightMap.frag` by commenting/uncommenting the first two lines if you have the Kinect and your machine supports new shaders
* On windows the Kinect is enabled by default; if you don't have one, define the macro `NO_KINECT`

### Linux

* `sudo apt install cmake`
* `sudo apt install qt5-default`
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/)

### Mac

* Install XCode from the Apple Store
* Install [homebrew](https://brew.sh/)
* Install dependencies with homebrew: `brew install cmake qt opencv`
* Add Qt to PATH: `echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.bashrc` or `echo 'export PATH="/usr/local/opt/qt/bin:$PATH"' >> ~/.zshrc` depending on your shell

### Windows

* Install Visual Studio (the community version is free) to obtain the C++ compiler. Select "Desktop development"
* Download and install cmake from its [official website](https://cmake.org/download/)
* Download Qt5 from the [official website](https://www.qt.io/download/). Choose the Open Source version, install it in `C:\`
* Download OpenCV from their [Sourceforge page](https://sourceforge.net/projects/opencvlibrary/) and extract in `C:\`
* Download and install the [Kinect 2.0 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=44561)
* Set the environment variable `Qt5_DIR` to the x64 installation of Qt (e.g. `C:\Qt\5.15.0\msvc2019_64`)
* Add the Qt and OpenCV bin folders (e.g. `C:\Qt\5.15.0\msvc2019_64\bin` and `C:\opencv\build\x64\vc15\bin`) to PATH and reboot your machine

## Compiling

Use cmake to generate a makefile (on Linux and Mac), or a Visual Studio project (on Windows).

### Linux/Mac

* `mkdir bin`
* `cd bin`
* `cmake ..`
* `make`
* `./ARSandBox`

### Windows

* Open the cmake GUI
* Under "Where is the source code", select the root folder of the project
* Under "Where to build the binaries" select the bin folder in the root directory
* Click "Configure" and choose your version of Visual Studio
* Click "Generate" until there are no more red lines to generate the Visual Studio Solution
* Click "Open Project" to open it
* In Visual Studio, choose "Release" instead of "Debug"
* Under "Build" choose "Build Solution"
* Run `bin\Release\ARSandBox.exe`

Note: if you get errors about certain DLLs, delete those in the `bin\Release` folder and
run `windeployqt ARSandBox.exe` in there

## Run @ USI

See [these instructions](https://github.com/USI-Showroom/ARSandBox/blob/master/docs/ARSandBox_Manual_ITA.pdf)

## Common keys

`ESC`:         exit\
`P`:           save mesh/point cloud\
`F5`:          exit setup mode\
`-`:           turn off visualization and show only sand

### In setup mode

`Shift`:       slower movement

`F1`:          terrain 1 (mountains with cows)\
`F2`:          terrain 2 (lava)\
`F3`:          terrain 3 (plains with fishes)

`1/2/3/4`:     select corners\
`5`:           scale\
`6`:           move

`W/A/S/D`:     move the current selection in the four directions

`Space`:       toggle preview

`9`:           save current calibration to disk\
`0`:           load calibration from disk

`U/J`:         increase/decrease the minimum height\
`I/K`:         increase/decrease the maximum height
