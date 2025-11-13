# Comet Channel Strip

### Overview

This project is an open-source channel strip audio plugin designed as an expandable foundation for
audio development. It will serve as the primary teaching medium in an ongoing YouTube tutorial
series focused on DSP, plugin design, UI/UX, and modern development workflows.

The plugin will grow continuously as new video topics are released, making it a lasting educational
tool for aspiring audio developers.

### Cloning the Project

If you intend to contribute to the project and open pull requests, navigate to
the [Github page](https://github.com/landonviator/CometChannelStrip) for the project and click the "
Fork" button to get
the .git link.

If you don't intend on contributing, click the green "Code" button to grab the .git link for the
project.

Open up your terminal of choice and navigate to a location on your machine you would like to put the
project. Once you're there, use this command to clone the repo:
<br><br>`git clone --recurse-submodules https://github.com/landonviator/CometChannelStrip.git`
<br><br> That's it, the project should populate with all submodules correctly and you're ready to
build.

### Building the Project

I recommend using the [CLion](https://www.jetbrains.com/clion) IDE from JetBrains as it integrates
perfectly with CMake, is free for non-commercial use, and cross-platform for Mac, Windows, and
Linux. Installing CLion should also install CMake, but it would be a good idea to install CMake explicitly before.

Release Build Commands:

```
cmake -B Build -D CMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64
cmake --build Build --config Release
```

### Install CMake

Mac: Download CMake [here](https://cmake.org/download/) and install.
<br> <br> Windows: Download CMake [here](https://cmake.org/download/) and install, making sure to choose “Add CMake to system
PATH” during install
<br> <br> Linux: run the following commands to install CMake:
`sudo apt update`
`sudo apt install cmake`

Open CLion and select File -> Open and navigate to the project folder containing the `modules` folder, `source`
folder, and `CMakeLists.txt` file; You should then see CLion pop-up a window for applying initial settings, so just
click "ok". Navigate to the File menu again and select "Reload CMake Project". That will initialize the project and
you should be ready to build the project, either with the build button (hammer icon in top right) or the play button
(switch the target to standalone just to the left of the build button). The project should build and you're ready to
go adding your own stuff or submitting contributions!

### Issues

If you have any issues, feel free to post questions in the [Discord](https://discord.gg/Rv5MEWgwwQ)
