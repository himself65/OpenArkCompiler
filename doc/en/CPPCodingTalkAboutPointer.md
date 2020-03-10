# Discussion of Pointer in C++ Programming

# Background

The use of pointers in `C/C++` is flexible and accompanied by many security risks, which poses higher requirements on programmers. This document will discuss how to use raw pointers in `C/C++` and ultimately establish a coding paradigm.

# Raw Pointer vs Reference

## Member access.

When an object member is accessed, the raw pointer may be null (the validity of a pointer is logically ensured by a closed object or function). Therefore, invalid pointers must be checked, whereas the reference cannot be null and does not need to be checked.

## Container members.

In the semantics of `C++`, references express alias relationships, which do not occupy memory theoretically. (In practice, references are implemented internally as pointers in a compiler.) A reference is not an object in itself, which is different from a pointer. A pointer can be a container member, but a reference cannot.

# Raw Pointer vs Smart Pointer

## Destroy a heap object.

```c++
class Int {
  ...
private:
  int data;
}

void test(int *in) {
  Int* tmp = new Int();
  ...
  goto LABEL;
  ...

  delete tmp;
LABEL:
}
```

The use of resources (heap objects, stack objects, and file resources) complies with the principle that **"resources that are released in the same scope as they are acquired"** in Resource Acquisition Is Initialization (RAII), which minimizes the possibility of resource leakage.

A segment of processing logic and sub-function calling are usually involved between `new` and `delete` of a raw pointer. The intermediate processing logic may encounter exceptions or jumps. (The current object will not go beyond authority to restrict the behavior of the intermediate processing logic, which exceeds the management scope of `new`.) The resource release is skipped due to exceptions or jumps, causing resource leakage (for example, the `tmp` object in the `test` function in the preceding example).

The smart pointer is reconstructed to `auto tmp = std::make_unique<Int>();`. When the `tmp` object is constructed, the `delete` behavior is bound and the current scope is destroyed, preventing resource leakage.

## Management permission vs use permission.

```c++
int *delete(int *in);
```

Management permission: Destroy and rebuild objects.

Use permission: Access and modify objects.

As shown in the preceding example, when a raw pointer is used to transfer parameters, the use of the **management permission** or **use permission** cannot be determined by the input parameter `in` and output parameter because the raw pointer implies an attribute of transferring the ownership (possibly or not). Additional information is required when this function is called: Will the `in` parameter be destroyed by the `delete` function? Does the return value need to be destroyed by the caller?

```c++
std::unique_ptr<int> delete(std::unique_ptr<int> &in);
```

A smart pointer is used to specify a role of a parameter in an interface. For example, `std::unique_ptr& in` indicates that the `delete` function has the **use permission**, and the return value indicates that the `delete` function transfers the ownership.

# Pointer Normalization

## A destruction method must be immediately bound to an object created by `new`.

Bad example:

```c++
Object *obj = new Object();
...
delete obj;
```

Good example:

```c++
std::unique_ptr<Object> obj = std::make_unique<Object>();
```

## A release method must be immediately bound to the applied resources.

Bad example:

```c++
FILE *file = open("xxx.txt");
...
file->close();
```

Good example: (This example is commonly used. The best way is to encapsulate an application class `open`.)

```c++
template <typename T, typename Func>
class ResourceGuard {
 public:
  ResourceGuard(T *_obj, Func _func) : obj(_obj), func(_func) {}

  ~ResourceGuard() { obj.func(); }
 private:
  T *obj;
  Func func;
}

FILE* file = open("xxx.txt");
auto fileGuard = ResourceGuard<FILE, std::function<void()>>(file, FILE::close);
...
```

## Use a reference instead of a pointer when the value is not null.

Bad example:

```c++
void func1(int *in) {
  if (in == nullptr) return;
  ...
}

void func2() {
  int *p = nullptr;
  ...
  if (p != nullptr) {
    func1(p);
  }
}
```

Good example:

```c++
void func1(int &in) {
  ...
}

void func2() {
  int *p = nullptr;
  ...
  if (p != nullptr) {
    func1(*p);
  }
}
```

## As a container member (without the management permission), use an encapsulated reference container instead of a pointer when the value is not null.

Bad example:

```c++
void func(std::vector<int*> &in) {
  for (auto *p : in) {
    if (p == nullptr) {
      continue;
    }
    ...
  }
}
```

Good example:

```c++
template <typename T>
class Ref {
 public:
  Ref() = delete;
  Ref(T &ref) : data(&ref) {}

  ...

  operator T() const noexcept {
    return *data;
  }

 private:
  T *data;
}

template <typename T>
using ref_vector = std::vector<Ref<T>>;
void func(ref_vector<int> &in) {
  for (auto p : in) {
  int &data = p;
    ...
  }
}
```

## As a container member (with the management permission), use a container that has the lifecycle management permission instead of a pointer container.

Bad example:

```c++
std::vector<int*> data;
...
for (auto *p : data) {
  delete p;
}
```

Good example:

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
  std::vector<T*> data;
}

ptr_vector<int> data;
...
```

## Explicitly transfer an object management permission and specify an object use permission.

`move` semantics is added to `C++11`, and `auto_ptr` is discarded. `unique_ptr` is used to explicitly transfer the ownership so that the lifecycle management methods of stack objects and heap objects can be unified.

Example of stack object transfer:

```c++
std::vector<int> func() {
  std::vector<int> data;
  data.push_back(0);
  return std::move(data);
}
```

Example of fuzzy heap object transfer:

```c++
Object *func() {
  std::unique_ptr<Object> data = std::make_unique<Object>();
  Object &rData = ToRef(data);
  rData.push_back(0);
  return data.release();
}
```

Example of clear heap object transfer:

```c++
std::unique_ptr<Object> func() {
  std::unique_ptr<Object> data = std::make_unique<Object>();
  Object &rData = ToRef(data);
  rData.push_back(0);
  return std::move(data);
}
```

## Scenarios where pointers should be used.

1. When a third-party library function transfers in or out a pointer, `unique_ptr.get()` or `unique_ptr.release()` must be used to construct input parameters before the function is called. After output parameters are obtained, `unique_ptr` must be used to catch or check whether the output parameters are null and convert the output parameters to references.
2. As a container member (without the management permission), a null pointer is designed in the application scenario. However, the null pointer must be checked and converted to a reference immediately before use. Pointer diffusion is not supported.

# Remarks

`Ref` and `ref_vector` have been developed. `Ref` is defined as `SafePtr` because `operator.` cannot be reloaded.

The `ResourceGuard` and `ptr_vector` are being developed and are mainly used as examples in this document.
