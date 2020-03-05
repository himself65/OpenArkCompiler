## Environment Configuration

## Recommended Hardware:

- 2 GHz dual-core processor or higher

- 2 GB system memory or higher

- 200 GB available disk space

## Recommended Development Environment

Install a 64-bit Ubuntu (Ubuntu 16.04 is recommended).


```
sudo apt-get -y install openjdk-8-jdk git-core build-essential zlib1g-dev libc6-dev-i386 g++-multilib gcc-multilib linux-libc-dev:i386
sudo apt-get -y install gcc-5-aarch64-linux-gnu g++-5-aarch64-linux-gnu
```


## Installing and Configuring Clang (for Compiling the OpenArkCompiler Code)

Download **clang+llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04**
LLVM download address: http://releases.llvm.org/download.html#8.0.0

Place the downloaded files in the `openarkcompiler/tools` directory, open the `openarkcompiler/build/config.gni` file, and set the three variables `GN_C_COMPILER`, `GN_CXX_COMPILER`, and `GN_AR_COMPILER` to the path where Clang is located. For example:

```
GN_C_COMPILER = "${MAPLE_ROOT}/tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang"
GN_CXX_COMPILER = "${MAPLE_ROOT}/tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang++"
GN_AR_COMPILER = "${MAPLE_ROOT}/tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/llvm-ar"
```

${MAPLE_ROOT} is the root directory of the OpenArkCompiler source code.

## Installing and configuring Ninja and GN

Download **Ninja(v1.9.0)** and **GN(Linux Version)**
Ninja download address: https://github.com/ninja-build/ninja/releases
GN download address: https://gitee.com/xlnb/gn_binary

Place the executable programs of GN and Ninja in the openarkcompiler/tools directory, modify these two files to be executable.

```
cd openarkcompiler/tools
chmod 775 gn
chmod 775 ninja
```

Open the openarkcompiler/Makefile file, and set the two variables GN and NINJA to the path where the executable programs of GN and Ninja are located. For example,

```
GN := ${MAPLE_ROOT}/tools/gn/gn
NINJA := ${MAPLE_ROOT}/tools/ninja_1.9.0/ninja
```

