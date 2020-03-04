# C++编程探讨之指针

# 背景

`C/C++`中指针的使用具有极大的灵活性，伴随着的是更多的安全风险，同时这也对程序员提出了更高的要求。本文将讨论裸指针在`C/C++`中当如何被使用，乃至最终确立一种编码范式。

# 裸指针vs引用

## 成员访问

当访问对象成员时，裸指针存在为空的场景（指针的有效性由闭合对象或函数从逻辑上自保证），所以必须检查非法指针。而引用必定非空。

## 做容器成员

引用从`C++`语义中，表达的是别名关系，理论上不占内存（实际中规中矩的编译器对于引用的内部实现是指针）。引用本身不是对象，这点与指针不同，指针可以作为各容器成员，而引用不行。

# 裸指针vs智能指针

## 堆对象销毁

```c++
class Int {
  ...
 private:
  int data;
}

void test(int* in) {
  Int* tmp = new Int();
  ...
  goto LABEL;
  ...

  delete tmp;
LABEL:
}
```

对于资源（堆对象、栈对象、文件资源等）的使用，遵循**“谁申请，谁释放”**的原则(RAII)，这样可以最大限度的降低资源泄露的可能。

裸指针的`new`与`delete`之间往往会包含一段处理逻辑以及子函数调用，中间的处理逻辑可能发生异常、跳转等动作（中间的处理逻辑的行为不会由当前对象越权限制，超出`new`行为的管辖范围），而跳过资源的释放，从而造成资源泄露（如示例中`test`函数中`tmp`对象）。

智能指针改造为`auto tmp = std::make_unique<Int>();`，构造对象`tmp`时，即绑定其`delete`行为，退出当前作用域销毁，而避免了资源泄露的可能。

## 管理权vs使用权

```c++
int* delete(int* in);
```

管理权：拥有销毁、重建对象的权利

使用权：拥有访问、修改对象的权利

如上示例所示，当使用裸指针传递参数时，由于其隐含了转移所有权的属性（可能转移所有权，亦可能没有），入参`in`以及出参均无法确定行使了**管理权**还是**使用权**。调用此函数将需要额外补充信息：`in`是否会被`delete`函数销毁？返回值是否需要调用者销毁？

```c++
std::unique_ptr<int> delete(std::unique_ptr<int>& in);
```

使用智能指针将在接口中明确表达参数的角色，如`std::unique_ptr<int>& in`代表`delete`函数享有其**使用权**，函数返回值代表`delete`函数转移所有权。

# 指针使用范式

## `new`创建的对象，必须立即绑定其销毁方式

错误示例：

```c++
Object* obj = new Object();
...
delete obj;
```

正确示例：

```c++
std::unique_ptr<Object> obj = std::make_unique(new Object());
```

## 申请的资源，必须立即绑定其释放方式

错误示例：

```c++
FILE* file = open("xxx.txt");
...
file->close();
```

正确示例（本例比较通用，最佳方式应用类封装`open`）：

```c++
template <typename T, typename Func>
class ResourceGuard {
 public:
  ResourceGuard(T* _obj, Func _func) : obj(_obj), func(_func) {}

  ~ResourceGuard() { obj.func(); }
 private:
  T* obj;
  Func func;
}

FILE* file = open("xxx.txt");
auto fileGuard = ResourceGuard<FILE, std::function<void()>>(file, FILE::close);
...
```

## 确定不为空的场景，使用引用而非指针

错误示例：

```c++
void func1(int* in) {
  if (in == nullptr) return;
  ...
}

void func2() {
  int* p = nullptr;
  ...
  if (p != nullptr) {
    func1(p);
  }
}
```

正确示例：

```c++
void func1(int& in) {
  ...
}

void func2() {
  int* p = nullptr;
  ...
  if (p != nullptr) {
    func1(*p);
  }
}
```

## 作为容器成员（不具管理权），确定不为空时，使用封装的引用容器，而非指针

错误示例：

```c++
void func(std::vector<int *>& in) {
  for (auto *p : in) {
    if (p == nullptr) {
      continue;
    }
    ...
  }
}
```

正确示例：

```c++
template <typename T>
class Ref {
 public:
  Ref() = delete;
  Ref(T& ref) : data(&ref) {}

  ...

  operator T() const noexcept {
    return *data;
  }

 private:
  T* data;
}

template <typename T>
using ref_vector = std::vector<Ref<T>>;
void func(ref_vector<int>& in) {
  for (auto p : in) {
    int& data = p;
    ...
  }
}
```

## 作为容器成员（具备管理权），使用具有管理生命周期的容器，而非指针容器

错误示例：

```c++
std::vector<int *> data;
...
for (auto *p : data) {
  delete p;
}
```

正确示例：

```c++
template <typename T>
class ptr_vector {
 public:
  ~ptr_vector() {
    for (auto *p : data) {
      delete p;
    }
  }

 private:
  std::vector<T *> data;
}

ptr_vector<int> data;
...
```

## 显示转移对象管理权，明确对象使用权

`C++11`新增了`move`语义，并废弃`auto_ptr`而使用需显示转移所有权的`unique_ptr`，使得栈对象和堆对象的生命周期管理方式可以进行统一。

栈对象转移示例：

```c++
std::vector<int> func() {
  std::vector<int> data;
  data.push_back(0);
  return std::move(data);
}
```

模糊的堆对象转移示例：

```c++
Object* func() {
  std::unique_ptr<Object> data = std::make_unique(new Object);
  Object& rData = ToRef(data);
  rData.push_back(0);
  return data.release();
}
```

明晰的的堆对象转移示例：

```c++
std::unique_ptr<Object> func() {
  std::unique_ptr<Object> data = std::make_unique(new Object);
  Object& rData = ToRef(data);
  rData.push_back(0);
  return std::move(data);
}
```

## 应当使用指针场景

1. 第三方库函数传入或传出指针，但必须在调用前一刻使用`unique_ptr.get()`或`unique_ptr.release()`构建入参，出参也必须在拿到后立即使用`unique_ptr`接住或判空并转引用。
2. 作为容器成员（不具管理权），使用场景中有空指针设计，但必须在使用前立即判空并转引用，不支持指针扩散。

# 备注

上述的`Ref`、`ref_vector`已开发完成，`Ref`由于`operator.`无法被重载，所以定义为`SafePtr`。

上述的`ResourceGuard`、`ptr_vector`正在开发中，文中主要为示意。
