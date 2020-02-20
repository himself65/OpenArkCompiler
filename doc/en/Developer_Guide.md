# Developer Guide

By referring to this document, you can download the OpenArkCompiler source code to compile it.

## Prerequisites

Prepare development environment by referring to Environment Configuration.


## Downloading Source Code

Download address: <https://code.opensource.huaweicloud.com/HarmonyOS/OpenArkCompiler/home> .
You can download the OpenArkCompiler source code in `Clone` or `Download` mode.
> Note: The download directory of the source code is openarkcompiler by default.


## Compiling Source Code


Run the following command in the openarkcompiler directory to compile OpenArkCompiler. The output path is openarkcompiler/out/bin by default.

```
source build/envsetup.sh
make
```
Command description:

- `source build/envsetup.sh`: Initialize the environment and add the toolchain path openarkcompiler/out/bin of OpenArkCompiler to environment variables.
- `make`: Compile the release version of OpenArkCompiler.
- `make BUILD_TYPE=DEBUG`: Compile the debug version of OpenArkCompiler.

## Compiling Sample

The Java basic library is required for OpenArkCompiler to compile a sample. The following uses the Java basic library provided by the Android OS to describe the sample compilation process.

**Preparing basic library**

- Download the Android code and compile it locally to obtain the libcore JAR package. The Android 9.0.0_r45 version is recommended.

- Gitee also provides the compiled libcore JAR file. Download address: https://gitee.com/mirrors/java-core/ .

**Generating the libjava-core.mplt file**

Before compilation, create the libjava-core directory in the openarkcompiler directory, copy the java-core.jar file to the libjava-core directory, and run the following commands in the openarkcompiler directory:

```
source build/envsetup.sh
make
cd libjava-core
jbc2mpl -injar java-core.jar -out libjava-core
```

After the execution is complete, the libjava-core.mplt file is generated in the directory.

**Compiling sample code**

The sample code is in the openarkcompiler/samples directory.

Take the samples/helloworld/ code as an example. Run the following command in the openarkcompiler/ directory:

```
source build/envsetup.sh
make
cd samples/helloworld/
make
```
