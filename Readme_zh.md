# OpenArkCompiler

## 概述
-----------------
> 面向多设备、支持多语言的统一编程平台。

OpenArkCompiler是来自华为方舟编译器的开源项目。

### OpenArkCompiler 四个技术特点 ###

能够将不同语言代码编译成一套可执行文件，在运行环境中高效执行：
- 支持多语言联合优化、消除跨语言调用开销；
- 更轻量的语言运行时；
- 软硬协同充分发挥硬件能效；
- 支持多样化的终端设备平台

## 开源计划
**编译器框架代码开源**
- 时间：2019年8月
- 开源范围：编译器IR+中端语言实现
- 开放能力：
   - 框架开源供参考学习，了解方舟编译器架构及框架代码
   - 开发者可构建出完整编译器工具链，支持Java Sample程序编译（非应用）

**后续开源范围**
陆续开源编译器前端、后端、编译优化；完整支持Java程序编译、JavaScript语言应用的编译等。

|开源范围          |2020年3月       |2020年5月             |
| ------------ | -------------------|--------------------- |
|编译器前端    |jbc前端基础框架     |前端全量开源          |
|编译器中端    |独立优化Phase每周持续开源                  ||
|编译器后端    |后端开源(O0) (aarch64)|独立优化按周开源(O2) (aarch64)|
|测试框架      |测试框架+基础用例开源| |

**计划持续更新...**

## 孵化器项目
- FutureWei编译器分支 https://gitee.com/openarkcompiler-incubator/mapleall
- Maple Engine https://gitee.com/openarkcompiler-incubator/maple_engine

## 相关文档

- 架构设计原理
   - [MAPLE IR Design](doc/en/MapleIRDesign.md)
   - [RC API](doc/cn/RcApi.md)
   - [Naive RC操作插入原理](doc/cn/NaiveRcInsertionDescription.md)
   - [虚函数表和接口函数表设计介绍](doc/cn/VtableItableDescription.md)
   - [Phase设计介绍](doc/cn/CompilerPhaseDescription.md)
   - [Maple通用模块应用手册](doc/cn/DeveloperGuide4Utility.md)

- [环境配置](doc/cn/DevelopmentPreparation.md)

- [开发者指南](doc/cn/DeveloperGuide.md)

- [编程规范](doc/cn/ProgrammingSpecifications.md)

- [C++编程探讨之指针](doc/cn/CPPCodingTalkAboutPointer.md)

## 许可证
- [LICENSE](license/LICENSE)
