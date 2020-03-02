# 开发者指南

通过参考本文档，您可以下载编译器源码编译出OpenArkCompiler。同时，本文档也为开发者提供了源码静态检查指南。

## 前提条件

请先按照《环境配置》文档完成您的开发环境准备。


## 源码下载

   下载地址：<https://code.opensource.huaweicloud.com/HarmonyOS/OpenArkCompiler/home>，可以通过`Clone` or `Download`的方式下载openarkcompiler源码。
   > 注：默认源码下载目录为openarkcompiler。


## 源码编译


在openarkcompiler目录下执行以下命令，编译出OpenArkCompiler，默认输出路径 openarkcompiler/output/bin。

```
source build/envsetup.sh
make
```
命令说明：

- `source build/envsetup.sh` 初始化环境，将OpenArkCompiler工具链路径openarkcompiler/output/bin设置到环境变量中；
- `make` 编译OpenArkCompiler的Release版本；
- `make BUILD_TYPE=DEBUG` 编译OpenArkCompiler的Debug版本。

此外，方舟编译器还提供了源码编译脚本，开发者也可以通过在openarkcompiler目录下执行该脚本，编译出OpenArkCompiler。执行命令如下：

```
source build/build.sh
```

## Sample示例编译

当前编译方舟编译器Sample应用需要使用到Java基础库，我们以Android系统提供的Java基础库为例，展示Sample样例的编译过程。

**基础库准备**

- 您可以自己下载Android代码本地编译来获得libcore的jar包，建议使用Android的9.0.0_r45版本；

- 同时码云上也提供了已经编译好的libcore的jar文件，你可以下载直接使用，下载链接：https://gitee.com/mirrors/java-core/ 。

**生成libjava-core.mplt文件**

编译前，请先在openarkcompiler目录下创建libjava-core目录，拷贝java-core.jar到此目录下，在openarkcompiler目录执行以下命令：

```
source build/envsetup.sh
make
cd libjava-core
jbc2mpl -injar java-core.jar -out libjava-core
```

执行完成后会在此目录下生成libjava-core.mplt文件。

**示例代码快速编译**

示例代码位于openarkcompiler/samples目录。

以samples/helloworld/代码为例，在openarkcompiler/目录下执行以下命令：

```
source build/envsetup.sh
make
cd samples/helloworld/
make
```

## 源码静态检查

本部分内容将指导您使用clang-tidy进行源码静态检查。在对源码进行修改之后，对源码进行静态检查，可以检查源码是否符合编程规范，有效的提高代码质量。

静态源码检查之前，需要先编译出OpenArkCompiler。此后，以检查src/maple_driver源码为例，在openarkcompiler目录下执行以下命令：

```
cp output/compile_commands.json ./
./tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/share/clang/run-clang-tidy.py -clang-tidy-binary='./tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang-tidy' -clang-apply-replacements-binary='./tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/bin/clang-apply-replacements' src/maple_driver/
```
命令说明：

- `cp output/compile_commands.json ./` 将output目录之下的compile_commands.json复制到当前目录之下，它是clang-tidy运行所需要的编译命令；

- `./tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/share/clang/run-clang-tidy.py` 调用clang-tidy进行批量检查的脚本run-clang-tidy.py，其中 `./tools/clang_llvm-8.0.0-x86_64-linux-gnu-ubuntu-16.04/`目录是之前配置的clang编译器的发行包主目录； `-clang-tidy-binary` 是指明clang-tidy的具体位置； `-clang-apply-replacements-binary` 是指明run-clang-tidy.py所依赖的clang-apply-replacements的位置； `src/maple_driver/` 是要进行源码检查的目录。
