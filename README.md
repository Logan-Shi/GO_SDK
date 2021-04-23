# GO_SDK
gocator support

# Source code

To install GoSDK dependency, the following steps are required: 

1. With a Serial Number of a device, register as a customer at the [LMI website](http://downloads.lmi3d.com/)
2. Download the SDK from the [LMI website](http://downloads.lmi3d.com/) (file 14400-4.2.5.17_SOFTWARE_GO_SDK.zip)
3. Uncompress GoSDK
4. To switch Build Type between "Debug" and "Release", you need to adjust three files in total. 

```
/GO_SDK/Platform/kApi/kApi-Linux_X64.mk
/GO_SDK/Gocator/GoSdk/GoSdk-Linux_X64.mk
/GO_SDK/Gocator/GoSdk/GoSdkExample-Linux_X64.mk
```
# Build  
```shell 
$ mkdir GO_SDK/bin/linux_x64
$ cd GO_SDK/Gocator
$ make -f GoSdk-Linux_X64.mk 
```
