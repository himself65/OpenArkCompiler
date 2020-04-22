# FEIR设计
[TOC]

## 1. 类型系统
### 1.1. 类型定义
```cpp
class FEIRType;
class FEIRTypeDefault : public FEIRType;
class FEIRTypeByName : public FEIRTypeDefault;
class FEIRTypePointer : public FEIRType;
using UniqueFEIRType = std::unique_ptr<FEIRType>;
```

#### 1.1.1. FEIRType
特性:
* 类型系统基类 (虚基类, 不可创建对象)
* 定义接口函数

接口函数列表
```cpp
PrimType GetPrimType() const;
bool IsRef() const;
bool IsArray() const;
bool IsPrecise() const;
bool IsValid() const;
MIRType GenerateMIRType(bool usePtr, PrimType ptyPtr = PTY_ref) const;
MIRType GenerateMIRTypeAuto(MIRSrcLang srcLang) const;
```

#### 1.1.2. FEIRTypeDefault
特性:
* 默认类型
* 使用```PrimType primType```, ```GStrIdx typeNameIdx```和```uint8 dim```来描述一个类型
* 不可在并发场景下创建精确Ref类型和精确Ref数组类型 (需向```GlobalTable```写入数据, 在FE并行框架中是禁止的)

类型组合
| 类型 | primType | typeNameIdx | dim | IsRef() | IsArray() | IsPrecise() |
|-----|-----|-----|-----|-----|-----|-----|-----|
| 精确Scalar     | scalar  |   0 |   0 | false | false | true  |
| 精确Scalar数组 | scalar  |   0 | 非0 | true  | true  | true  |
| 模糊Ref        | PTY_ref |   0 |   0 | true  | false | false |
| 模糊Ref数组    | PTY_ref |   0 | 非0 | true  | true  | false |
| 精确Ref        | PTY_ref | 非0 |   0 | true  | false | true  |
| 精确Ref数组    | PTY_ref | 非0 | 非0 | true  | true  | true  |
| 无效           | scalar  | 非0 |   0 | -     | -     | -     |
| 无效           | scalar  | 非0 | 非0 | -     | -     | -     |

注:
* scalar包括```PrimitiveInteger```, ```PrimitiveFloat```, 和```PTY_void```, 排除```PTY_ref```

#### 1.1.3. FEIRTypeByName
特性:
* ```FEIRTypeDefault```的扩展类型
* 使用```std::string typeName```替换```GStrIdx typeNameIdx```
* 可以在并发场景下创建 (弥补```FEIRTypeDefault```)

### 1.2. 语言配置
#### 1.2.1. GenerateMIRTypeAuto()
| 语言 | usrPtr | primTypeForPtr |
|-----|-----|-----|
| kSrcLangJava | true | PTY_ref |

### 1.3. 使用方法
#### 1.3.1. 直接创建

#### 1.3.2. 使用FEIRTypeHelper


## 2. 变量系统

## 3. IR设计
