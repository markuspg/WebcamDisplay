# WebcamDisplay

## Installation

First *WebcamDisplay* must be compiled using *Qt Creator*. Afterwards the created executable file can be placed anywhere. On *Linux* it is suggested to place it in */usr/local/bin*, on Windows it could be placed in *C://EcoLabLib*.

## Building and Running on Windows

Install a recent *MinGW* version of *Qt* on your computer. Afterwards open the project file of *WebcamDisplay* and build it in *Debug* mode (which is the default). *WebcamDisplay* should run just fine from *Qt Creator*, only complaining on every start, since the necessary webcam URL as argument is not passed. If you want to run it normally you will have to put the following files from the *Qt* installation directory (e.g. C:\Qt\Qt5.4.2\5.4\mingw491\_32\bin) into the same directory as the created executable:

icuin53.dll
icudt53.dll
icuuc53.dll
libgcc\_s\_dw2-1.dll
libstdc++-6.dll
libwinpthread-1.dll
Qt5Cored.dll
Qt5Guid.dll
Qt5Networkd.dll
Qt5Widgetsd.dll

The *d* at the end of the *Qt* dlls marks them as debug versions. If disk space and resources shall be saved, *WebcamDisplay* can be built in *Release* mode. In this case, use the *Qt* files without the attached *d* (e.g. *Qt5Core.dll* instead of *Qt5Cored.dll*).
