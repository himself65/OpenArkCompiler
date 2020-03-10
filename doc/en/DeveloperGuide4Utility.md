# Application Manual of Maple General Modules

# Cast

## `instance_of` and `safe_cast`.

In principle, `RTTI`, that is, `dynamic_cast`, must be disabled for the use of `C++` in `maple`. The compiler system is complex. It will make the object relationship more complex to cast from a class object to a subclass object by class design. Therefore, the `maple` code implementation introduces the following designs:

```c++
SubClass *sub = nullptr;
if (base.Type() == SubType) {
  sub = static_cast<SubClass*>(base);
}
```

A property field is designed to implement the binding between a class and its subclass, achieving the same effect as `dynamic_cast`.

However, this method has some disadvantages. First, whereas the binding relationship between `SubType` and `SubClass` is  static and determined by the designer, the caller needs to make the relationship explicit, resulting in strong dependency. Second, not all scenarios are as intuitive as type comparison. Callers are prone to make mistakes in complex scenarios, causing shotgun surgery in later code rectification. Therefore, the `safe_cast` is designed. The designer registers the casting relationship. A caller only needs to call the `dynamic_cast` method.

### Registration method.

Use the `REGISTER_SAFE_CAST` macro to complete the registration. The declaration is as follows:

```c++
#define REGISTER_SAFE_CAST(type, condition)
```

`type` is a subclass type (assumed as `B`), and `condition` is a Boolean expression that matches `B` and all its subclasses. Example:

```c++
class A;
class B : A;
class C : B;
REGISTER_SAFE_CAST(B, from.Kind() == B || from.Kind() == C);
REGISTER_SAFE_CAST(C, from.Kind() == C);
```

`from` is a formal parameter name of a type transferred by an expression.

*Note:*

*- The registration supports the casting from a subclass to a class and from a class to a subclass.*

*- `condition` can be any Boolean expression. However, the designer must ensure that it complies with the inheritance relationship to avoid casting of non-inheritance relationships.*

*- For a complex `condition`, using `kind ==` to indicate an inheritance relationship tree is not recommended. Optimization needs to be considered. For example, the range of `kind` needs to be organized, and a specific bit flag needs to be used to achieve quick matching.*

#### Application scenarios.

1. For scenarios where only one type is matched:

```c++
SubClass *sub = safe_cast<SubClass>(base);
if (sub == nullptr) {
  // TODO
}
```

2. For scenarios where multiple types are matched:

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

*Note:*

*- In scenarios where types have been correctly identified, such as `switch case`, use `static_cast`.*

*- The return value of `safe_cast` is always a pointer, indicating whether the casting is successful.*

*- If the input is a pointer, both `instance_of` and `safe_cast` check whether the input is null. Therefore, if `base` is not null, the reference is preferentially transferred.*

# Container

## `Ptr`

`Ptr` simulates the behavior of a native pointer, but removes support for array operations.

In this method, data is validated in the construction and assignment operations, so that a feature of a pointer object needs to be validated only during construction, and does not need to be validated again during transfer and use, thereby reducing overheads of repeated check.

```c++
template<typename T>
using PtrCheckerType = void (*)(const T*);

template<typename T>
constexpr void CheckNothing(const T*) {}

template<typename T, PtrCheckerType<T> Check = CheckNothing<T>>
class Ptr;
```

As shown in the preceding example, the default validation behavior of `Ptr` is not checked.

```c++
template < typename T>
using XXXPtr = utils::Ptr<T, CheckXXX<T>>;
```

In the preceding example, the validated pointer objects in different scenarios can be defined, which are called security pointers.

*Note: The preceding behavior is extended based on the requirements of `safe_ptr`. However, `safe_ptr` is more complex than the common `Ptr` scenario because it is specific to the validity of a pointer rather than features of a pointer object.*

## `safe_ptr`

For details, see the definition of `Ref` in the CPPCodingTalkAboutPointer. The `operator.` in `c++` cannot be overloaded. As a result, the `Ref` object cannot be built. However, the `safe_ptr` object, which is a non-null pointer object equivalent to the `Ref` object, can be defined.

### Scenario: Container members such as arrays and dictionaries.

For details, see the CPPCodingTalkAboutPointer.

To solve the problem that `operator.` cannot be overloaded, the `ToRef` tool capability in `Utility` will be extended, ensuring that the pointer does not have overhead and is securely casted into a reference. It may have the following features:

```c++
template <typename T>
constexpr T &ToRef(safe_ptr<T> ptr) {
  return *ptr;
}
```

*Note: For containers that are frequently used, containers of the `ref_xxx` series will be encapsulated to replace the `safe_ptr` application scenarios.*

### Scenario: Object members.

This is an unexpected scenario discovered after `safe_ptr` is developed.

The compiler cannot generate default copies and transfers for classes of reference data members. However, the copy and transfer capabilities are required in many scenarios. In this case, the common method is to store the classes as pointer members. However, pointer members bring the following problems:

1. When a pointer member is accessed, the validity of the pointer needs to be identified.
2. The behavior can be controllable by defining member roles during refactoring and evolution. However, the role that uses the member may not be notified, which may further cause hidden bugs.

If `safe_ptr` is used to replace a raw pointer and `ToRef` is used to convert the reference at the `safe_ptr` dereference position (`ToRef` of a raw pointer has extra overheads), the preceding problems can be avoided.

*Note: `safe_ptr` is designed as a container or object member. `&` is used for transferring function parameters.*

## `mpl_iterator`

The original name of `mpl_iterator` is `iterator`, but the name is the same as that of `using iterator` in the container. Therefore, the `mpl` prefix is added.

Taking the design of the `ref_xxx` container and possible expansion of the small talk series in the future into consideration, repeated iterator design is troublesome. Therefore, a unified iterator container that continuously evolves is abstracted, which uses `mpl_iterator_traits` to quickly implement the iterator of the new container with a few configurations.

Based on the iterator requirements of `ref_vector`, members of `mpl_iterator_traits` are designed as follows:

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

For details, see the definition of `ref_vector` in the CPPCodingTalkAboutPointer.

Use the pointer array: `std::vector<T*>`.

```c++
  int a = 0, b = 1;

  std::vector<int*> data;
  data.push_back(&a);
  data.push_back(&b);
  ASSERT_EQ(*data[0], 0);
  ASSERT_EQ(*data[1], 1);
```

Refactored as a reference array: `utils::ref_vector<T>`.
```c++
  int a = 0, b = 1;

  ref_vector<int> data;
  data.push_back(a);
  data.push_back(b);
  ASSERT_EQ(data[0], 0);
  ASSERT_EQ(data[1], 1);
```

## `Index`

`Index` is designed to meet the static security requirements of basic types.

For example, `GStrIdx`, `UStrIdx`, and `U16StrIdx` are involved in calculation in service code, and their interfaces are similar. Their bottom layers are `uint32`. If all formal parameters are defined as `uint32`, it will be a disaster. The caller must be careful, but it is difficult to avoid transferring incorrect data. So static type conflict is used and the compiler checks the correctness of the use, which will greatly reduce the errors.

To define a static type, you only need to define different tags, as shown in the following example:

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

The `ObjectFactory` is an encapsulation for an abstract factory and is used to solve the following problems:

1. Eliminate large functions with high cyclomatic complexity in code, such as `switch... case` and `if... else if...`. These functions are difficult to maintain and extend and have low flexibility.* (Note: Analyze semantics and design well instead of rigidly solving problems, for example, use template mode to implement design isolation instead of code isolation.)*

2. The use of a factory will invert the dependency, making it easier to isolate the caller from the designer and the designer from each other.

Example:

```c++
// Defines a key to mark the product type to be created by the factory.
enum class ObjectType {
  kPlus,
  kMinus
};

//Defines interfaces and interface protocols that all products must implement.
class Base {
 public:
  virtual ~Base() = default;
  virtual int32_t DoIt(int32_t lhs, int32_t rhs) const = 0;
};

// Defines a factory by using ObjectType as the search keyword. Base indicates the interface protocol, and int32_t indicates the parameter type of all product constructor functions.
using TestObjectFactory = ObjectFactory<ObjectType, Base, int32_t>;

// Key, interface, and factory must be visible to both the registrant and caller. Therefore, the key may be in the .h file or summarized in the .cpp file.
// Products can be registered in different .cpp files as long as they can be registered in the factory.
// Defines a product.
class ObjectPlus : public Base {
 public:
  // Constructor function agreed upon during factory definition.
  explicit ObjectPlus(int32_t base) : base(base) {}
  virtual ~ObjectPlus() = default;

  // Conventions for defining interfaces.
  virtual int32_t DoIt(int32_t lhs, int32_t rhs) const override {
    return base + lhs + rhs;
  }

 private:
  int32_t base;
};

// Defines the product, which may be in another .cpp file.
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

// Registers a product. The product registration mode varies depending on the distribution and loading of keys, interfaces, factories, and products. Ensure that the registration is successful.
// Initializes the static variable to ensure registration.
bool RegisterObject() {
  RegisterFactoryObject<TestObjectFactory, ObjectType::kPlus, ObjectPlus>();
  RegisterFactoryObject<TestObjectFactory, ObjectType::kMinus, ObjectMinus>();
}
static auto testObjectFactory = RegisterObject();

TEST(TestFactory, ObjectFactory) {
  // Obtains a product object. Check whether the value is null. The example is omitted.
  auto obj = CreateProductObject<TestObjectFactory>(ObjectType::kPlus, 10);
  ASSERT_EQ(obj->DoIt(1, 2), 13);
  obj = CreateProductObject<TestObjectFactory>(ObjectType::kMinus, 10);
  ASSERT_EQ(obj->DoIt(1, 2), 9);
}
```

## `FunctionFactory`

Similar to `ObjectFactory`, `FunctionFactory` is used to simplify the complexity of abstract factories. In most scenarios, `FunctionFactory` is easier to compile and use.

Example:

```c++
// Defines a key to mark the product type to be created by the factory.
enum class FunctionType {
  kPlus,
  kMinus
};

// Defines the interface and factory by using the FunctionType as the search keyword and the int32_t(int32_t, int32_t) as the function protocol.
using TestFunctionFactory = FunctionFactory<FunctionType, int32_t, int32_t, int32_t>;

// Defines a product.
int32_t Plus(int32_t lhs, int32_t rhs) {
  return lhs + rhs;
}

// Defines the product, which may be in another .cpp file.
int32_t Minus(int32_t lhs, int32_t rhs) {
  return lhs - rhs;
}

// Registers a product. The product registration mode varies depending on the distribution and loading of keys, interfaces, factories, and products. Ensure that the registration is successful.
// Uses the singleton pattern to ensure registration.
bool RegisterFunction() {
  RegisterFactoryFunction<TestFunctionFactory>(FunctionType::kPlus, Plus);
  RegisterFactoryFunction<TestFunctionFactory>(FunctionType::kMinus, Minus);
}
void AutoFunctionLoader() {
  static auto testObjectFactor = RegisterFunction();
}

TEST(TestFactory, TestAll) {
  // Loads the product.
  AutoFunctionLoader();

  // Obtains a product object. Check whether the value is null. The example is omitted.
  auto func = CreateProductFunction<TestFunctionFactory>(FunctionType::kPlus);
  ASSERT_EQ(func(1, 2), 3);
  func = CreateProductFunction<TestFunctionFactory>(FunctionType::kMinus);
  ASSERT_EQ(func(1, 2), -1);
}
```


# Utility

## `ToRef`

In the scenario where pointers transfer parameters in the new code and refactored code of Ark Compiler, it is expected that all pointers are validated and then transferred to the called function in reference mode. In most cases, the called function should not bear the risk of null pointers in function parameters and the overhead of judgment.

The common format is as follows (In the example, `DoIt` and `Run` can be used as third-party interfaces and cannot be changed):

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

If most pointers are transferred into a reference type once obtained:

```c++
A *DoIt(B &b);
void Run(B *b) {
  B &bRef = utils::ToRef(b);
  // ...
  A &a = utils::ToRef(DoIt(bRef));
  a.Do;
}
```

If `b` is used only once:

```c++
A *DoIt(B &b);
void Run(B *b) {
  // ...
  A &a = utils::ToRef(DoIt(utils::ToRef(b)));
  a.Do;
}
```

## `bit_field_v`&`lbit_field_v`

Using a bit to mark a state switch combination is a design method that saves memory and can be efficiently encoded. Generally, the enumeration definition or constant definition is written as follows:

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

This design is obviously intended to use bits to record some attribute information, but the bits are obscure and difficult to maintain and read.

Therefore, a clearer design is required.

```c++
enum BBAttr : uint32 {
  kBBAttrIsEntry        = utils::bit_field_v<1>,
  kBBAttrIsExit         = utils::bit_field_v<2>,
  kBBAttrWontExit       = utils::bit_field_v<3>,
  kBBAttrIsTry          = utils::bit_field_v<4>,
  kBBAttrIsTryEnd       = utils::bit_field_v<5>,
  kBBAttrIsJSCatch      = utils::bit_field_v<6>,
  kBBAttrIsJSFinally    = utils::bit_field_v<7>,
  kBBAttrIsCatch        = utils::bit_field_v<8>,
  kBBAttrIsJavaFinally  = utils::bit_field_v<9>,
  kBBAttrArtificial     = utils::bit_field_v<10>,
  kBBAttrIsInLoop       = utils::bit_field_v<11>,
  kBBAttrIsInLoopForEA  = utils::bit_field_v<12>
};
```

`bit_field_v`:`uint32` and `lbit_field_v`:`uint64` are supported. In the future, `sbit_field_v`:`uint16` and `bbit_field_v`:`uint8` will be added as required.
