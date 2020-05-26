# Developer Guide

By referring to this document, you can download the OpenArkCompiler source code to compile it. At the same time, this document also provide the guide of static code analysis to developers.

## Prerequisites

Prepare development environment by referring to Environment Configuration.


## Downloading Source Code

Download address: <https://code.opensource.huaweicloud.com/HarmonyOS/OpenArkCompiler/home> .
You can download the OpenArkCompiler source code in `Clone` or `Download` mode.
> Note: The download directory of the source code is openarkcompiler by default.


## Compiling Source Code


Run the following command in the openarkcompiler directory to compile OpenArkCompiler. The output path is openarkcompiler/output/bin by default.

```
source build/envsetup.sh
make
```
Command description:

- `source build/envsetup.sh`: Initialize the environment and add the toolchain path openarkcompiler/output/bin of OpenArkCompiler to environment variables.
- `make`: Compile the release version of OpenArkCompiler.
- `make BUILD_TYPE=DEBUG`: Compile the debug version of OpenArkCompiler.

In addition, the OpenArkCompiler also provides a shell script which contains the command to compile OpenArkCompiler. The developer can run the script in the openarkcompiler directory to compile OpenArkCompiler. The command to run the script:

```
source build/build.sh
```

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

## Static code analysis

This part will guide you to do the static code analysis by using the clang-tidy. After the code is changed, the static code analysis will check the coding specifications to improve the code quality.

Before the static code analysis, we need compiled the OpenArkCompiler. After that, using the code of src/maple_driver as the tested directory, run the following commands in the openarkcompiler directory:

```
cp output/compile_commands.json ./
./tools/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/share/clang/run-clang-tidy.py -clang-tidy-binary='./tools/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang-tidy' -clang-apply-replacements-binary='./tools/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang-apply-replacements' src/maple_driver/
```
Command description:

- `cp output/compile_commands.json ./`: Copy the compile_commands.json in the output directory to the openarkcompiler directory, it is required by the clang-tidy, it contains the compile commands of OpenArkCompiler.

- `./tools/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/share/clang/run-clang-tidy.py`: Call the run-clang-tidy.py which is the parallel clang-tidy runner. The `./tools/clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/` directory is the directory of the release package of clang compiler. The `-clang-tidy-binary` set the path of clang-tidy binary. The `-clang-apply-replacements-binary` set the path of the clang-apply-replacements binary which is requried by the run-clang-tidy.py. The `src/maple_driver/` is the tested code directory.
