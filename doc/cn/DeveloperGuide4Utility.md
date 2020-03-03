# Maple通用模块应用手册

# Cast

## `instance_of`与`safe_cast`

`maple`对于`C++`的使用，原则上需要禁用`RTTI`，即禁用`dynamic_cast`。而由于编译器系统的复杂性，完全通过类设计来破除父类向子类转换的情况，反而会使得对象关系变得更加复杂，得不偿失。所以`maple`代码实现中便有了众多如下设计：

```c++
SubClass *sub = nullptr;
if (base.Type() == SubType) {
  sub = static_cast<SubClass*>(base);
}
```

通过设计某属性字段来实现父类与子类之间的关系绑定，从而达到与`dynamic_cast`相同的效果。

但此种写法仍有一些不足，首先`SubType`与`SubClass`之间隐藏有静态绑定关系，这由设计者决定，却需要调用者将两者关系显化，这会产生强依赖；其次，并不是所有场景都如类型比较这样直观，复杂的场景对于调用者来说更容易出错；最后，散落在各地的转换，将会埋下散弹式修改的问题。所以我们设计出了`safe_cast`，由设计者注册转换关系，调用者只需以`dynamic_cast`的方式调用即可

### 注册方式

通过`REGISTER_SAFE_CAST`宏来达成注册，声明如下：

```c++
#define REGISTER_SAFE_CAST(type, condition)
```

其中`type`为子类类型(假设为`B`)，`condition`为匹配到`B`以及其子类所有场景的布尔表达式。示例如下：

```c++
class A;
class B : A;
class C : B;
REGISTER_SAFE_CAST(B, from.Kind() == B || from.Kind() == C);
REGISTER_SAFE_CAST(C, from.Kind() == C);
```

`from`为表达式传入的类型的形参名。

*注意：*

*- 注册同时持了子类向父类转换以及父类向子类转换。*

*- `condition`可以是任意的布尔表达式，但设计者应尽量使其符合继承关系，避免非继承关系的转换。*

*- 对于复杂的`condition`使用`kind ==`表示一棵继承关系树是不明智的，需考虑优化，如：组织`kind`的范围、使用特定位标记等方式来达成快速匹配。*

### 使用场景

1. 针对只做单个类型匹配的场景

```c++
SubClass *sub = safe_cast<SubClass>(base);
if (sub == nullptr) {
  // TODO
}
```

2. 针对多个类型匹配的场景

```c++
if (instanceof<SubClass1>(base)) {
  auto *sub = static_cast<SubClass1*>(base);
  // TODO
} else if (instanceof<SubClass2>(base)) {
  auto *sub = static_cast<SubClass2*>(base);
  // TODO
}
...
```

*注意：*

*- 对于`switch case`等已经正确识别类型的场景，使用`static_cast`即可。*

*- `safe_cast`返回值永远是指针，外部识别是否转换成功。*

*- 输入若为指针，`instance_of`与`safe_cast`都会做判空，所以`base`非空时优先传入引用*

# Container

## `Ptr`

`Ptr`模拟了原生指针的行为，但切除了对数组操作的支持。

其通过在构造、赋值的操作中校验数据，使得指针对象仅在构造时需验证满足某特征，而传递和使用时均无需再次验证该特征，降低重复检查的开销。

```c++
template<typename T>
using PtrCheckerType = void (*)(const T*);

template<typename T>
constexpr void CheckNothing(const T*) {}

template<typename T, PtrCheckerType<T> Check = CheckNothing<T>>
class Ptr;
```

如上所示，`Ptr`的默认校验行为不做任何检查，通过

```c++
template < typename T>
using XXXPtr = utils::Ptr<T, CheckXXX<T>>;
```

即可定义不同场景下的已经校验过的指针对象，统称安全指针。

*注：其由`safe_ptr`的诉求而扩展而来，但`safe_ptr`相对通用的`Ptr`场景更为复杂，因为其针对的是指针本身的合法性而非指针对象的特征。*

## `safe_ptr`

参见《CPPCodingTalkAboutPointer》中`Ref`的定义。由于`c++`中`operator.`无法进行重载，所以无法构建`Ref`对象，但可以定义与其等效的非空指针对象，即`safe_ptr`。

### 场景：数组、字典等容器成员

示例可见于《CPPCodingTalkAboutPointer》。

为了弥补`operator.`无法重载的问题，后续将扩展`Utility`中`ToRef`小工具的能力，保证指针无开销但安全转换为引用。其可能特化如下：

```c++
template <typename T>
constexpr T &ToRef(safe_ptr<T> ptr) {
  return *ptr;
}
```

*注：对于使用频繁的容器，后续将封装`ref_xxx`系列的容器以取代`safe_ptr`这类使用场景。*

### 场景：作为对象成员

此为`safe_ptr`开发出来后意外发掘的场景。

由于存在引用数据成员的类，编译器无法为其生成默认拷贝和转移，但很多场景下需要支持拷贝和转移能力，此时通常的做法是存储为指针成员。但指针成员将带来两个问题：

1. 针对指针成员访问时需要去识别指针的合法性。
2. 重构和演进都有可能从定义成员的角色方来改变其行为可空，而使用成员的角色方不一定会被告知，这可能会导致进一步的隐藏bug。

而使用`safe_ptr`代替裸指针，`safe_ptr`解引用处都使用`ToRef`来转引用(裸指针的`ToRef`有额外的开销)，将会避免这样的问题。

*注意：`safe_ptr`设计为容器或对象成员，而不应用于函数传参，函数传参应使用`&`。*

## `mpl_iterator`

`mpl_iterator`原名为`iterator`，但由于和容器中`using iterator`重名，所以添加`mpl`前缀。

由于`ref_xxx`容器的设计，以及未来可能扩展small talk系列，重复的迭代器设计是个比较麻烦的问题，所以抽象出持续演进的统一迭代器容器，通过`mpl_iterator_traits`进行少量配置而快速实现新容器的迭代器。

当前基于`ref_vector`的迭代器诉求，设计`mpl_iterator_traits`的成员如下：

```c++
template <typename Iterator, typename Container>
struct mpl_iterator_traits {
  using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using difference_type = typename std::iterator_traits<Iterator>::difference_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;

  static reference operator_dereference(Iterator iter) {
    return *iter;
  };

  static Iterator operator_arrow(Iterator iter) {
    return iter;
  }

  static reference operator_bracket(Iterator iter, difference_type n) {
    return iter[n];
  }
};

```

## `ref_vector`

参见《CPPCodingTalkAboutPointer》中`ref_vector`的定义。

使用指针数组：`std::vector<T*>`

```c++
  int a = 0, b = 1;

  std::vector<int*> data;
  data.push_back(&a);
  data.push_back(&b);
  ASSERT_EQ(*data[0], 0);
  ASSERT_EQ(*data[1], 1);
```

重构为引用数组：`utils::ref_vector<T>`
```c++
  int a = 0, b = 1;

  ref_vector<int> data;
  data.push_back(a);
  data.push_back(b);
  ASSERT_EQ(data[0], 0);
  ASSERT_EQ(data[1], 1);
```

## `Index`

`Index`的设计初衷为满足基础类型的静态安全。

如`GStrIdx`、`UStrIdx`、`U16StrIdx`三者在业务代码中会共同参与计算，包括其提供的接口也有比较高的相似性。它们底层均为`uint32`，若均定义为`uint32`那将是个灾难，调用者不得不得小心翼翼，但也很难避免传错数据。所以使静态类型互斥，由编译器来检查使用的正确性，将大大降低出错的几率。

定义一个静态类型方式非常简单，只要定义不同的Tag即可，如下：

```c++
class GStrTag;
using GStrIdx = utils::Index<GStrTag, uint32>;

class UStrTag;
using UStrIdx = utils::Index<UStrTag, uint32>;

class U16StrTag;
using U16StrIdx = utils::Index<U16StrTag, uint32>; 
```

# Generalize Pattern

## `ObjectFactory`

`ObjectFactory`为针对抽象工厂的封装，用于解决以下问题：

1. 消除代码中由`switch ... case`、`if ... else if ...`等构成的具有高圈复杂度的大函数，这类函数难以维护和扩展，且灵活度低。*(注意：应避免机械的性解决问题，需分析语义做好设计，如配合模板模式等，实现设计隔离而非仅仅代码隔离。)*

2. 采用工厂将依赖反转，更容易将调用者和设计者、设计者和设计者之间隔离。

其应用方式如下示例：

```c++
// 定义Key，用于标记工厂将创建的产品类别
enum class ObjectType {
  kPlus,
  kMinus
};

// 定义接口，所有产品应实现的接口协议
class Base {
 public:
  virtual ~Base() = default;
  virtual int32_t DoIt(int32_t lhs, int32_t rhs) const = 0;
};

// 定义工厂，使用ObjectType作为查找关键字，Base为接口协议，int32_t为所有产品构造函数的参数类型
using TestObjectFactory = ObjectFactory<ObjectType, Base, int32_t>;

// Key，接口，工厂需要对注册者和调用者均可见，所以可能在.h文件中，可能在.cpp文件汇总
// 产品只要保证能注册入工厂，可以在不同的.cpp文件中
// 定义产品
class ObjectPlus : public Base {
 public:
  // 定义工厂时所约定的构造函数
  explicit ObjectPlus(int32_t base) : base(base) {}
  virtual ~ObjectPlus() = default;

  // 定义接口时的约定
  virtual int32_t DoIt(int32_t lhs, int32_t rhs) const override {
    return base + lhs + rhs;
  }

 private:
  int32_t base;
};

// 定义产品，可能在另外的.cpp文件中
class ObjectMinus : public Base {
 public:
  explicit ObjectMinus(int32_t base) : base(base) {}
  virtual ~ObjectMinus() = default;

  virtual int32_t DoIt(int32_t lhs, int32_t rhs) const override {
    return base + lhs - rhs;
  }

 private:
  int32_t base;
};

// 注册产品，注册产品的方式随着Key，接口，工厂，产品之间的分布以及加载可灵活处理，保证注册成功即可
// 此处利用static变量初始化来确保注册
bool RegisterObject() {
  RegisterFactoryObject<TestObjectFactory, ObjectType::kPlus, ObjectPlus>();
  RegisterFactoryObject<TestObjectFactory, ObjectType::kMinus, ObjectMinus>();
}
static auto testObjectFactory = RegisterObject();

TEST(TestFactory, ObjectFactory) {
  // 获取产品对象，需要判空，本示例略
  auto obj = CreateProductObject<TestObjectFactory>(ObjectType::kPlus, 10);
  ASSERT_EQ(obj->DoIt(1, 2), 13);
  obj = CreateProductObject<TestObjectFactory>(ObjectType::kMinus, 10);
  ASSERT_EQ(obj->DoIt(1, 2), 9);
}
```

## `FunctionFactory`

`FunctionFactory`解决的问题与`ObjectFactory`类似，其主要为了简化抽象工厂的复杂性，对于大多数场景，`FunctionFactory`更容易编写和上手。

其应用方式如下示例：

```c++
// 定义Key，用于标记工厂将创建的产品类别
enum class FunctionType {
  kPlus,
  kMinus
};

// 定义接口和工厂，使用FunctionType作为查找关键字，int32_t(int32_t, int32_t)为函数协议
using TestFunctionFactory = FunctionFactory<FunctionType, int32_t, int32_t, int32_t>;

// 定义产品
int32_t Plus(int32_t lhs, int32_t rhs) {
  return lhs + rhs;
}

// 定义产品，可能在另外的.cpp文件中
int32_t Minus(int32_t lhs, int32_t rhs) {
  return lhs - rhs;
}

// 注册产品，注册产品的方式随着Key，接口，工厂，产品之间的分布以及加载可灵活处理，保证注册成功即可
// 此处利用单件模式来确保注册
bool RegisterFunction() {
  RegisterFactoryFunction<TestFunctionFactory>(FunctionType::kPlus, Plus);
  RegisterFactoryFunction<TestFunctionFactory>(FunctionType::kMinus, Minus);
}
void AutoFunctionLoader() {
  static auto testObjectFactor = RegisterFunction();
}

TEST(TestFactory, TestAll) {
  // 加载产品
  AutoFunctionLoader();

  // 获取产品对象，需要判空，本示例略
  auto func = CreateProductFunction<TestFunctionFactory>(FunctionType::kPlus);
  ASSERT_EQ(func(1, 2), 3);
  func = CreateProductFunction<TestFunctionFactory>(FunctionType::kMinus);
  ASSERT_EQ(func(1, 2), -1);
}
```


# Utility

## `ToRef`

针对方舟新增代码和重构代码中指针传参的场景，期望以引用的方式来替代，即确保所有指针均已经过校验，再以引用的方式传递给被调用的函数，被调用函数多数情况下不应承担函数参数中指针为空的风险以及判断的开销。

通常的写法为(示例中`DoIt`和`Run`可当做第三方接口，无法更改)：

```c++
A *DoIt(B &b);
void Run(B *b) {
  CHECK_NULL_FATAL(b);
  // ...
  A *a = DoIt(*b);
  CHECK_NULL_FATAL(a);
  a->Do;
}
```

期望多数指针获取即引用：

```c++
A *DoIt(B &b);
void Run(B *b) {
  B &bRef = utils::ToRef(b);
  // ...
  A &a = utils::ToRef(DoIt(bRef));
  a.Do;
}
```

对于`b`仅单次使用，亦可以调整为：

```c++
A *DoIt(B &b);
void Run(B *b) {
  // ...
  A &a = utils::ToRef(DoIt(utils::ToRef(b)));
  a.Do;
}
```

## `bit_field_v`&`lbit_field_v`

使用bit位来标记状态开关组合，是一种既节约内存又能高效编码的设计方法。通常，在枚举定义或常量定义时，会有如下写法：

```c++
enum BBAttr : uint32 {
  kBBAttrIsEntry = 0x02,
  kBBAttrIsExit = 0x04,
  kBBAttrWontExit = 0x08,
  kBBAttrIsTry = 0x10,
  kBBAttrIsTryEnd = 0x20,
  kBBAttrIsJSCatch = 0x40,
  kBBAttrIsJSFinally = 0x80,
  kBBAttrIsCatch = 0x0100,
  kBBAttrIsJavaFinally = 0x0200,
  kBBAttrArtificial = 0x0400,
  kBBAttrIsInLoop = 0x0800,
  kBBAttrIsInLoopForEA = 0x1000
};
```

此设计很明显的欲用位来记录某些属性信息，但位信息比较隐晦，难以维护与阅读。

所以需要更加清晰的设计：

```c++
enum BBAttr : uint32 {
  kBBAttrIsEntry       	= utils::bit_field_v<1>,
  kBBAttrIsExit 		= utils::bit_field_v<2>,
  kBBAttrWontExit 		= utils::bit_field_v<3>,
  kBBAttrIsTry 			= utils::bit_field_v<4>,
  kBBAttrIsTryEnd 		= utils::bit_field_v<5>,
  kBBAttrIsJSCatch 		= utils::bit_field_v<6>,
  kBBAttrIsJSFinally 	= utils::bit_field_v<7>,
  kBBAttrIsCatch 		= utils::bit_field_v<8>,
  kBBAttrIsJavaFinally 	= utils::bit_field_v<9>,
  kBBAttrArtificial 	= utils::bit_field_v<10>,
  kBBAttrIsInLoop 		= utils::bit_field_v<11>,
  kBBAttrIsInLoopForEA 	= utils::bit_field_v<12>
};
```

其中`bit_field_v`:`uint32`，`lbit_field_v`:`uint64`，未来将按需添加`sbit_field_v`:`uint16`以及`bbit_field_v`:`uint8`。
