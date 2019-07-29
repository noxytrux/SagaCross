# SagaCross

Quake3 like cross-platform tanks death match game. You can consider this as a yet another Battle Royale shooter. 

Currently supported platforms: Win, Linux, macOS, HTML5, iOS, Android.

## Releases

You can play web demo version [here](http://noxytrux.github.io/sagacross/)

## Gallery

![1](../master/screenshots/1.jpg)
![2](../master/screenshots/2.jpg)
![3](../master/screenshots/3.jpg)

## System requirements:

* 32Bit operating system 
* Single core CPU running at about 1Ghz (2Ghz recommended)
* 128MB RAM (256 recommended)
* GPU able to run GL2.0 ES or 3.2Core Profile with about 128MB (256 recommended)
* About 60MB disk space (depends on platform and build type)
* Music card (PC) 
* Mouse (PC)
* Monitor capable of running in 16bit colour mode with at least 1024x768 resolution (PC)

## Controls

Desktop:

* WASD to move
* LMB fire
* RMB drop mine

Joystick:

* D-pad or left stick to move
* Right stick to aim (analog)
* R1 to drop mine
* R2 to shoot 

Mobile:

* Touch and drag left side of screen to move  
* Touch and drag right side of screen to aim (release to shoot)
* Tap mine button to drop mine

## Contents
 
[1. 3rd party libraries](#1-3rd-party-libraries)  
[2. Building](#2-building)  
[3. License](#3-license)

# 1. 3rd party libraries

This project use the following 3rd party libraries:
  
Graphics library - [OpenGL](https://www.khronos.org/opengl)  
Sound library - [FMOD](https://www.fmod.com)  
Image loading - [stb_image](https://github.com/nothings/stb)  
UI - [nuclear](https://github.com/vurtun/nuklear)  
Window creation and input [GLFW](http://www.glfw.org)  
OpenGL extensions [GLEW](http://glew.sourceforge.net)

# 2. Building  

### iOS

Before compiling you need to download [Xcode](https://developer.apple.com/xcode/)

* Navigate to SagaCross/platform/iOS
* Open SagaCross.xcodeproj
* Press `cmd + r` to build and run


### macOS

Before compiling and running you need to download additional libraries:

```
brew install glfw3
brew install glew
brew install cmake
```

* Open Terminal or shell
* Navigate to project root directory
* Type `make` and press enter
* When finished type `./Game` to run

### Linux

Before compiling and running you need to download additional libraries:

```
sudo apt-get install libglfw3-dev 
sudo apt-get install libglew-dev
sudo apt-get install cmake
```

* Open Terminal or shell
* Navigate to project root directory
* Type `make` and press enter
* When finished type `./Game` to run

### Windows

This project require at least `Visual Studio 2017 community version`  	 

* Navigate to project root directory
* Open `SagaCross.sln`
* Press F5 or play button to build and run.

### HTML5

To build it you need to download and install [Emscripten](http://kripken.github.io/emscripten-site/) (please follow instructions on site)

* Open Terminal or shell
* Navigate to SagaCross/platform/html5
* Type `emmake make` 
* Build will be placed in `bin` directory  

To run game you will need to start some local server e.g: by running `python -m SimpleHTTPServer 8000` in the same directory where you run `enmake make`  

Now open browser and type `localhost:8000` it will open in `HTML5` directory. Navigate to `bin` and klick on `SagaCross.html` the game will now run.
 
# 3. License

```
MIT

Copyright 2019 Marcin Małysz

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
