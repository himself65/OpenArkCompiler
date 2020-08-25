# OpenArkCompiler

## Overview
-----------------
> Unified programming platform supporting multiple devices and languages

OpenArkCompiler is Huawei's open source project.

### Four Technical Features of OpenArkCompiler ###

Compiles code of different languages into a set of executable files and efficiently executes the files in the running environment.
- Supports joint optimization of multiple languages and eliminates cross-language calling overhead.
- Achieves lightweight language runtime.
- Collaborates hardware and software to maximize hardware energy efficiency.
- Supports diversified terminal platforms.

## Open Source Plan
**Open source compiler framework code**
- Time: August, 2019
- Scope: Compiler IR and middle-end language implementation
- Benefits:
   - Provides an open source framework for understanding the architecture and framework code of OpenArkCompiler.
   - Allows developers to build a complete compiler toolchain that supports the compilation of Java samples (non-application).

**Subsequent open source scope**
Open the front end, back end, and compilation optimization of compilers. Support the full compilation of Java programs and JavaScript applications.

|Open Source Scope|March 2020 |May 2020 |
| ------------ | -------------------|--------------------- |
|Front end| Jbc front-end basic framework | Front-end full open source |
|Middle end |Weekly open source optimization phase |       |
|Back end |Backend open source (O0) (aarch64)|Weekly open source optimization (O2) (aarch64)|
|Test framework|Test framework and basic Cases|     |

**Updating...**

## OpenArkCompiler Incubator
- FutureWei Mapleall https://gitee.com/openarkcompiler-incubator/mapleall
- Maple Engine https://gitee.com/openarkcompiler-incubator/maple_engine

## Related Documents

- Architecture Design Principles
   - [MAPLE IR Design](doc/en/MapleIRDesign.md)
   - [RC API](doc/en/RcApi.md)
   - [Naive RC Insertion Principle](doc/en/NaiveRcInsertionDescription.md)
   - [Virtual Table and Interface Table Design](doc/en/VtableItableDescription.md)
   - [Phase Design](doc/en/CompilerPhaseDescription.md)
   - [Application Manual of Maple General Modules](doc/en/DeveloperGuide4Utility.md)


- [Environment Configuration](doc/en/DevelopmentPreparation.md)

- [Developer Guide](doc/en/DeveloperGuide.md)

- [Programming Specifications](doc/en/ProgrammingSpecifications.md)

- [Discussion of Pointer in C++ Programming](doc/en/CPPCodingTalkAboutPointer.md)

## License
- [LICENSE](license/LICENSE)
