# 开发者指南

通过参考本文档，您可以下载编译器源码编译出OpenArkCompiler。

## 前提条件

请先按照《环境配置》文档完成您的开发环境准备。


## 源码下载

   下载地址：<https://code.opensource.huaweicloud.com/HarmonyOS/OpenArkCompiler/home>，可以通过`Clone` or `Download`的方式下载openarkcompiler源码
   > 注：默认源码下载目录为openarkcompiler


## 源码编译


在openarkcompiler目录下执行以下命令，编译出OpenArkCompiler，默认输出路径 openarkcompiler/out/bin。

```
source build/envsetup.sh
make
```
命令说明：

- `source build/envsetup.sh` 初始化环境，将OpenArkCompiler工具链路径openarkcompiler/src/bin设置到环境变量中
- `make` 编译OpenArkCompiler的Release版本
- `make BUILD_TYPE=DEBUG` 编译OpenArkCompiler的Debug版本
