<!-- <center><br><br><br><br><br><br><br> -->
<big><big><big><big><big><strong> Ark Compiler C++ Coding Style Guide </strong><big><big><big><big><big>

<!-- <center><br><br><br><br><br><br><br> -->






<!-- <br></center> -->


| Chapter                 | Content                                       |
| ------------------ | ---------------------------------------- |
| <u>[0 About This Document](#c0)</u>        | <u>[Purpose](#c0-1)</u> <u>[Key Points](#c0-2)</u> <u>[Conventions](#c0-3)</u> <u>[Exceptions](#c0-4)</u> |
| <u>[1 Principles](#c1)</u>        | <u>[Principles of Good Code](#c1-1)</u> <u>[Class and Function Design Guidelines](#c1-2)</u> <u>[Follow C++ ISO Standards](#c1-4)</u> <br><u>[Check Errors During Compilation](#c1-5)</u> <u>[Use Namespaces for Scoping](#c1-6)</u> <u>[Use C++ Features over C Features](#c1-7)</u> |
| <u>[2 Naming](#c2)</u>        | <u>[General Naming Rules](#c2-1)</u> <u>[File Names](#c2-2)</u> <u>[Function Names](#c2-3)</u> <u>[Type Names](#c2-4)</u> <u>[Variable Names](#c2-5)</u> <u>[Macro, Constant, and Enumeration Names](#c2-6)</u> |
| <u>[3 Formatting](#c3)</u> | <u>[Line Length](#c3-1)</u> <u>[Indentation](#c3-2)</u> <u>[Braces](#c3-3)</u> <u>[Function Declarations and Definitions](#c3-4)</u> <u>[Function Calls](#c3-5)</u> <u>[if Statements](#c3-6)</u> <u>[Loop Statements](#c3-7)</u> <u>[Switch Statements](#c3-8)</u> <u>[Expressions](#c3-9)</u> <u>[Variable Assignment](#c3-10)</u> <br><u>[Initialization](#c3-11)</u> <u>[Pointers and References](#c3-12)</u> <u>[Preprocessor Directives](#c3-13)</u> <u>[Whitespace](#c3-14)</u> <u>[Classes](#c3-15)</u> |
| <u>[4 Comments](#c4)</u>        | <u>[Comment Style](#c4-1)</u> <u>[File Header Comments](#c4-2)</u> <u>[Function Header Comments](#c4-3)</u> <u>[Code Comments](#c4-4)</u> |
| <u>[5 Header Files](#c5)</u>       | <u>[Header File Responsibility](#c5-1)</u> <u>[Header File Dependency](#c5-2)</u>            |
| <u>[6 Scopes](#c6)</u>       | <u>[Namespaces](#c6-1)</u> <u>[Global Functions and Static Member Functions](#c6-2)</u> <u>[Global Variables](#c6-3)</u> <u>[Global Constants and Static Member Constants](#c6-4)</u> |
| <u>[7 Classes](#c7)</u>         | <u>[Constructors, Copy/Move Constructors, Copy/Move Assignment Operators, and Destructors](#c7-1)</u> <u>[Inheritance](#c7-2)</u> <u>[Multiple Inheritance](#c7-3)</u> <u>[Overloading](#c7-4)</u> |
| <u>[8 Functions](#c8)</u>        | <u>[Function Design](#c8-1)</u> <u>[Inline Functions](#c8-2)</u> <u>[Function Parameters](#c8-3)</u> |
| <u>[9 Other C++ Features](#c9)</u>   | <u>[Constants and Initialization](#c9-1)</u> <u>[Expressions](#c9-2)</u> <u>[Type Casting](#c9-3)</u> <u>[Resource Allocation and Release](#c9-4)</u> <u>[Standard Template Library](#c9-5)</u> <u>[Usage of const](#c9-6)</u> <u>[Templates](#c9-7)</u> <u>[Macros](#c9-8)</u> <u>[Others](#c9-9)</u>|
| <u>[10 Modern C++ Features](#c10)</u> | <u>[Code Simplicity and Security Improvement](#c10-1)</u> <u>[Smart Pointers](#c10-2)</u> <u>[Lambda](#c10-3)</u> <u>[Interfaces](#c10-4)</u> |
| <u>[11 Security Coding Specifications](#c11)</u> | <u>[Basic Principles](#c11-1)</u> <u>[Variables](#c11-2)</u> <u>[Assertions](#c11-3)</u> <u>[Exception Mechanisms](#c11-4)</u> <u>[Memory](#c11-5)</u> <u>[Dangerous Functions](#c11-6)</u> |

# <a name="c0"></a>0 About This Document

## <a name="c0-1"></a>Purpose

Rules are not perfect. Disabling useful features in specific situations may affect implementation. However, the rules are formulated "to benefit most programmers". If a rule is found unhelpful or difficult to follow during team coding, please send feedback so we can improve it accordingly.

Before referring to this guide, you are expected to have the following basic capabilities for C++. It is not for a beginner that wants to learn about C++.
1. Have a general knowledge of ISO standards for C++.
2. Be familiar with the basic features of C++, including those of C++ 03/11/14/17.
3. Have a general knowledge of the C++ Standard Library.


## <a name="c0-2"></a>Key Points
1. C++ programming style, such as naming and typesetting.
2. C++ modular design, including how to design header files, classes, interfaces, and functions.
3. Best practices of C++ features, including constants, type casting, resource management, and templates.
4. Best practices of modern C++, including conventions that can improve code maintainability and reliability in C++ 11/14/17.


## <a name="c0-3"></a>Conventions
**Rule**: a regulating principle that must be followed during programming.

**Recommendation**: a guideline that must be considered during programming.

This document is applicable to standard C++ versions (03/11/14/17) unless otherwise specified in the rule.

## <a name="c0-4"></a>Exceptions
It is necessary to understand the reason for each 'rule' or 'recommendation' and to try and comply with them.
However, some rules and recommendations have exceptions.

The only acceptable exceptions are those that do not violate the general principles and provide appropriate reasons for the exception.
Try to avoid exceptions because they affect the code consistency. Exceptions to 'Rules' should be very rare.

The style consistency principle is preferred in the following cases:  
**When you modify external open source or third-party code, the existing code specifications prevail.**  
**For specific domains, the domain specifications prevail.**

# <a name="c1"></a>1 Principles

## <a name="c1-1"></a>Principles of Good Code
We refer to Kent Beck's four rules of simple design to guide our coding and to identify good code.
1. Passes its tests
2. Minimizes duplication
3. Maximizes clarity
4. Has fewer elements
5. The importance of the preceding four rules decreases in sequence.
    They are referred to as rules of simple design.

The first point is the most important as it stresses external requirements. The second point refers to the modular design of code to ensure orthogonality and maintainability. The third point refers to code readability. The fourth point is code simplicity. Of course, we still emphasize expressiveness over simplicity.

## <a name="c1-2"></a>Class and Function Design Guidelines
C++ is a typical object-oriented programming (OOP) language. The software engineering industry has developed many OOP principles to guide programmers in compiling large-scale, highly scalable, and maintainable code:
- Basic rule of high cohesion and low coupling: improves the reuse and portability of program modules.
- SOLID principles: consists of single responsibility, open–closed, Liskov substitution, interface segregation, and dependency inversion. The SOLID principles can make the program less coupled and more robust.
- Law of Demeter: reduces coupling between classes.
- "Tell, Don't ask": suggests that it is better to issue an object a command to perform some operation or logic, rather than to query its state and then take some action as a result.
- Composition/Aggregation Principle (CARP): favors composition/aggregation over class inheritance.

## <a name="c1-3"></a>Follow C++ ISO Standards
It is hoped that C++ code is written using features compliant with ISO standards. Features that are not defined by ISO or those used in compilers must be used with caution. Extended features provided by compilers such as GCC must be used with caution too, because these features lead to poor portability.

Note: If extended features are required by the product, encapsulate these features into independent interfaces and enable or disable these features through options on the interface. Develop programming manuals to instruct programmers on the use of these extended features.

## <a name="c1-4"></a>Check Errors During Compilation
Use compilers to ensure code robustness instead of compiling error processing codes to handle exceptions.

- Use const to ensure data consistency and prevent data from being modified unexpectedly.
- Run the static_assert command to check errors at compilation time.

## <a name="c1-5"></a>Use Namespaces for Scoping
Global variables, global constants, and global type definitions belong to the global scope. Conflicts may occur when a third-party library is used in a project.

Namespaces divide a scope into independent, name-specified scopes that can effectively prevent name conflicts within the global scope.
1. Classes and structs have their own scopes.
2. A named namespace can implement an upper-level scope, higher than a class scope.
3. Unnamed namespaces and the static keyword can be used to implement a file scope.

We strongly recommend programmers not use global macro variables and functions, and instead place them inside a more restrictive scope.

Scopes have the following disadvantages:
1. Although two types of the same name can be distinguished in different scopes, they are still confusing to readers.
2. An inline namespace allows its members to be treated as if they are members of the enclosing namespace, which is also confusing to readers.
3. A nested namespace definition can make names lengthy when the namespace needs to be referenced.

Therefore, we recommended:
- For variables, constants, and type definitions, use namespaces as much as possible to reduce conflicts within the global scope.
- Do not use "using namespace" in header files.
- Do not use inline namespaces.
- Encapsulate definitions using unnamed namespaces or the static keyword in .cpp files to prevent leaking through APIs.


## <a name="c1-6"></a>Use C++ Features over C Features
C++ is more type safe and more abstract than C. It is recommended that you use C++ features for programming. For example, use strings instead of `char*`, use vectors instead of native arrays, and use namespaces instead of statically defined members.


# <a name="c2"></a>2 Naming
## <a name="c2-1"></a>General Naming Rules
General naming styles include the following:
__CamelCase__
CamelCase is the practice of writing compound words or phrases so that each word or abbreviation in the phrase begins with a capital letter, with no intervening spaces or punctuation.
There are two conventions: UpperCamelCase and lowerCamelCase.

__Kernel Style (Unix-like)__
Words are in lowercase and are separated with underscores (_).
'test_result'

__Hungarian Style__
Add a prefix to UpperCamelCase. The prefix indicates the type or usage.
'uiSavedCount ', or ' bTested'

### <a name="r2-1-1"></a>Rule 2.1.1 Use the CamelCase style for identifier names.
The Hungarian style is not considered for identifier names, and we choose the CamelCase style over the Kernel style.

| Type                                                         | Naming Style                                 |
| ------------------------------------------------------------ | ---------------------------------------- |
| Class Type, Struct Type, Enumeration Type, and Union Type Definitions           | UpperCamelCase                                   |
| Functions (Including Global Functions, Scope Functions, and Member Functions)                     | UpperCamelCase (You can add a prefix to an interface. XXX_FunctionName) |
| Global Variables (Including Variables of the Global and Namespace Scopes, Namespace Variables, and Class Static Variables), Local Variables, Function Parameters, and Class, Struct, and Union Member Variables | lowerCamelCase                                   |
| Constant, Enumerated Value                                          | k+CamelCase                             |
| Macro                                                           | All caps, separated with underscores (_)                              |
| Namespace                                                     | All in lowercase                                   |

Note:
__Constant__ indicates the variables of the basic, enumeration, or character string type modified by const or constexpr in the global scope, the namespace scope, and the scope of a static member of a class.
__Variable__ indicates the variables excluding those defined in Constant. These variables use the lowerCamelCase style.

## <a name="c2-2"></a>File Names
### <a name="a2-2-1"></a>Recommendation 2.2.1 Use .cpp as the C++ file name extension and .h as the header file name extension. Use Kernel style for file names.

At present, there are some other file name extensions used by programmers:

- Header files:  .hh, .hpp, .hxx
- Implementation files: .cc, .cxx, .C

This document uses .h and .cpp extensions.

File names are as follows:
- database_connection.h
- database_connection.cpp


## <a name="c2-3"></a>Function Names
Functions are named in UpperCamelCase. Generally, the verb or verb-object structure is used. You can add a prefix to an interface. XXX_FunctionName
```cpp
class List {
 public:
  void AddElement(const Element& element);
  Element GetElement(const unsigned int index) const;
  bool IsEmpty() const;
  bool MCC_GetClass();
};

namespace utils {
void DeleteUser();
}
```

## <a name="c2-4"></a>Type Names

Types are named in the UpperCamelCase style.
All types, such as classes, structs, unions, typedefs, and enumerations, use the same conventions.

```cpp
// classes, structs and unions
class UrlTable { ...
class UrlTableTester { ...
struct UrlTableProperties { ...
union Packet { ...

// typedefs
typedef std::map<std::string, UrlTableProperties*> PropertiesMap;

// enums
enum UrlTableErrors { ...
```

For namespace naming, UpperCamelCase is recommended.
```cpp
// namespace
namespace osutils {
  
namespace fileutils {
      
}
  
}
```


## <a name="c2-5"></a>Variable Names
General variables are named in lowerCamelCase, including global variables, function parameters, local variables, and member variables.
```cpp
std::string tableName;  // Good: Recommended style.
std::string tablename;  // Bad: Forbidden style.
std::string path;       // Good: When there is only one word, lowerCamelCase (all lowercase) is used.

class Foo {
 private:
  std::string fileName;    // Do not add a prefix or suffix that identifies the scope.
};
```

## <a name="c2-6"></a>Macro, Constant, and Enumeration Names
For macros, use all caps separated with underscores (_). For constants and enumerated values, use k+CamelCase.
Local constants and ordinary const member variables use the lowerCamelCase naming style.

```cpp
#define MAX(a, b)   (((a) < (b)) ? (b) : (a)) // Example of naming a macro only.

enum TintColor {    // Note: Enumerated types are named in the UpperCamelCase style, while their values are in k+CamelCase style.
  kRed,
  kDarkRed,
  kGreen,
  kLightGreen
};

int Func(...) {
  const unsigned int bufferSize = 100;   // Local variable
  char *p = new char[bufferSize];
  ...
}

namespace utils {
const unsigned int kFileSize = 200;      // Global variable
}

```

# <a name="c3"></a>3 Formatting
While programming styles coexist to meet different requirements, we strongly recommend that you use a standardized coding style in the same project so that everyone can easily read and understand the code and the code can be easily maintained.

## <a name="c3-1"></a>Line Length

### <a name="a3-1-1"></a>Recommendation 3.1.1 Each line of code should contain a maximum of 120 characters.
It is recommended that the number of characters in each line not exceed 120. If the line of code exceeds the permitted length, wrap the line appropriately.

Exception:
- If a one-line comment contains a command or URL of more than 120 characters, you can keep the line for ease in using copy, paste, and search using the grep command.
- The length of an #include statement can contain a long path exceeding 120 characters, but this should be avoided if possible.
- The error information in preprocessor directives can exceed the permitted length.
Put the error information of preprocessor directives in one line to facilitate reading and understanding even if the line contains more than 120 characters.

```cpp
#ifndef XXX_YYY_ZZZ
#error Header aaaa/bbbb/cccc/abc.h must only be included after xxxx/yyyy/zzzz/xyz.h, because xxxxxxxxxxxxxxxxxxxxxxxxxxxxx
#endif
```

## <a name="c3-2"></a>Indentation

### <a name="r3-2-1"></a>Rule 3.2.1 Use spaces to indent and indent two spaces at a time.
Only spaces can be used for indentation. Two spaces are indented each time.




## <a name="c3-3"></a>Braces
### <a name="r3-3-1"></a>Rule 3.3.1 Use the K&R indentation writing style except for functions.
The left brace of the function is placed at the end of the statement.
The right brace starts a new line and nothing else is placed on the line, unless it is followed by the remaining part of the same statement, for example, "while" in the do statement, "else" or "else if" in the if statement, a comma, and a semicolon.

 
```cpp
struct MyType {     // Follow the statement to the end, and indent one space.
  ...
};

int Foo(int a) {                   // The left brace of the function is placed at the end of the statement.
  if (...) {
    ...
  } else {
    ...
  }
}
```
 

- Code is more compact.
- Placing the brace at the end of the statement makes the code more continuous in reading rhythm than starting a new line.
- This style complies with mainstream norms and habits of programming languages.
- Most modern IDEs have an automatic code indentation, alignment and display. Placing the brace at the end of a line does not impact understanding.


If no function body is inside the braces, the braces can be put on the same line.
```cpp
class MyClass {
 public:
  MyClass() : value(0) {}
    
 private:
  int value;
};
```

## <a name="c3-4"></a>Function Declarations and Definitions

### <a name="r3-4-1"></a>Rule 3.4.1 The return type and the function name of a function declaration or definition must be on the same line. When the length of the function parameter list exceeds the permitted length, a line break is required and parameters must be aligned appropriately.
When a function is declared or defined, the return value type of the function should be on the same line as the function name. If the line length permits, the function parameters should be placed on the same line. Otherwise, the function parameters should be wrapped and properly aligned.
The left parenthesis of a parameter list should always be on the same line as the function name. The right parenthesis always follows the last parameter.

The following is an example of line breaks:
```cpp
ReturnType FunctionName(ArgType paramName1, ArgType paramName2) {  // Good: All are on the same line.
  ...
}

ReturnType VeryVeryVeryLongFunctionName(ArgType paramName1,     // Each added parameter starts on a new line because the line length limit is exceeded.
                                        ArgType paramName2,     // Good: Aligned with the previous parameter
                                        ArgType paramName3) {
  ...
}

ReturnType LongFunctionName(ArgType paramName1, ArgType paramName2, // The parameters are wrapped because the line length limit is exceeded.
    ArgType paramName3, ArgType paramName4, ArgType paramName5) {     // Good: After the line break, 4 spaces are used for indentation.
  ...
}

ReturnType ReallyReallyReallyReallyLongFunctionName(            // The line length cannot accommodate even the first parameter, and a line break is required.
    ArgType paramName1, ArgType paramName2, ArgType paramName3) { // Good: After the line break, 4 spaces are used for indentation.
  ...
}
```

## <a name="c3-5"></a>Function Calls
### <a name="r3-5-1"></a>Rule 3.5.1 A function call parameter list should be placed on one line. When the parameter list exceeds the line length and requires a line break, the parameters should be properly aligned.
The left parenthesis always follows the function name, and the right parenthesis always follows the last parameter.


The following is an example of line breaks:
```cpp
ReturnType result = FunctionName(paramName1, paramName2);   // Good: All function parameters are on one line.

ReturnType result = FunctionName(paramName1,
                                 paramName2,                // Good: Aligned with the previous parameter.
                                 paramName3);

ReturnType result = FunctionName(paramName1, paramName2, 
    paramName3, paramName4, paramName5);                    // Good: Parameters are wrapped. After the line break, 4 spaces are used for indentation.

ReturnType result = VeryVeryVeryLongFunctionName(           // The line length cannot accommodate even the first parameter, and a line break is required.
    paramName1, paramName2, paramName3); // After the line break, 4 spaces are used for indentation.
```

If some of the parameters called by a function are associated with each other, you can group them for better understanding.
```cpp
// Good: The parameters in each line represent a group of data structures with a strong correlation. They are placed on one line for ease of understanding.
int result = DealWithStructureLikeParams(left.x, left.y,     // A group of related parameters.
                                         right.x, right.y);  // Another group of related parameters.
```

## <a name="c3-6"></a>if Statements

### <a name="r3-6-1"></a>Rule 3.6.1 Use braces to include an if statement.
We require that all if statements use braces, even if there is only one statement.

 
- The logic is intuitive and easy to read.
- It is less prone to mistakes when new code is added to the existing if statement.
- If function-like macros are used in a conditional statement, it is less prone to mistakes (in case the braces are missing when macros are defined).

```cpp
if (objectIsNotExist) {         // Good: Braces are added to a single-line conditional statement.
  return CreateNewObject();
}
```
### <a name="r3-6-2"></a>Rule 3.6.2 Place if, else, and else if keywords on separate lines.
If there are multiple branches in a conditional statement, they should be placed on separate lines.

Good example:

```cpp
if (someConditions) { 
  DoSomething();
  ...
} else {  // Good: Put the if and else keywords on separate lines.
  ...
}
```

Bad example:

```cpp
if (someConditions) { ... } else { ... } // Bad: The if and else keywords are put on the same line.
```

## <a name="c3-7"></a>Loop Statements
### <a name="r3-7-1"></a>Rule 3.7.1 Use braces after loop statements.
Similar to if statements, we require that the for and while loop statements contain braces, even if the loop body is empty or there is only one loop statement.

```cpp
for (int i = 0; i < someRange; i++) {
  DoSomething();
}
```

If the loop body is empty, use empty braces instead of a single semicolon. A single semicolon is easy to miss or incorrectly regarded as a part of the loop statement.

```cpp
for (int i = 0; i < someRange; i++) { } // Good: The for loop body is empty. Braces should be used, instead of semicolons (;).

while (someCondition) { }  // Good: The while loop body is empty. Braces should be used, instead of semicolons (;).

while (someCondition) {
  continue;  // Good: The continue keyword highlights the end of the empty loop. Braces are optional in this case.
}

```

Bad example:
```cpp
for (int i = 0; i < someRange; i++) ;   // Bad: The for loop body is empty. Braces are mandatory.

while (someCondition) ; // Bad: Using a semicolon here will make people misunderstand that it is a part of the while statement and not the end to it.
```

## <a name="c3-8"></a>Switch Statements
### <a name="r3-8-1"></a>Rule 3.8.1 Indent case and default in a switch statement with four spaces.
This rule includes the requirement to further indent all content encased by a case or the default case.
```cpp
switch (var) {
  case 0:             // Good: Indented
    DoSomething1();   // Good: Indented
    break;
  case 1: {           // Good: Braces are added.
    DoSomething2();
    break;
  }
  default:
    break;
}
```

```cpp
switch (var) {
case 0:                 // Bad: case is not indented.
  DoSomething();
  break;
default:                // Bad: default is not indented.
  break;
}
```

## <a name="c3-9"></a>Expressions

### <a name="a3-9-1"></a>Recommendation 3.9.1 Keep a consistent line break style for expressions.
A long expression that does not meet the line length requirement must be wrapped appropriately.
 

// Assume that the first line exceeds the length limit.
```cpp
if (currentValue > threshold &&  
  someConditionsion) {
  DoSomething();
  ...
}

int result = reallyReallyLongVariableName1 +    // Good
             reallyReallyLongVariableName2;
```
After an expression is wrapped, ensure that the lines are aligned appropriately or indented with 4 spaces. See the following example.

```cpp
int sum = longVaribleName1 + longVaribleName2 + longVaribleName3 +
    longVaribleName4 + longVaribleName5 + longVaribleName6;         // Good: Indented with 4 spaces.

int sum = longVaribleName1 + longVaribleName2 + longVaribleName3 +
          longVaribleName4 + longVaribleName5 + longVaribleName6;   // Good: The lines are aligned.
```
## <a name="c3-10"></a>Variable Assignment

### <a name="r3-10-1"></a>Rule 3.10.1 Multiple variable definitions and assignment statements cannot be written on one line.
Each line should have only one variable initialization statement. It is easier to read and understand.

```cpp
int maxCount = 10;
bool isCompleted = false;
```

Bad example:

```cpp
int maxCount = 10; bool isCompleted = false; // Bad: Multiple variable initialization statements must be separated on different lines. Each variable initialization statement occupies one line.
int x, y = 0;  // Bad: Multiple variable definitions must be separated on different lines. Each definition occupies one line.

int pointX;
int pointY;
...
pointX = 1; pointY = 2;  // Bad: Multiple variable assignment statements must be separated on different lines.
```
Exception: Multiple variables can be declared and initialized in the for loop header, if initialization statement (C++17), and structured binding statement (C++17). Multiple variable declarations in these statements have strong associations. Forcible division into multiple lines may cause problems such as scope inconsistency and separation of declaration from initialization.

## <a name="c3-11"></a>Initialization
Initialization is applicable to structs, unions, and arrays.

### <a name="r3-11-1"></a>Rule 3.11.1 When an initialization list is wrapped, ensure that the line after the break is indented and aligned properly.
If a structure or array initialization list is wrapped, the line after the break is indented with four spaces.
Choose the wrap location and alignment style for best comprehension.

```cpp
const int rank[] = {
    16, 16, 16, 16, 32, 32, 32, 32,
    64, 64, 64, 64, 32, 32, 32, 32
};
```

## <a name="c3-12"></a>Pointers and References
### <a name="a3-12-1"></a>Recommendation 3.12.1 The pointer type "`*`" follows a variable name. There is one space between variable name and type.

```cpp
int *p = nullptr;  // Good
```

Exception: When a variable is modified by const or restrict, "`*`" cannot follow the variable or type.
```cpp
char * const VERSION = "V100";
```

### <a name="a3-12-2"></a>Recommendation 3.12.2 The reference type "`&`" follows a variable name. There is one space between variable name and type.
```cpp
int i = 8;

int &p = i;     // Good
```

## <a name="c3-13"></a>Preprocessor Directives
### <a name="r3-13-1"></a>Rule 3.13.1 The number sign "#" that starts a preprocessor directive must be at the beginning of the line and is not indented in nested preprocessor directives.
The number sign "#" that starts a preprocessor directive must be at the beginning of the line even through the preprocessor directive is inside a function.

```cpp
#if defined(__x86_64__) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16) // Good: "#" is at the beginning of the line.
#define ATOMIC_X86_HAS_CMPXCHG16B 1  // Good: "#" is at the beginning of the line.
#else
#define ATOMIC_X86_HAS_CMPXCHG16B 0
#endif 


int FunctionName() {
  if (someThingError) {
    ...
#ifdef HAS_SYSLOG        // Good: Even in the function body, "#" is at the beginning of the line.
    WriteToSysLog();
#else
    WriteToFileLog();
#endif
  }
}
```
The nested preprocessor directives starting with "#" is not indented.

```cpp
#if defined(__x86_64__) && defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16)
#define ATOMIC_X86_HAS_CMPXCHG16B 1  // Good: Wrapped for easier comprehension.
#else
#define ATOMIC_X86_HAS_CMPXCHG16B 0
#endif
```

## <a name="c3-14"></a>Whitespace
### <a name="r3-14-1"></a>Rule 3.14.1 Ensure that horizontal spaces are used to highlight keywords and important information, and avoid unnecessary whitespace.
Horizontal spaces are used to highlight keywords and important information. Spaces are not allowed at the end of each code line. The general rules are as follows:

- Add spaces after keywords such as if, switch, case, do, while, and for.
- Do not add spaces after the left parenthesis or before the right parenthesis.
- For expressions enclosed by braces, either add a space on either side or avoid a space on either side.
- Do not add spaces after unary operators (& * + - ~ !).
- Add a space to the left and right sides of each binary operator (= + - < > * / % | & ^ <= >= == !=).
- Add spaces to the left and right sides of a ternary operator (? :).
- Do not add spaces between a prefix or suffix increment (++) or decrement (--) operator and a variable.
- Do not add spaces before or after a struct member operator (. ->).
- Do not add spaces before commas. Add spaces after commas.
- Do not add spaces between a template or type conversion operator (<>) and a type.
- Do not add spaces before or after a domain operator (::).
- Determine whether to add spaces before and after a colon (:) based on the situation.

In normal cases:
```cpp
void Foo(int b) {  // Good: A space is before the left brace.

int i = 0;  // Good: During variable initialization, there should be spaces before and after =. Do not leave a space before the semicolon.

int buf[kBufSize] = {0};    // Good: Spaces are not allowed in braces.
```

Function definition and call:
```cpp
int result = Foo(arg1,arg2);
                    ^    // Bad: Function arguments must be separated by spaces for explicit display.

int result = Foo( arg1, arg2 );
                 ^          ^  // Bad: There cannot be spaces after the left parenthesis or before the right parenthesis.
```

Pointer and Address Operator
```cpp
x = *p;     // Good: There is no space between the operator * and the pointer p.
p = &x;     // Good: There is no space between the operator & and the variable x.
x = r.y;    // Good: When a member variable is accessed through the operator (.), no space is added.
x = r->y;   // Good: When a member variable is accessed through the operator (->), no space is added.
```

Other Operators:
```cpp
x = 0;   // Good: There is a space before and after the assignment operator (=).
x = -5;  // Good: There is no space between the minus sign (–) and the number.
++x;     // Good: Do not add spaces between a prefix or suffix increment (++) or decrement (--) operator and a variable.
x--;

if (x && !y)  // Good: There is a space before and after the Boolean operator. There is no space between the ! operator and the variable.
v = w * x + y / z;  // Good: There is a space before and after the binary operator.
v = w * (x + z);    // Good: There is no space before or after the expression in the parentheses.

int a = (x < y) ? x : y;  // Good: Ternary operator. There is a space before and after ? and :
```

Loops and Conditional Statements:
```cpp
if (condition) {  // Good: There is a space between the if keyword and the left parenthesis, and no space before or after the conditional statement in the parentheses.
  ...
} else {           // Good: There is a space between the else keyword and the left brace.
  ...
}

while (condition) {}   // Good: There is a space between the while keyword and the left parenthesis. There is no space before or after the conditional statement in the parentheses.

for (int i = 0; i < someRange; ++i) {  // Good: There is a space between the for keyword and the left parenthesis, and after the semicolons.
  ...
}

switch (condition) {  // Good: There is a space after the switch keyword.
  case 0:     // Good: There is no space between the case condition and the colon.
    ...
    break;
    ...
  default:
    ...
    break;
}
```

Templates and Conversions
```cpp
// Angle brackets (< and >) are not adjacent to space. There is no space before < or between > and (.
vector<string> x;
y = static_cast<char*>(x);

// There can be a space between the type and the pointer operator. Keep the spacing style consistent.
vector<char *> x;
```

Scope Operators
```cpp
std::cout;    // Good: Namespace access. Do not leave spaces.

int MyClass::GetValue() const {}  // Good: Do not leave spaces in the definition of member functions.
```

Colons
```cpp
// Scenarios when space is required.

// Good: Add a space before or after the colon in a derived class definition.
class Sub : public Base {
    
};

// Add a space before and after the colon for the initialization list of a constructor function.
MyClass::MyClass(int var) : someVar(var) {
  DoSomething();
}

// Add a space before and after the colon in a bit-field.
struct XX {
  char a : 4;     
  char b : 5;     
  char c : 4;
};
```

```cpp
// Scenarios when space is not required.

// Good: // No space is added before or after the colon next to a class access permission (public or private).
class MyClass {
 public:
  MyClass(int var);
 private:
  int someVar;
};

// No space is added before or after the colon in a switch statement.
switch (value) {
  case 1:
    DoSomething();
    break;
  default:
    break;
}
```

Note: Currently, all IDEs support automatic deletion of spaces at the end of a line. Please configure your IDE correctly.

### <a name="a3-14-2"></a>Recommendation 3.14.2 Use blank lines only when necessary to keep code compact.

There must be as few blank lines as possible so that more code can be displayed for easy reading. Recommendations:
- Add blank lines according to the correlation between lines.
- Consecutive blank lines are not allowed inside functions, type definitions, macros, and initialization expressions.
- A maximum of **two** consecutive blank lines can be used.
-.Do not add blank lines on the first and last lines of a code block.

```cpp
int Foo() {
  ...
}


// Bad: More than one blank lines are used between two function definitions.
int Bar() {
  ...
}


if (...) {
        // Bad: Do not add blank lines on the first and last lines of a code block.
  ...
        // Bad: Do not add blank lines on the first and last lines of a code block.
}

int Foo(...) {
        // Bad: Do not add blank lines before the first statement in a function body.
  ...
}
```

## <a name="c3-15"></a>Classes
### <a name="r3-15-1"></a>Rule 3.15.1 Class access specifier declarations are in the sequence: public, protected, private. Indent each specifier with one space.
```cpp
class MyClass : public BaseClass {
 public:      // Indented with 1 space.
  MyClass();  // Indented with 2 spaces.
  explicit MyClass(int var);
  ~MyClass() {}

  void SomeFunction();
  void SomeFunctionThatDoesNothing() {
  }

  void SetVar(int var) { 
    someVar = var; 
  }
  
  int GetVar() const { 
    return someVar; 
  }

 private:
  bool SomeInternalFunction();

  int someVar;
  int someOtherVar;
};
```

In each part, it is recommended that similar statements be put together in the following order: Type (including typedef, using, nested structs and classes), Constant, Factory Function, Constructor, Assignment Operator, Destructor, Other Member Function, and Data Member.


### <a name="r3-15-2"></a>Rule 3.15.2 The constructor initialization list must be on the same line or wrapped and aligned with four spaces of indentation.
```cpp
// If all variables can be placed on the same line:
MyClass::MyClass(int var) : someVar(var) {
  DoSomething();
}

// If the variables cannot be placed on the same line:
// Wrapped at the colon and indented with four spaces.
MyClass::MyClass(int var)
    : someVar(var), someOtherVar(var + 1) { // Good: Add a space after the comma.
  DoSomething();
}

// If an initialization list needs to be placed in multiple lines, put each member on a separate line and align between lines.
MyClass::MyClass(int var)
    : someVar(var),             // Indented with 4 spaces.
      someOtherVar(var + 1) {  
  DoSomething();
}
```

# <a name="c4"></a>4 Comments
Generally, clear architecture and good naming are recommended to improve code readability, and comments are provided only when necessary.
Comments are used to help readers quickly understand code. Therefore, comments should be provided __for the sake of readers__.

Comments must be concise, clear, and unambiguous, ensuring that information is complete and not redundant.

__Comments are as important as code.__
When writing a comment, you need to step into the reader's shoes and use comments to express what the reader really needs. Comments are used to express the function and intention of code, rather than repeating the code.
When modifying the code, ensure that the comments are consistent with the modified code. It is not polite to modify only code and keep the old comments, which will undermine the consistency between code and comments, and may confuse or even mislead readers.

## <a name="c4-1"></a>Comment Style
In C++ code, both ` /* */` and ` // ` can be used for commenting.
Comments can be classified into different types, such as file header comments, function header comments, and code comments. This is based on their purposes and positions.
Comments of the same type must keep a consistent style.
(1) Use ` /* */ ` for file header comments.
(2) The style of function header comments and code comments in the same file must be consistent.

Note: __Example code in this document uses comments in the '//' format only to better describe the rules and recommendations. This does not mean this comment format is better.__

## <a name="c4-2"></a>File Header Comments
### <a name="r4-2-1"></a>Rule 4.2.1 File header comments must contain the copyright notice.
```cpp
/*
 * Copyright (c) [2019] [name of copyright holder]
 * [Software Name] is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *   http://license.coscl.org.cn/MulanPSL
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 * PURPOSE.
 * See the Mulan PSL v1 for more details.
 */
```



## <a name="c4-3"></a>Function Header Comments
### <a name="r4-3-1"></a>Rule 4.3.1 Function header comments with no content are forbidden.
Not all functions need function header comments.
For information that cannot be described by function signatures, add function header comments.

Function header comments are placed above the function declaration or definition. Use one of the following styles:
Use `//` to start the function header.

```cpp
// Single-line function header
int Func1(void);

// Multi-line function header
// Second line
int Func2(void);
```

Use `/*   */` to start the function header.
```cpp
/* single-line function header */
int Func1(void);

/*
 * Another single-line function header
 */
int Func2(void);

/*
 * Multi-line function header
 * Second line
 */
int Func3(void);
```
Use function names to describe functions, and add function header comments if necessary.
Do not write useless or redundant function headers. Do not write empty function headers with no content.

The function header comment content will depend on the function and includes but is not limited to: a function description, return value, performance constraints, usage comments, memory conventions, algorithm implementation, reentering requirements.
In the function interface declaration in the external header file, the function header comment should clearly describe important and useful information.

Good example:

```cpp
/*
 * The number of written bytes is returned. If -1 is returned, the write operation failed.
 * Note that, the memory buffer should be released by the caller.
 */
int WriteString(const char *buf, int len);
```

Bad example:
```cpp
/*
 * Function name: WriteString
 * Function: Write a character string.
 * Parameters:
 * Return value:
 */
int WriteString(const char *buf, int len);
```
Problems:

- The 'Parameters' and 'Return value' have no content.
- The function name comment is redundant.
- The most important thing, that is, who needs to release the buffer, is not clearly stated.

## <a name="c4-4"></a>Code Comments
### <a name="r4-4-1"></a>Rule 4.4.1 Code comments are placed above or to the right of the corresponding code.
### <a name="r4-4-2"></a>Rule 4.4.2 There must be a space between the comment character and the comment content. At least one space is required between the comment and code if the comment is placed to the right of code.
Comments placed above the code should be indented the same as that of the code.
Use one of the following styles:
Use `//`.
```cpp

// Single-line comment
DoSomething();

// Multi-line comment
// Second line
DoSomething();
```

Use `/*' '*/`.
```cpp
/* Single-line comment */
DoSomething();

/*
 * Multi-line comment in another mode
 * Second line
 */
DoSomething();
```
Leave at least one space between the code and the comment on the right. It is recommended that no more than four spaces be left.
You can use the Tab key to indent 1–4 spaces, set this in your IDE or editor.

Use one of the following styles:

```cpp
int foo = 100;  // Comment on the right
int bar = 200;  /* Comment on the right */
```
It is more appealing sometimes when the comment is placed on the right of code and the comments are aligned vertically.
After the alignment, ensure that the comment is still 1–4 spaces away from the widest line of code.
Example:

```cpp
const int kConst = 100;          /* Related comments of the same type can be aligned vertically. */
const int kAnotherConst = 200;   /* Leave spaces after code to align comments vertically.*/
```
When the comment on the right exceeds the line width, consider placing the comment above the code.

### <a name="r4-4-3"></a>Rule 4.4.3 Delete unused code segments. Do not comment them out.
Code that is commented out cannot be maintained normally. When you attempt to restore the code, it is very likely to introduce easy to overlook defects.
The correct method is to delete unnecessary code directly. If necessary, consider porting or rewriting the code.

Here, commenting out the removal of code from compilation without actually deleting it. This is done using /* */, //, #if 0, #ifdef NEVER_DEFINED, and so on.

### <a name="a4-4-1"></a>Recommendation 4.4.1 Try not to contain a TODO/TBD/FIXME comment in code.
TODO/TBD comments are used to describe required improvements and supplements.
FIXME comments are used to describe defects that need fixing.
They should have a standardized style, which facilitates text search. 

```cpp
// TODO(<author-name>): XX
// FIXME: XX
```


# <a name="c5"></a>5 Header Files
## <a name="c5-1"></a>Header File Responsibility
A header file is an external interface of a module or file. The design of a header file shows most of the system design.
The interface declaration for most functions is more suitable placed in the header file, but implementation (except inline functions) cannot be placed in the header file. Functions, macros, enumerations, and structure definitions that need to be used in .cpp files cannot be placed in the header file.
The header responsibility should be simple. A too complex header file will make dependencies complex and cause a long compilation time.

### <a name="a5-1-1"></a>Recommendation 5.1.1 Each .cpp file should have a .h file with the same name. The file is used to declare the classes and interfaces that need to be exposed externally.
Generally, each .cpp file has a corresponding .h file. This .cpp file is used to store the function declarations, macro definitions, and class definitions that are to be disclosed externally. In addition, corresponding .inline.h files can be added based on sit requirements to optimize code.
If a .cpp file does not need to open any interface externally, it should not exist.
Exception: __An entry point (for example, the file where the main function is located), unit tests, and dynamic library code.__

Example:
```cpp
// Foo.h 

#ifndef FOO_H
#define FOO_H

class Foo {
 public:
  Foo();
  void Fun();
    
 private:
  int value;
};

#endif
```

```cpp
// Foo.cpp
#include "Foo.h"

namespace { // Good: The declaration of the internal function is placed in the header of the .cpp file, and has been limited to the unnamed namespace or static scope.
void Bar() {
}
}

...

void Foo::Fun() {
  Bar();
}
```

## <a name="c5-2"></a>Header File Dependency
### <a name="r5-2-1"></a>Rule 5.2.1 Header file cyclic dependency is forbidden.
Cyclic dependency of header files means that a.h contains b.h, b.h contains c.h, and c.h contains a.h. If any header file is modified, all code containing a.h, b.h, and c.h needs to be recompiled.  
For a unidirectional dependency, for example, a.h contains b.h, b.h contains c.h, and c.h does not contain any header file, modifying a.h does not mean that we need to recompile the source code for b.h or c.h.

The cyclic dependency of header files directly reflects the unreasonable architecture design, which can be avoided by optimizing the architecture.

### <a name="r5-2-2"></a>Rule 5.2.2 Do not include unnecessary header files.
The inclusion of header files that are not used will cause unnecessary dependency, which increases the coupling between modules or units. As long as a header file is modified, the code needs to be recompiled.

In many systems, the inclusion relationships of header files are complex. To save time, developers may directly include all header files in their files, or even release a god.h file that contains all header files to project teams. This will cause a great time in compilation and great trouble in maintenance.

### <a name="r5-2-3"></a>Rule 5.2.3 Header files should be self-contained.
Simply, self-containing means that any header file can be compiled independently. For a file containing a header file, unnecessary burdens are added to users if the file cannot work unless the header file contains another header file.

For example, if the a.h header file is not self-contained, but must contain b.h, it will cause:

Each .cpp file that uses the a.h header file must include the additional b.h header file to ensure that the a.h content can be compiled.
The additional b.h header file must be included before a.h, which has a dependency in the inclusion order.


### <a name="r5-2-4"></a>Rule 5.2.4 Header files must have `#define` guards to prevent multiple inclusion.
To prevent header files from being included multiple times, all header files should be protected by #define. Do not use #pragma once.

When defining a protection character, comply with the following rules:
(1) The protection character uses a unique name.
(2) Do not place code or comments (except for file header comments) before or after the protected part.

Example: Assume that the timer.h file of the timer module of the VOS project is in the VOS/include/timer/Timer.h directory. Perform the following operations to protect the timer.h file:

```cpp
#ifndef VOS_INCLUDE_TIMER_TIMER_H
#define VOS_INCLUDE_TIMER_TIMER_H
...
#endif
```

You do not need to add a path as shown in the preceding example, but you need to ensure that the macro in the current project is unique.
```cpp
#ifndef TIMER_H
#define TIMER_H
...
#endif
```

### <a name="r5-2-1"></a>Recommendation 5.2.1 It is prohibited to reference external function interfaces and variables in declaration mode.
Interfaces provided by other modules or files can be used only by including header files.
Using external function interfaces and variables in extern declaration mode may cause inconsistency between declarations and definitions when external interfaces are changed.
In addition, this implicit dependency may cause architecture corruption.

Cases that do not comply with specifications:

// a.cpp content
```cpp
extern int Fun();   // Bad: Use external functions in extern mode.

void Bar() {
  int i = Fun();
  ...
}
```

// b.cpp content
```cpp
int Fun() {
  // Do something
}
```
It should be changed to:

// a.cpp content
```cpp
#include "b.h"   // Good: Use the interface provided by other .cpp by including its corresponding header file.

void Bar() {
  int i = Fun();
  ...
}
```

// b.h content
```cpp
int Fun();
```

// b.cpp content
```cpp
int Fun() {
  // Do something
}
```
In some scenarios, if the internal functions need to be referenced with no intrusion to the code, the extern declaration mode can be used.
For example:
When performing unit testing on an internal function, you can use the extern declaration to reference the function to be tested.
When a function needs to be stubbed or patched, the function can be declared using extern.

### <a name="r5-2-5"></a>Rule 5.2.5 Do not include header files in extern "C".
If a header file is included in extern "C", extern "C" may be nested. Some compilers restrict the nesting level of extern "C". If there are too many nested layers, compilation errors may occur.

When C and C++ programmings are used together and if extern "C" includes a header file, the original intent behind the header file may be hindered. For example, when the link specifications are modified incorrectly.

Example: Assume that there are two header files, a.h and b.h.

// a.h content
```cpp
...
#ifdef __cplusplus
void Foo(int);
#define A(value) Foo(value)
#else
void A(int)
#endif
```
// b.h content
```cpp
...
#ifdef __cplusplus
extern "C" {
#endif

#include "a.h"
void B();

#ifdef __cplusplus
}
#endif
```

Use the C++ preprocessor to expand b.h. The following information is displayed:
```cpp
extern "C" {
  void Foo(int);
  void B();
}
```

According to the author of a.h, the function Foo is a C++ free function following the "C++" link specification.
However, because `#include "a.h"` is placed inside `extern "C"` in b.h, the link specification of function Foo is changed incorrectly.

Exception:
In the C++ compilation environment, if you want to reference the header file of pure C, the C header files must not include `extern "C"`. The non-intrusive approach is to include the C header file in `extern "C"`.

### <a name="a5-2-2"></a>Recommendation 5.2.2 Use `#include` instead of a forward declaration to include header files.
A forward declaration is for the declaration of classes, functions, and templates and is not meant for its definition.

- Pros:
  1. Forward declarations can save compilation time. Unnecessary #includes force the compiler to open more files and process more input.
  2. Forward declarations can save unnecessary recompilation time. The use of #include will force your code to be recompiled for multiple times due to unrelated changes in header files.
- Cons:
  1. Forward declarations hide dependency relationship. When a header file is modified, user code will skip the necessary recompilation process.
  2. A forward declaration may be broken by subsequent changes to the library. Forward declarations of functions and templates sometimes prevent header file developers from changing APIs. For example, widening a formal parameter type, adding a formal template parameter with a default value, and so on.
  3. Forward declaration of symbols from the namespace `std::` is seen as undefined behavior (as specified in the C++ 11 standard specification).
  4. Forward declaration of multiple symbols from a header file can be more verbose than simply including (#include) the header.
  5. Structuring code only for forward declaration (for example, using pointer members instead of object members) can make the code more complex and slower.
  6. It is difficult to determine whether a forward declaration or `#include` is needed. In some scenarios, replacing `#include` with a forward declaration may cause unexpected results.

Therefore, we should avoid using forward declarations as much as possible. Instead, we use the #include statement to include a header file and ensure dependency.

### <a name="a5-2-3"></a>Recommendation 5.2.3 Include headers in the following sequence: .h file corresponding to the .cpp file > other header files according to their stability.
Using standard header file inclusion sequence can enhance readability and avoid hidden dependencies. The recommended header file inclusion priority is: the header file corresponding to the .cpp file > C/C++ standard libraries > .h files from used system libraries > .h files from other libraries > other .h files in the project.

For example, the sequence of the header files in Foo.cpp is as follows:
```cpp
#include "Foo/Foo.h"

#include <cstdlib>
#include <string>

#include <linux/list.h>
#include <linux/time.h>

#include "platform/Base.h"
#include "platform/Framework.h"

#include "project/public/Log.h"
```
Placing the Foo.h file at the top ensures that when the Foo.h file misses some necessary libraries or an error occurs, the Foo.cpp build is terminated immediately, reducing the compilation time. For the sequence of header files, refer to this suggestion.

Exception:
Platform-specific code requires conditional compilation. The code can be placed after other "includes".
```cpp
#include "foo/public/FooServer.h"

#include "base/Port.h"  // For LANG_CXX11.

#ifdef LANG_CXX11
#include <initializer_list>
#endif  // LANG_CXX11
```

# <a name="c6"></a>6 Scopes

## <a name="c6-1"></a>Namespaces
The content of a namespace is not indented.

### <a name="a6-1-1"></a>Recommendation 6.1.1 Use an unnamed namespace to encapsulate or use static to modify variables, constants, or functions that do not need to be exported from the .cpp file.
In the C++ 2003 standard, using static to modify the external availability of functions and variables was marked as deprecated. Therefore, unnamed namespaces are the recommended method.

The main reasons are as follows:
1. There are too many meanings for static in C++: static function member variable, static member function, static global variable, and static function local variable. Each of them has special processing.
2. Static can only be used to define variables, constants, and functions that are not referenced outside the current .cpp file, while namespaces can also be used to encapsulate types.
3. Use a namespace to process the scope of C++ instead of using both static and namespaces.
4. Unnamed namespaces rather than functions modified by static can be used to instantiate templates.

Do not use unnamed namespaces or static in header files.

```cpp
// Foo.cpp

namespace {
const int kMaxCount = 20;
void InternalFun(){};
}

void Foo::Fun() {
  int i = kMaxCount;
    
  InternalFun();
}

```

### <a name="r6-1-1"></a>Rule 6.1.1 Do not use "using" to import a namespace in header files or before #include statements.
Note: Using "using" to import a namespace will affect subsequent code and may cause symbol conflicts. Therefore, do not use "using" to import a namespace in a header file or before #include in a source file.
Example:

```cpp
// Header file a.h
namespace namespacea {
int Fun(int);
}
```

```cpp
// Header file b.h
namespace namespaceb {
int Fun(int);
}

using namespace namespaceb;

void G() {
  Fun(1); 
}
```

```cpp
// Source code a.cpp
#include "a.h"
using namespace namespacea; 
#include "b.h"

void main() {
  G(); // using namespace namespacea is before #include "b.h", which will cause the following issues: The calling of namespacea::Fun and namespaceb::Fun is not clear.
}
```

Using "using" to import a symbol or define an alias in a header file is allowed in customized namespaces of modules, but is prohibited in the global namespace.
```cpp
// foo.h

#include <fancy/string>
using fancy::string;  // Bad: It is prohibited to import symbols to global namespaces.

namespace foo {
using fancy::string;  // Good: Symbols can be imported in customized namespaces of modules.
using MyVector = fancy::vector<int>;  // Good: In C++11, aliases can be defined in customized namespaces.
}
```


### <a name="r6-1-2"></a>Rule 6.1.2 Do not use "using namespace std".
Note: The std:: prefix can make code clear and avoid naming conflicts.


## <a name="c6-2"></a>Global Functions and Static Member Functions

### <a name="a6-2-1"></a>Recommendation 6.2.1 Preferentially use namespaces to manage global functions. If global functions are closely related to a class, you can use static member functions.
Note: Placing non-member functions in a namespace avoids polluting the global scope. Do not use "class + static member function" to simply manage global functions. If a global function is closely tied to a class, it can be used as a static member function of the class.

If you need to define some global functions for a .cpp file, use unnamed namespaces for management.
```cpp
namespace mynamespace {
int Add(int a, int b);
}

class File {
 public:
  static File CreateTempFile(const std::string& fileName);
};
```

## <a name="c6-3"></a>Global Constants and Static Member Constants

### <a name="a6-3-1"></a>Recommendation 6.3.1 Preferentially use namespaces to manage global constants. If global constants are closely related to a class, you can use static member constants.
Note: Placing global constants in a namespace avoids polluting the global scope. Do not use "class + static member constant" to simply manage global constants. If a global constant is closely tied to a class, it can be used as a static member constant of the class.

If you need to define some global constants only for a .cpp file, use unnamed namespaces for management.
```cpp
namespace mynamespace {
const int kMaxSize = 100;
}

class File {
 public:
  static const std::string kName;
};
```

## <a name="c6-4"></a>Global Variables

### <a name="a6-4-1"></a>Recommendation 6.4.1 Do not use global variables. Use the singleton pattern instead.
Note: Global variables can be modified and read, which causes data coupling between the business code and the global variable.
```cpp
int counter = 0;

// a.cpp
counter++;

// b.cpp
counter++;

// c.cpp
cout << counter << endl;
```

Singleton
```cpp
class Counter {
 public:
  static Counter& GetInstance() {
    static Counter counter;
    return counter;
  }  // Simple example of a singleton implementation
    
  void Increase() {
    value++;
  }
    
  void Print() const {
    std::cout << value << std::endl;
  }

 private:
  Counter() : value(0) {}

 private:
  int value;
};

// a.cpp
Counter::GetInstance().Increase();

// b.cpp
Counter::GetInstance().Increase();

// c.cpp
Counter::GetInstance().Print();
```

After the singleton is implemented, there is a unique global instance, which can functions as a global variable. In addition, singleton provides better encapsulation.

Exception: In some cases, the scope of a global variable is only inside a module. Multiple instances of the same global variable may exist in the process space, and each module holds one copy. In this case, a singleton cannot be used as it is limited to one instance.

# <a name="c7"></a>7 Classes

Use a struct only for passive objects that carry data; everything else is a class.

## <a name="c7-1"></a>Constructors, Copy/Move Constructors, Copy/Move Assignment Operators, and Destructors
Constructors, copy/move constructors, copy/move assignment operators, and destructors provide lifetime management methods for objects.
- Constructor: `X()`
- Copy constructor: `X(const X&)`
- Copy assignment operator: `operator=(const X&)`   
- Move constructor: `X (X&&)`         *Provided in versions later than C++ 11*.
- Move assignment operator: `operator=(X&&)`       *Provided in versions later than C++ 11*.
- Destructor: `~X()`

### <a name="r7-1-1"></a>Rule 7.1.1 The member variables of a class must be initialized explicitly.
Note: If a class has member variables but no constructors and default constructors are defined, the compiler will automatically generate a constructor, which will not initialize member variables. The object status is uncertain.

Exception:
- If the member variables of a class have a default constructor, explicit initialization is not required.

Example: The following code has no constructor, and private data members cannot be initialized:
```cpp
class Message {
 public:
  void ProcessOutMsg() {
    //…
  }
 private:
  unsigned int msgID;
  unsigned int msgLength; 
  unsigned char* msgBuffer;
  std::string someIdentifier;
};

Message message;   // The message member variable is not initialized.
message.ProcessOutMsg();   // Potential risks exist in subsequent use.

// Therefore, it is necessary to define the default constructor as follows:
class Message {
 public:
  Message() : msgID(0), msgLength(0) {
  }
    
  void ProcessOutMsg() {
    // …
  }
    
 private:
  unsigned int msgID;
  unsigned int msgLength; 
  unsigned char* msgBuffer;
  std::string someIdentifier; // The member variable has a default constructor. Therefore, explicit initialization is not required.
};
```

### <a name="a7-1-1"></a>Recommendation 7.1.1 Initialization during declaration (C++ 11) and initialization using the constructor initialization list are preferred for member variables.
Note: Initialization during declaration (C++11) is preferred because initialized values of member variables can be easily understood. If initialized values of certain member variables are relevant to constructors, or C++ 11 is not supported, the constructor initialization list should be used preferentially to initialize these member variables. Compared with the assignment statements in constructors, code of the constructor initialization list is simpler and has higher performance, and can be used to initialize constant and reference members.

```cpp
class Message {
 public:
  Message() : msgLength(0) {  // Good: The constructor initialization list is preferred.
    msgBuffer = NULL;     // Bad: Values cannot be assigned in constructors.
  }
    
 private:
  unsigned int msgID{0};  // Good: used in C++11.
  unsigned int msgLength;
  unsigned char* msgBuffer;
};
```

### <a name="r7-1-2"></a>Rule 7.1.2 Declare single-parameter constructors as explicit to prevent implicit conversion.
Note: If a single-parameter constructor is not declared as explicit, it will become an implicit conversion function.
Example:

```cpp
class Foo {
 public:
  explicit Foo(const string& name): name(name) {
  }
 private:
  string name;
};


void ProcessFoo(const Foo& foo){}

int main(void) {
  std::string test = "test";
  ProcessFoo(test);  // Compiling failed.
  return 0;
}
```

The preceding code fails to be compiled because the parameter required by `ProcessFoo` is of the Foo type, which mismatch with the input string type.

If the explicit keyword of the Foo constructor is removed, implicit conversion is triggered and a temporary Foo object is generated when `ProcessFoo` is called with the string parameter. Usually, this implicit conversion is confusing and bugs are apt to be hidden, due to unexpected type conversion. Therefore, single-parameter constructors require explicit declaration.

### <a name="r7-1-3"></a>Rule 7.1.3 If copy/move constructors and copy/move assignment operators are not needed, clearly prohibit them.
Note: If users do not define it, the compiler will generate copy/move constructors and copy/move assignment operators (move semantic functions will be available in versions later than C++ 11).
If we do not use copy constructors or copy assignment operators, explicitly delete them.

1. Set copy constructors or copy assignment operators to private and do not implement them.

```cpp
class Foo {
 private:
  Foo(const Foo&);
  Foo& operator=(const Foo&);
};
```
2. Use delete provided by C++ 11.

```cpp
// Copy constructors and copy assignment operators are forbidden together. Use delete provided by C++ 11.
class Foo {
 public:
  Foo(Foo&&) = delete;
  Foo& operator=(Foo&&) = delete;
};
```
3. For static method class, disable constructors to prevent instances from being created.

```cpp
class Helper {
 public:
  static bool DoSomething();

 private:
  Helper();
};
```
4. For singleton class, disable constructors and copy constructors to prevent instances from being created.

```cpp
class Foo {
 private:
  static Foo *instance;
  Foo() {}
  Foo(const Foo &a);
  Foo& operator=(const Foo &a);
 public:
  static Foo &Instance() {
    if (!instance) {
      instance = new Foo();
    }
    return *instance;
  }
};
```

5. For destructors that release resources by raw pointers, disable copy constructions and copy assignment operators to prevent repeated release.

```cpp
class Foo {
 private:
  FILE *fp;
  Foo(const Foo &a);
  Foo& operator=(const Foo &a);
 public:
  Foo() : fp(nullptr) {}
  ~Foo() {
    if (fp != nullptr) {
      fclose(fp);
      fp = nullptr;
    }
  }
};

Foo* Foo::instance = nullptr;
```

### <a name="r7-1-4"></a>Rule 7.1.4 Copy constructors and copy assignment operators should be implemented or forbidden together.
Both copy constructors and copy assignment operators provide copy semantics. They should be implemented or forbidden together.

```cpp
// Copy constructors and copy assignment operators are implemented together.
class Foo {
 public:
  ...
  Foo(const Foo&);
  Foo& operator=(const Foo&);
  ...
};

// Copy constructors and copy assignment operators are both set to default, as supported by C++ 11.
class Foo {
 public:
  Foo(const Foo&) = default;
  Foo& operator=(const Foo&) = default;
};

// Copy constructors and copy assignment operators are forbidden together. You can use delete provided by C++ 11.
class Foo {
 private:
  Foo(const Foo&);
  Foo& operator=(const Foo&);
};
```

### <a name="r7-1-5"></a>Rule 7.1.5 Move constructors and move assignment operators should be implemented or forbidden together.
The move operation is added in C++ 11. If a class is required to support the move operation, move constructors and move assignment operators need to be implemented.

Both move constructors and move assignment operators provide move semantics. They should be implemented or forbidden together.
```cpp
// Move constructors and move assignment operators are implemented together.
class Foo {
 public:
  ...
  Foo(Foo&&);
  Foo& operator=(Foo&&);
  ...
};

// Move constructors and move assignment operators are both set to default, as supported by C++ 11.
class Foo {
 public:
  Foo(Foo&&) = default;
  Foo& operator=(Foo&&) = default;
};

// Move constructors and move assignment operators are forbidden together. Use delete provided by C++ 11.
class Foo {
 public:
  Foo(Foo&&) = delete;
  Foo& operator=(Foo&&) = delete;
};
```

### <a name="r7-1-6"></a>Rule 7.1.6 It is prohibited to call virtual functions in constructors and destructors.
Note: Calling a virtual function of the current object in a constructor or destructor will cause behavior of non-polymorphism.
In C++, a base class constructs only one complete object at a time.

Example: Base indicates the base class, and Sub indicates the derived class.
```cpp
class Base {                       
 public:                
  Base();
  virtual void Log() = 0;    // Different derived classes call different log files.
};

Base::Base() {        // Base class constructor
  Log();              // Call the virtual function log.
}                                                  

class Sub : public Base {       
 public:
  virtual void Log();          
};
```

When running the following statement:
`Sub sub;`
The constructor of the derived class is executed first. However, the constructor of the base class is called first. Because the constructor of the base class calls the virtual function log, the log is in the base class version. The derived class is constructed only after the base class is constructed. As a result, behavior of non-polymorphism are caused.
This also applies to destructors.

### <a name="a7-1-2"></a>Recommendation 7.1.2 Do not add the inline keyword to functions in the class definition.
Note: By default, functions in the class definition are inline.


## <a name="c7-2"></a>Inheritance

### <a name="r7-2-1"></a>Rule 7.2.1 Destructors of the base class should be declared as virtual.
Note: Destructors of the derived class can be called during polymorphism invocation only when destructors of the base class are virtual.

Example: There will be memory leak if destructors of the base class are not declared as virtual.
```cpp
class Base {
 public:
  virtual std::string getVersion() = 0;
    
  ~Base() {
    std::cout << "~Base" << std::endl;
  }
};
```

```cpp
class Sub : public Base {
 public:
  Sub() : numbers(nullptr) {  
  }
    
  ~Sub() {
    delete[] numbers;
    std::cout << "~Sub" << std::endl;
  }
    
  int Init() {
    const size_t numberCount = 100;
    numbers = new (std::nothrow) int[numberCount];
    if (numbers == nullptr) {
      return -1;
    }
        
    ...
  }

  std::string getVersion() { 
    return std::string("hello!");
  }
private:
  int* numbers;
};
```

```cpp
int main(int argc, char* args[]) {
  Base* b = new Sub();

  delete b;
  return 0;
}
```
Because destructors of the base class are not declared as virtual, only destructors of the base class are called when an object is destroyed. Destructors of the derived class Sub are not called. As a result, a memory leak occurs.


### <a name="r7-2-2"></a>Rule 7.2.2 Do not use default parameter values for virtual functions.
Note: In C++, virtual functions are dynamically bound, but the default parameters of functions are statically bound during compilation. This means that the function you finally execute is a virtual function that is defined in the derived class but uses the default parameter value in the base class. To avoid confusion and other problems caused by inconsistent default parameter declarations during overriding of virtual functions, it is prohibited to declare default parameter values for all virtual functions.
Example: The default value of parameter "text" of the virtual function "Display" is determined at compilation time instead of runtime, which does not fit with polymorphism.
```cpp
class Base {
 public:
  virtual void Display(const std::string& text = "Base!") {
    std::cout << text << std::endl;
  }
    
  virtual ~Base(){}
};

class Sub : public Base {
 public:
  virtual void Display(const std::string& text  = "Sub!") {
    std::cout << text << std::endl; 
  }
    
  virtual ~Sub(){}
};

int main() {
  Base* base = new Sub();
  Sub* sub = new Sub();
   
  ...
    
  base->Display();  // The program output is as follows: Base! The expected output is as follows: Sub!
  sub->Display();   // The program output is as follows: Sub!
    
  delete base;
  delete sub;
  return 0;
};
```

### <a name="r7-2-3"></a>Rule 7.2.3 Do not redefine inherited non-virtual functions.
Note: Non-virtual functions cannot be dynamically bound (only virtual functions can be dynamically bound). You can obtain the correct result by operating the pointer of the base class.

Example:
```cpp
class Base {
 public:
  void Fun();
};

class Sub : public Base { 
 public:
  void Fun();
};

Sub* sub = new Sub();                     
Base* base = sub;

sub->Fun();    // Call Fun of the derived class.                  
base->Fun();   // Call Fun of the base class.
//...

```

## <a name="c7-3"></a>Multiple Inheritance
In the actual development process, multiple inheritance scenarios are seldom used because the following typical problems may occur:
1. Data duplication and name ambiguity caused by "diamond" inheritance. Therefore, C++ introduces virtual inheritance to solve these problems.
2. In addition to "diamond" inheritance, names of multiple base classes may also conflict with each other, resulting in name ambiguity.
3. If a derived class needs to be extended or needs to override methods of multiple base classes, the responsibilities of the derived classes are unclear and semantics are muddled.
4. Compared with delegation, inheritance is seen as white box reuse, that is, a derived class can access the protected members of the base class, which leads to more coupling. Multiple inheritance, due to the coupling of multiple base classes, leads to even more coupling.

Multiple inheritance has the following advantages:
Multiple inheritance provides a simpler method for assembling and reusing multiple interfaces or classes.

Therefore, multiple inheritance can be used only in the following cases:

### <a name="a7-3-1"></a>Recommendation 7.3.1 Use multiple inheritance to implement interface separation and multi-role combination.
If a class requires multiple interfaces, combine multiple separated interfaces by using multiple inheritance. This is similar to the Traits mixin of the Scala language.

```cpp
class Role1 {};
class Role2 {};
class Role3 {};

class Object1 : public Role1, public Role2 {
  // ...
};

class Object2 : public Role2, public Role3 {
  // ...
};

```

The C++ standard library has a similar implementation example:
```cpp
class basic_istream {};
class basic_ostream {};

class basic_iostream : public basic_istream, public basic_ostream {
  
};
```

## <a name="c7-4"></a>Overloading

Overload operators should be used when there are sufficient reasons, and they do not change the original perception of the operators. For example, do not use the plus sign (+) to perform subtraction.
Operator overloading can make code more intuitive but has some disadvantages:
- It is often mistaken that the operation is as fast as a built-in operator, which has no performance degradation.
- There is no naming to aid debugging. It is more convenient to search by function name than by operator.
- Overloading operators can cause confusion if behavior definitions are not intuitive (for example, if the "+" operator is used for subtraction).
- The implicit conversion caused by the overloading of assignment operators may lead to entrenched bugs. Functions such as Equals () and CopyFrom () can be defined to replace the = and == operators.



# <a name="c8"></a> 8 Functions
## <a name="c8-1"></a>Function Design
### <a name="r8-1-1"></a>Recommendation 8.1.1 Avoid long functions and ensure that each function contains no more than 50 lines (non-null and non-comment).
A function should be displayed on one screen (no longer than 50 lines). It should do only one thing, and do it well.

Long functions often mean that the functions are too complex to implement more than one function, or overly detailed but not further abstracted.

Exception: Some implementation algorithm functions may be longer than 50 lines due to algorithm convergence and functional comprehensiveness.

Even if a long function works very well now, once someone modifies it, new problems may occur, even causing bugs that are difficult to discover.
It is recommended that you split a long function into several functions that are simpler and easier to manage, facilitating code comprehension and modification.

## <a name="c8-2"></a>Inline Functions

###  <a name="a8-2-1"></a>Recommendation 8.2.1 An inline function cannot exceed 10 lines.
**Note**: An inline function has the same characteristics of a normal function. The difference between an inline function and a normal function lies in the processing of function calls. When a general function is called, the program execution right is transferred to the called function, and then returned to the function that calls it. When an inline function is called, the invocation expression is replaced with an inline function body.

Inline functions are only suitable for small functions with only 1-10 lines. For a large function that contains many statements, the function call and return overheads are relatively trivial and do not need to be implemented by an inline function. Most compilers may abandon the inline mode and use the common method to call the function.

If an inline function contains complex control structures, such as loop, branch (switch), and try-catch statements, the compiler may regard the function as a common function.
**Virtual functions and recursive functions cannot be used as inline functions**.

## <a name="c8-3"></a>Function Parameters

### <a name="a8-3-1"></a>Recommendation 8.3.1 Use a reference instead of a pointer for function parameters.

**Note**: A reference is more secure than a pointer because it is not empty and does not point to other targets. Using a reference stops the need to check for illegal null pointers.

Use const to avoid parameter modification, so that readers can clearly know that a parameter is not going to be modified. This greatly enhances code readability.

### <a name="a8-3-2"></a>Recommendation 8.3.2 Use strongly typed parameters. Do not use void*.
While different languages have their own views on strong typing and weak typing, it is generally believed that C/C++ is a strongly typed language. Since we use such a strongly typed language, we should keep this style.
An advantage of this is the compiler can find type mismatch problems at the compilation stage.

Using strong typing helps the compiler find more errors for us. Pay attention to the usage of the FooListAddNode function in the following code:
```cpp
struct FooNode {
  struct List link;
  int foo;
};

struct BarNode {
  struct List link;
  int bar;
}

void FooListAddNode(void *node) { // Bad: Here, the void * type is used to transfer parameters.
  FooNode *foo = (FooNode *)node;
  ListAppend(&fooList, &foo->link);
}

void MakeTheList() {
  FooNode *foo = nullptr;
  BarNode *bar = nullptr;
  ...

  FooListAddNode(bar);        // Wrong: In this example, the foo parameter was supposed to be transferred, but the bar parameter is accidentally transferred instead. However, no error is reported.
}
```

1. You can use the template function to change the parameter type.
2. A base class pointer can be used to implement polymorphism.

### <a name="a8-3-3"></a>Recommendation 8.3.3 A function can have a maximum of five parameters.
If a function has too many parameters, it is apt to be affected by external changes, and therefore maintenance is affected. Too many parameters will also increase the testing workload.

If a function has more than five parameters, you can:
- Split the function.
- Combine related parameters into a struct.

# <a name="c9"></a> 9 Other C++ Features

## <a name="c9-1"></a>Constants and Initialization

Unchanged values are easier to understand, trace, and analyze. Therefore, use constants instead of variables as much as possible. When defining values, use const as a default.

### <a name="r9-1-1"></a>Recommendation 9.1.1 Do not use macros to replace constants.

**Note**: Macros are a simple text replacement that is completed in the preprocessing phase. When an error is reported, the corresponding value is reported. During tracing and debugging, the value is also displayed instead of the macro name. A macro does not support type checking and is insecure. A macro has no scope.

```cpp
#define MAX_MSISDN_LEN 20    // Bad

// Use the const constant in C++.
const int kMaxMsisdnLen = 20; // Good

// In versions later than C++ 11, constexpr can be used.
constexpr int kMaxMsisdnLen = 20;
```

###  <a name="a9-1-2"></a>Recommendation 9.1.2 A group of related integer constants must be defined as an enumeration.

**Note**: Enumerations are more secure than `#define` or `const int`. The compiler checks whether a parameter value is within the enumerated value range to avoid errors.

```cpp
// Good example:
enum Week {
  kSunday,
  kMonday,
  kTuesday,
  kWednesday,
  kThursday,
  kFriday,
  kSaturday
};

enum Color {
  kRed,
  kBlack,
  kBlue
};

void ColorizeCalendar(Week today, Color color);

ColorizeCalendar(kBlue, kSunday); // Compilation error. The parameter type is incorrect.

// Bad example:
const int kSunday = 0;
const int kMonday = 1;

const int kRed = 0;
const int kBlack = 1;

bool ColorizeCalendar(int today, int color);
ColorizeCalendar(kBlue, kSunday); // No error is reported.
```

When an enumeration value needs to correspond to a specific value, explicit value assignment is required during declaration. Otherwise, do not assign explicit values. This will prevent repeated assignment and reduce the maintenance workload (when adding and deleting members).

```cpp
// Good example: Device ID defined in the S protocol. It is used to identify a device type.
enum DeviceType {
  kUnknown = -1,
  kDsmp = 0,
  kIsmg = 1,
  kWapportal = 2
};
```



### <a name="r9-1-3"></a>Recommendation 9.1.3 Magic numbers cannot be used.
So-called magic numbers are the numbers that are unintelligible and difficult to understand.

Some numbers can be understood based on context.
For example, you may understand the number 12 in certain contexts.
type = 12; is not intelligible (and a magic number), but `month = year * 12`; can be understood, so we wouldn't really class this as a magic number.
The number 0 is often seen as a magic number. For example, `status = 0`; cannot truly express any status information.

Solution:
Comments can be added for numbers that are used locally.
For the numbers that are used multiple times, you must define them as constants and give them descriptive names.

The following cases are forbidden:
No symbol is used to explain the meaning of a number, for example, `const int kZero = 0`.
The symbol name limits the value. For example, in`const int kXxTimerInterval = 300`, Use `kXxTimerInterval` instead.

### <a name="r9-1-1"></a>Rule 9.1.1 Ensure that a constant has only one responsibility.

**Note**: A constant is used for only a specific function, that is, a constant cannot be used for multiple purposes.

```cpp
// Good example: For protocol A and protocol B, the length of the MSISDN is 20.
const unsigned int kAMaxMsisdnLen = 20;
const unsigned int kBMaxMsisdnLen = 20;

// Or use different namespaces:
namespace namespace1 {
const unsigned int kMaxMsisdnLen = 20;
}

namespace namespace2 {
const unsigned int kMaxMsisdnLen = 20;
}
```

### <a name="r9-1-4"></a>Recommendation 9.1.4 Do not use memcpy_s or memset_s to initialize non-POD objects.

**Note**: `POD` is short for `Plain Old Data`, which is a concept introduced in the C++ 98 standard (ISO/IEC 14882, first edition, 1998-09-01). The `POD` types include the original types and aggregate types such as `int`, `char`, `float`, `double`, `enumeration`, `void`, and pointer. Encapsulation and object-oriented features cannot be used (for example, user-defined constructors, assignment operators, destructors, base classes, and virtual functions).

For non-POD classes, such as class objects of non-aggregate types, virtual functions may exist. Memory layout is uncertain, and is related to the compiler. Misuse of memory copies may cause serious problems.

Even if a class of the aggregate type is directly copied and compared, and any functions hiding information or protecting data are destroyed, the `memcpy_s` and `memset_s` operations are not recommended.

For details about the POD type, see the appendix.


## <a name="c9-2"></a>Expressions

### <a name="r9-2-2"></a>Rule 9.2.1 A switch statement must have a default branch.
In most cases, a switch statement requires a default branch to ensure that there is a default action when the case tag is missing.

Exception:
If the switch condition variables are enumerated and the case branch covers all values, the default branch is redundant.
Modern compilers can check which case branches are missing in the switch statement and provide an advanced warning. 

```cpp
enum Color {
  kRed = 0,
  kBlue
};

// The switch condition variables are enumerated. Therefore, you do not need to add a default branch.
switch (color) {
  case kRed:
    DoRedThing();
    break;
  case kBlue:
    DoBlueThing();
    ...
    break;
}
```

### <a name="a9-2-1"></a>Recommendation 9.2.1 When comparing expressions, follow the principle that the left side tends to change and the right side tends to remain unchanged.
When a variable is compared with a constant, placing the constant on the left, for example, if (MAX == v), does not comply with reading habits and if (MAX > v) is more difficult to understand.
The constant should be placed on the right according to common reading and expression conventions. The expression is written as follows:
```cpp
if (value == MAX) {
  
}

if (value < MAX) {
  
}
```
There are special cases: for example, if the expression `if (MIN < value && value < MAX)` is used to describe a range, the first half, as a constant, should be placed on the left.

You do not need to worry about writing '==' as '=' because a compilation alarm will be generated for `if (value = MAX)` and an error will be reported by other static check tools. Use the tool to solve such writing errors and ensure that the code must be readable.


## <a name="c9-3"></a>Type Casting

Do not use type branches to customize behavior. Type branch customization behavior is prone to errors and is an obvious sign of attempting to compile C code using C++. This is very inflexible technology. If you forget to modify all branches when adding a new type to a compiler, you will not be notified. Use templates and virtual functions to let the type define itself rather than letting the calling side determine behavior.

It is recommended that type casting be avoided. We should consider the data type of each type of data in the code design instead of overusing type casting to solve type conflicts. When designing a basic type, consider the following:
- Whether it is unsigned or signed?
- Is it suitable for float or double?
- Should you use int8, int16, int32, or int64 bit lengths?

However, we cannot prohibit the use of type casting because the C++ language is a machine-oriented programming language, involving pointer addresses, and we interact with various third-party or underlying APIs. Their type design may not be reasonable and type casting tends to occur in the adaptation process.

Exception: When calling a function, if we do not want to process the result of the function, first consider whether this is your best choice. If you do not want to process the return value of the function, cast it to void.

### <a name="r9-3-1"></a>Rule 9.3.1 If type casting is required, use the type casting provided by the C++ instead of the C style.

**Note**:

The type casting provided by C++ is more targeted, easy to read, and more secure than the C style. C++ provides the following types of casting:
- Type casting:
1. `dynamic_cast`: It is used to inherit the downstream transformation of the system. `dynamic_cast` has the type check function. Design the base class and derived class to avoid using dynamic_cast for casting.
2. `static_cast`: It is similar to the C style casting, which can be used to convert a value, or to convert the pointer or reference of a derived class into a base class pointer or reference. This casting is often used to eliminate type ambiguity brought on by multiple inheritance, which is relatively safe. If it is a pure arithmetic conversion, use the braces as stated in the following text.
3. `reinterpret_cast`: It is used to convert irrelevant types. `reinterpret_cast` forces the compiler to reinterpret the memory of a certain type of objects into another type, which is an unsafe conversion. It is recommended that `reinterpret_cast` be used as little as possible.
4. `const_cast`: It is used to remove the `const` attribute of an object so that the object can be modified. It is recommended that `const_cast` be used as little as possible.

- Arithmetic conversion: (Supported by C++ 11 and later versions)
  If the type information is not lost, for example, the casting from float to double, or from int32 to int64, the initial mode of braces is recommended.
```cpp
  double d{ someFloat };
  int64_t i{ someInt32 };
```

### <a name="a9-3-1"></a>Recommendation 9.3.1 Avoid using `dynamic_cast`.
1. `dynamic_cast` depends on the RTTI of C++ so that the programmer can identify the type of the object in C++ at run time.
2. `dynamic_cast` indicates that a problem occurs in the design of the base class and derived class. The derived class destroys the contract of the base class and it is necessary to use `dynamic_cast` to convert the class to a subclass for special processing. In this case, it is more desirable to improve the design of the class, instead of using `dynamic_cast` to solve the problem.

### <a name="a9-3-2"></a>Recommendation 9.3.2 Avoid using `reinterpret_cast`.

**Note**: `reinterpret_cast` is used to convert irrelevant types. Trying to use `reinterpret_cast` to force a type to another type destroys the security and reliability of the type and is an insecure casting method. Avoid casting between different types.

### <a name="a9-3-3"></a>Recommendation 9.3.3 Avoid using `const_cast`.

**Note**: The `const_cast` command is used to remove the `const` and `volatile` properties of an object.

The action of using a pointer or reference after the const_cast conversion to modify the const property of an object is undefined.

```cpp
// Bad example:
const int i = 1024; 
int* p = const_cast<int*>(&i);
*p = 2048;      // The action is undefined.
```

```cpp
// Bad example:
class Foo {
 public:
  Foo() : i(3) {}

  void Fun(int v) {
    i = v;
  }

 private:
  int i;
};

int main(void) {
  const Foo f;
  Foo* p = const_cast<Foo*>(&f);
  p->Fun(8);  // The action is undefined.
}

```


## <a name="c9-4"></a>Resource Allocation and Release

### <a name="r9-4-1"></a>Rule 9.4.1 When a single object is released, delete is used. When an array object is released, delete [] is used.
Note: Delete is used to delete a single object, and delete [] is used to delete an array object. Reason:

- new: Apply for memory from the system and call the corresponding constructor to initialize an object.
- new[n]: Apply for memory for n objects and call the constructor n times for each object to initialize them.
- delete: Call the corresponding destructor first and release the memory of an object.
- delete[]: Call the corresponding destructor for each object and release their memory.

If the usage of new and delete does not match this format, the results are unknown. For a non-class type, new and delete will not call the constructor or destructor.

The incorrect format is as follows:
```cpp
const int KMaxArraySize = 100;
int* numberArray = new int[KMaxArraySize];
...
delete numberArray;
numberArray = NULL;
```

The correct format is as follows:
```cpp
const int KMaxArraySize = 100;
int* numberArray = new int[KMaxArraySize];
...
delete[] numberArray;
numberArray = NULL;
```

## <a name="c9-5"></a>Standard Template Library

The standard template library (STL) varies between modules. The following table lists some basic rules and suggestions.

### <a name="r9-5-1"></a>Rule 9.5.1 Do not save the pointer returned by c_str () of std::string.

Note: The C++ standard does not specify that the string::c_str () pointer is permanently valid. Therefore, the STL implementation used can return a temporary storage area and release it quickly when calling string::c_str (). Therefore, to ensure the portability of the program, do not save the result of string::c_str (). Instead, call it directly.

Example:

```cpp
void Fun1() {
  std::string name = "demo";
  const char* text = name.c_str(); // After the expression ends, the life cycle of name is still in use and the pointer is valid.

  // If a non-const member function (such as operator[] and begin()) of the string type is invoked and the string is therefore modified,
  // the text content may become unavailable or may not be the original character string.
  name = "test";
  name[1] = '2';

  // When the text pointer is used next time, the string is no longer "demo".
}

void Fun2() {
  std::string name = "demo";
  std::string test = "test";
  const char* text = (name + test).c_str(); // After the expression ends, the temporary object generated by the + operator may be destroyed, and the pointer may be invalid.

    // When the text pointer is used next time, it no longer points to the valid memory space.
}
```
Exception: In rare cases where high performance coding is required , you can temporarily save the pointer returned by string::c_str() to match the existing functions which support only the input parameters of the const char* type. However, you should ensure that the life cycle of the string object is longer than that of the saved pointer, and that the string object is not modified within the life cycle of the saved pointer.


### <a name="a9-5-1"></a>Recommendation 9.5.1 Use std::string instead of char*.

Note: Using string instead of `char*` has the following advantages:
1. There is no need to consider the null character '\0' at the end.
2. You can directly use operators such as +, =, and ==, and other character string operation functions.
3. No need to consider memory allocation operations. This helps avoid explicit usage of new and delete and the resulting errors.

Note that in some STL implementations, string is based on the copy-on-write policy, which causes two problems. One is that the copy-on-write policy of some versions does not implement thread security, and the program breaks down in multi-threaded environments. Second, dangling pointers may be caused when a dynamic link library transfers the string based on the copy-on-write policy, due to the fact that reference count cannot be reduced when the library is unloaded. Therefore, it is important to select a reliable STL implementation to ensure the stability of the program.

Exceptions:
When an API of a system or other third-party libraries is called, only `char*` can be used for defined interfaces. However, before calling the interfaces, you can use string. When calling the interfaces, you can use string::c_str () to obtain the character pointer.
When a character array is allocated as a buffer on the stack, you can directly define the character array without using string or containers such as `vector<char>`.

### <a name="r9-5-2"></a>Rule 9.5.2 Do not use auto_ptr.
Note: The std::auto_ptr in the STL library has an implicit ownership transfer behavior. The code is as follows:
```cpp
auto_ptr<T> p1(new T);
auto_ptr<T> p2 = p1;
```
After the second line of statements is executed, p1 does not point to the object allocated in line 1 and becomes NULL. Therefore, auto_ptr cannot be placed in any standard containers.
This ownership transfer behavior is not expected. In scenarios where ownership must be transferred, implicit transfer should not be used. This often requires the programmer to keep extra attention on code that uses auto_ptr , otherwise access to a null pointer will occur.
There are two common scenarios for using auto_ptr . One is to transfer it as a smart pointer to outside of the function that generates the auto_ptr , and the other is to use auto_ptr as the RAII management class. Resources are automatically released when the lifecycle of auto_ptr expires.
In the first scenario, you can use std::shared_ptr instead.
In the second scenario, you can use std::unique_ptr in the C++ 11 standard. std::unique_ptr is a substitute for std::auto_ptr and supports explicit ownership transfer.

Exceptions:
Before the C++ 11 standard is widely used, std::auto_ptr can be used in scenarios where ownership needs to be transferred. However, it is recommended that std::auto_ptr be encapsulated. The copy constructor and assignment operator of the encapsulation class should not be used, so that the encapsulation class cannot be used in a standard container.


### <a name="a9-5-2"></a>Recommendation 9.5.2 Use the new standard header files.

Note:
When using the standard header file of C++, use `<cstdlib>` instead of `<stdlib.h>`.

## <a name="c9-6"></a>Usage of const
Add the keyword const before the declared variable or parameter (example: `const int foo`) to prevent the variable from being tampered with. Add the const qualifier to the function in the class (example: `class Foo {int Bar (char c) const;} ;`) to make sure the function does not modify the status of the class member variable. const variables, data members, functions, and parameters ensure that the type detection during compilation is accurate and errors are found as soon as possible. Therefore, we strongly recommend that const be used in any possible case.
Sometimes it is better to use constexpr of C++ 11 to define real constants.

### <a name="r9-6-1"></a>Rule 9.6.1 For formal parameters of pointer and reference types, if the parameters do not need to be modified, use const.
Unchanged values are easier to understand, trace, and analyze. const is used as the default option and is checked during compilation to make the code more secure and reliable.
```cpp
class Foo;

void PrintFoo(const Foo& foo);
```

### <a name="r9-6-2"></a>Rule 9.6.2 For member functions that do not modify member variables, use const.
Declare the member function as const whenever possible. The access function should always be const. So long as the function of a member is not modified, the function is declared with const.

```cpp
class Foo {
 public:

  // ...

  int PrintValue() const // const’s usage here modifies member functions and does not modify member variables.
    std::cout << value << std::endl;
  }

  int GetValue() const // and again here.
    return value;
  }

 private:
  int value;
};
```

### <a name="a9-6-1"></a>Recommendation 9.6.1 Member variables that will not be modified after initialization should be defined as constants.

```cpp
class Foo {
 public:
  Foo(int length) : dataLength(length) {}
 private:
  const int dataLength;  
};
```

## <a name="c9-7"></a>Templates

Template programming allows for extremely flexible interfaces that are type safe and high performance, enabling reuse of code of different types but with the same behavior.

The disadvantages of template programming are as follows:

1. The techniques used in template programming are often obscure to anyone but language experts. Code that uses templates in complicated ways is often unreadable, and is hard to debug or maintain.
2. Template programming often leads to extremely poor compiler time error messages: even if an interface is simple, complicated implementation details become visible when the user does something wrong.
3. If the template is not properly used, the code will be over expanded during runtime.
4. It is difficult to modify or refactor the template code. The template code is expanded in multiple contexts, and it is hard to verify that the transformation makes sense in all of them.

Therefore, it is recommended that __template programming be used only in a small number of basic components and basic data structure__. When using the template programming, minimize the __complexity as much as possible__, and __avoid exposing the template__. It is better to hide programming as an implementation detail whenever possible, so that user-facing headers are readable. And you should write sufficiently detailed comments for code that uses templates.


## <a name="c9-8"></a>Macros
In the C++ language, it is strongly recommended that complex macros be used as little as possible.
- For constant definitions, use `const` or `enum` as stated in the preceding sections. 
- For macro functions, try to be as simple as possible, comply with the following principles, and use inline functions and template functions for replacement.

```cpp
// The macro function is not recommended.
#define SQUARE(a, b) ((a) * (b)) 

// Use the template function and inline function as a replacement.
template<typename T> T Square(T a, T b) { return a * b; }
```

For details about how to use macros, see the related chapters about the C language specifications.
**Exception**: For some common and mature applications, for example, encapsulation for new and delete, the use of macros can be retained.


## <a name="c9-9"></a>Others

### <a name="a9-9-1"></a>Recommendation 9.9.1 Use '\n' instead of std::endl when exporting objects to a file.
Note: std::endl flushes content in the buffer to a file, which may affect the performance.

# <a name="c10"></a>10 Modern C++ Features

As the ISO released the C++ 11 language standard in 2011 and released the C++ 17 in March 2017, the modern C++ (C++ 11/14/17) adds a large number of new language features and standard libraries that improve programming efficiency and code quality.
This chapter describes some guidelines for modern C++ use, to avoid language pitfalls.

## <a name="c10-1"></a>Code Simplicity and Security Improvement
### <a name="a10-1-1"></a>Recommendation 10.1.1 Use `auto` properly.


* `auto` can help you avoid writing verbose, repeated type names, and can also ensure initialization when variables are defined.
* The `auto` type deduction rules are complex and need to be read carefully.
* If using `auto` makes the code clearer, use a specific type of it and use it only for local variables.

**Example**

```cpp
// Avoid verbose type names.
std::map<string, int>::iterator iter = m.find(val);
auto iter = m.find(val);

// Avoid duplicate type names.
class Foo {...};
Foo* p = new Foo;
auto p = new Foo;

// Ensure that the initialization is successful.
int x;    // The compilation is correct but the variable is not initialized.
auto x;   // The compilation failed. Initialization is needed.
```

`auto` type deduction may cause the following problems:

```cpp
auto a = 3;           // int
const auto ca = a;    // const int
const auto& ra = a;   // const int&
auto aa = ca;         // int, const and reference are neglected.
auto ila1 = { 10 };   // std::initializer_list<int>
auto ila2{ 10 };      // std::initializer_list<int>

auto&& ura1 = x;      // int&
auto&& ura2 = ca;     // const int&
auto&& ura3 = 10;     // int&&

const int b[10];
auto arr1 = b;        // const int*
auto& arr2 = b;       // const int(&)[10]
```

If you do not pay attention to `auto` type deduction and ignore the reference, hard-to-find performance problems may be created.

```cpp
std::vector<std::string> v;
auto s1 = v[0]; // auto deduction changes s1 to std::string in order to copy v[0].
```

If the `auto` is used to define an interface, such as a constant in the header file, it may be possible that the type has changed because the developer has modified the value.

In a loop, consider using auto & and auto * to traverse complex objects to improve performance.

```cpp
for (auto &stmt : bb->GetStmtNodes()) {
...
}
```

### <a name="r10-1-1"></a>Rule 10.1.1 Use the keyword `override` when rewriting virtual functions.

The keyword `override` ensures that the function is a virtual function and an overridden virtual function of the base class. If the subclass function is different from the base class function prototype, a compilation alarm is generated.

If you modify the prototype of a base class virtual function but forget to modify the virtual function overridden by the subclass, you can find inconsistency during compilation. You can also avoid forgetting to modify the overridden function when there are multiple subclasses.

**Example**

```cpp
class Base {
 public:
  virtual void Foo();
  void Bar();
};

class Derived : public Base {
 public:
  void Foo() const override; // Compilation failed: derived::Foo is different from that of the prototype of base::Foo and is not overridden.
  void Foo() override;       // Compilation successful: derived::Foo overrode base::Foo.
  void Bar() override;       // Compilation failed: base::Bar is not a virtual function.
};
```

**Summary**
1. When defining the virtual function for the first time based on the base class, use the keyword `virtual`.
2. When the subclass overrides the base class’ virtual function, use the keyword `virtual`.
3. For the non-virtual function, do not use `virtual` or `override`.

### <a name="r10-1-2"></a>Rule: 10.1.2 Use the keyword delete to `delete` functions.

The `delete` keyword is clearer and the application scope is wider than a class member function that is declared as private and not implemented.

**Example**

```cpp
class Foo {
 private:
  // Whether the copy structure is deleted or not is unknown because usually only the header file is checked.
  Foo(const Foo&);
};

class Foo {
 public:
  // Explicitly delete the copy assignment operator.
  Foo& operator=(const Foo&) = delete;
};
```

The `delete` keyword can also be used to delete non-member functions.

```cpp
template<typename T>
void Process(T value);

template<>
void Process<void>(void) = delete;
```

### <a name="r10-1-3"></a>Rule 10.1.3 Use `nullptr` instead of `NULL` or `0`.

For a long time, C++ has not had a keyword that represents a null pointer, which is embarrassing:

```cpp
#define NULL ((void *)0)

char* str = NULL;   // Error: void* cannot be automatically converted to char*.

void(C::*pmf)() = &C::Func;
if (pmf == NULL) {} // Error: void* cannot be automatically converted to the pointer that points to the member function.
```

If `NULL` is defined as `0` or `0L`, the above problems can be solved.

Alternatively, use `0` directly in places where null pointers are required. However, another problem occurs. The code is not clear, especially when the `auto` is used for automatic deduction.

```cpp
auto result = Find(id);
if (result == 0) {  // Does Find() return a pointer or an integer?
  // do something
}
```

Literally `0` is of the `int` type (`0L` is the `long` type). Therefore, neither `NULL` nor `0` is a pointer type.
When a function of the pointer or integer type is overloaded, `NULL` or `0` calls only the overloaded pointer function.

```cpp
void F(int);
void F(int*);

F(0);      // Call F(int) instead of F(int*).
F(NULL);   // Call F(int) instead of F(int*).
```

In addition, `sizeof(NULL) == sizeof(void*)` does not always make sense, which is a potential risk.

Summary: If `0` or `0L` is directly used, the code is not clear and type security cannot be ensured. If `NULL` is used, the type security cannot be ensured. These are all potential risks.

`nullptr` has many advantages. It literally represents the null pointer and makes the code clearer. More to the point, it is no longer an integer type.

`nullptr` is of the `std::nullptr_t` type. `std::nullptr_t` can be implicitly converted into all original pointer types, so that `nullptr` can represent a null pointer that points to any type.

```cpp
void F(int);
void F(int*);
F(nullptr);   // Call F(int*).

auto result = Find(id);
if (result == nullptr) {  // Find() returns a pointer.
  // do something
}
```

### <a name="r10-1-2"></a>Recommendation 10.1.2 Use `using` instead of `typedef`.
For versions earlier than `C++11`, you can define the alias of the type by using `typedef`. No one wants to repeat code like `std::map<uint32_t, std::vector<int>>`.

```cpp
typedef std::map<uint32_t, std::vector<int>> SomeType;
```

Using alias for the type is actually encapsulating the type. This encapsulation makes the code clearer, and to a large extent avoids the bulk modification caused by the type change.
For versions later than `C++ 11`, `using` is provided to implement `alias declarations`:

```cpp
using SomeType = std::map<uint32_t, std::vector<int>>;
```

Compare the two formats:

```cpp
typedef Type Alias;   // It cannot be told whether Type or Alias is at the front.
using Alias = Type;   // The format confirms to the assignment rule. It is easy to understand and helps reduce errors.
```

If this is not enough to prove the advantages of `using`, the alias template may be a better example:

```cpp
//: Only one line of code is need to define an alias for a template.
template<class T>
using MyAllocatorVector = std::vector<T, MyAllocator<T>>;

MyAllocatorVector<int> data;       // An alias for a template defined with "using".

template<class T>
class MyClass {
 private:
  MyAllocatorVector<int> data_;   // Another.
};
```

`typedef` does not support alias templates and they have to be hacked in.

```cpp
// A template is used for packaging typedef. Therefore, a template class is needed.
template<class T>
struct MyAllocatorVector {
  typedef std::vector<T, MyAllocator<T>> type;
};

MyAllocatorVector<int>::type data;  // ::type needs to be added when using typedef to define an alias.

template<class T>
class MyClass {
 private:
  typename MyAllocatorVector<int>::type data_;  // For a template class, typename is also needed in addition to ::type.
};
```

### <a name="r10-1-4"></a>Rule 10.1.4 Do not use std::move to operate the const object.
Literally, `std::move` means moving an object. The const object cannot be modified and cannot be moved. Therefore, using `std::move` to operate the const object may confuse code readers.
Regarding actual functions, `std::move` converts an object to the rvalue reference type. It can convert the const object to the rvalue reference of const. Because few types define the move constructor and the move assignment operator that use the const rvalue reference as the parameter, the actual function of code is often degraded to object copy instead of object movement, which brings performance loss.

**Bad example:**
```cpp
std::string gString;
std::vector<std::string> gStringList;

void func() {
  const std::string myString = "String content";
  gString = std::move(myString); // Bad: myString is not moved. Instead, it is copied.
  const std::string anotherString = "Another string content";
  gStringList.push_back(std::move(anotherString));    // Bad: anotherString is not moved. Instead, it is copied.
}
```

## <a name="c10-2"></a>Smart Pointers
### <a name="r10-2-1"></a>Recommendation 10.2.1 Preferentially use the smart pointer instead of the raw pointer to manage resources.

Avoid resource leakage.

**Example**:

```cpp
void Use(int i) {
  auto p = new int {7};               // Bad: Initializing local pointers with new.
  auto q = std::make_unique<int>(9);  // Good: Guarantee that memory is released.
  if (i > 0) {
    return;                           // Return and possible leak.
  }
  delete p;                           // Too late to salvage.
}
```

**Exception:**
Raw pointers can be used in scenarios such as performance sensitivity and compatibility.

### <a name="r10-2-1"></a>Rule 10.2.1 Use `unique_ptr` instead of `shared_ptr`.

1. Using `shared_ptr` a lot has an overhead (atomic operations on the `shared_ptr`s reference count have a measurable cost).
2. Shared ownership in some cases (such as circular dependency) may create objects that can never be released.
3. Shared ownership can be an attractive alternative to careful ownership design but it may obfuscate the design of a system.

### <a name="r10-2-2"></a>Rule 10.2.2 Use `std::make_unique` instead of `new` to create `unique_ptr`.

1. `make_uniqe` provides a simpler creation method.
2. `make_uniqe` ensures the exception safety of complex expressions.

**Example**

```cpp
// Bad: MyClass appears twice, which carries a risk of inconsistency.
std::unique_ptr<MyClass> ptr(new MyClass(0, 1));
// Good: MyClass appears once and there is no possibility of inconsistency.
auto ptr = std::make_unique<MyClass>(0, 1);
```

Recurrence of types may cause serious problems, and it is difficult to find them:

```cpp
// The code compiles fine, but new and delete usage does not match.
std::unique_ptr<uint8_t> ptr(new uint8_t[10]);
std::unique_ptr<uint8_t[]> ptr(new uint8_t);
// No exception safety: The compiler may calculate parameters in the following order:
// 1. Allocate the memory of Foo.
// 2. Construct Foo.
// 3. Call Bar.
// 4. Construct unique_ptr<Foo>.
// If Bar throws an exception, Foo is not destroyed and a memory leak occurs.
F(unique_ptr<Foo>(new Foo()), Bar());

// Exception safety: Calling of function is not interrupted.
F(make_unique<Foo>(), Bar());
```

**Exception:**
`std::make_unique` does not support user-defined `deleter`.
In the scenario where the `deleter` needs to be customized, it is recommended that `make_unique` of the customized version be implemented in its own namespace.
`Using `new` to create `unique_ptr` with the user-defined `deleter` is the last choice.

### <a name="r10-2-3"></a>Rule 10.2.3 Create `shared_ptr` by using `std::make_shared` instead of `new`.

In addition to the consistency factor similar to that in `std::make_unique` when using `std::make_shared`, performance is also a factor to consider.
`std::shared_ptr` manages two entities:
* Control block (storing reference count, `deleter`, etc.)
Managed objects

When `std::make_shared` creates `std::shared_ptr`, it allocates sufficient memory for storing control blocks and managed objects on the heap at a time. When `std::shared_ptr<MyClass>(new MyClass)`is used to create `std::shared_ptr`, except that `new MyClass` triggers a heap allocation, the constructor function of `std::shard_ptr` triggers the second heap allocation, resulting in extra overhead.

**Exception:**
Similar to `std::make_unique`, `std::make_shared` does not support `deleter` customization.

## <a name="c10-3"></a>Lambda
### <a name="a10-3-1"></a>Recommendation 10.3.1 Use `lambda` to capture local variables or write local functions when normal functions do not work.

Functions cannot capture local variables or be declared at local scope. If you need those things, choose `lambda` instead of handwritten `functor`.
On the other hand, `lambda` and `functor` objects do not overload. If overload is required, use a function.
If both `lambda` and functions work, a function is preferred. Use the simplest tool.

**Example**

```cpp
// Write a function that accepts only an int or string.
// -- Overloading is natural.
void F(int);
void F(const string&);

// The local state needs to be captured or appear in the statement or expression range.
// -- A lambda is natural.
vector<Work> v = LotsOfWork();
for (int taskNum = 0; taskNum < max; ++taskNum) {
  pool.Run([=, &v] {...});
}
pool.Join();
```

### <a name="r10-3-2"></a>Rule 10.3.1 Avoid capturing by reference in lambdas that will be used nonlocally.

When used in non-local scope, `lambdas` includes returned values which are stored on the heap, or passed to other threads. Local pointers and references should not outlive their scope. Capturing by reference in `lambdas` indicates storing a reference to a local object. If this leads to a reference that exceeds the local variable lifecycle, capturing by reference should not be used.

**Example**

```cpp
// Bad
void Foo() {
  int local = 42;
  // Capture a reference to a local variable.
  // After the function returns results, local no longer exists,
  // Process() call will have undefined behavior.
  threadPool.QueueWork([&]{ Process(local); });
}

Good
void Foo() {
  int local = 42;
  // Capture a copy of local.
  // Since a copy of local is made, it will be always available for the call.
  threadPool.QueueWork([=]{ Process(local); });
}
```

### <a name="a10-3-2"></a>Recommendation 10.3.2 All variables are explicitly captured if `this` is captured.

The `[=]` in the member function seems to indicate capturing by value but actually it is capturing data members by reference because it captures the invisible `this` pointer by value. Generally, it is recommended that capturing by reference be avoided. If it is necessary to do so, write `this` explicitly.

**Example**

```cpp
class MyClass {
 public:
  void Foo() {
    int i = 0;

    auto Lambda = [=]() { Use(i, data_); };   // Bad: It looks like coping or capturing by value but member variables are actually captured by reference.

    data_ = 42;
    Lambda(); // Call use(42);
    data_ = 43;
    Lambda(); // Call use(43);

    auto Lambda2 = [i, this]() { Use(i, data_); }; // Good: the most explicit and least confusing method.
  }

 private:
  int data_ = 0;
};
```

### <a name="a10-3-3"></a>Recommendation 10.3.3 Avoid default capture modes.

The lambda expression provides two default capture modes: by-reference (&) and by-value (=).
By default, the "by-reference" capture mode will implicitly capture the reference of all local variables, which will easily lead to dangling references. By contrast, explicitly writing variables that need to be captured can make it easier to check the life cycle of an object and reduce the possibility of making a mistake
By default, the "by-value” capture mode will implicitly capture this pointer, and it is difficult to find out which variables the lambda function depends on. If a static variable exists, the reader mistakenly considers that the lambda has copied a static variable.
Therefore, it is required to clearly state the variables that lambda needs to capture, instead of using the default capture mode.

**Bad example:**
```cpp
auto func() {
  int addend = 5;
  static int baseValue = 3;

  return [=]() {    // Only addend is actually copied.
    ++baseValue;    // The modification will affect the value of the static variable.
    return baseValue + addend;
  };
}
```

**Good example:**
```cpp
auto func() {
  int addend = 5;
  static int baseValue = 3;

  return [addend, baseValue = baseValue]() mutable {  // Uses the C++14 capture initialization to copy a variable.
    ++baseValue;    // Modifying the copy of a static variable does not affect the value of the static variable.
    return baseValue + addend;
  };
}
```

Reference: Effective Modern C++: Item 31: Avoid default capture modes.

## <a name="c10-4"></a>Interfaces
### <a name="a10-4-1"></a>Recommendation 10.4.1 Use `T*` or `T&` arguments instead of a smart pointer in scenarios where ownership is not involved.

1. Passing a smart pointer to transfer or share ownership should only be used when the ownership mechanism is explicitly required.
2. Passing a smart pointer (for example, passing the `this` smart pointer) restricts the use of a function to callers using smart pointers.
3. Passing a shared smart pointer adds a runtime performance cost.

**Example**:

```cpp
// Accept any int*.
void F(int*);

// Accept only integers for which you want to transfer ownership.
void G(unique_ptr<int>);

// Accept only integers for which you want to share ownership.
void G(shared_ptr<int>);

// Does not need to change the ownership but requires ownership of the caller.
void H(const unique_ptr<int>&);

// Accept any int.
void H(int&);

//Bad example
void F(shared_ptr<Widget>& w) {
  // ...
  Use(*w); // When only w is used, lifecycle management is not required.
  // ...
};
```


# <a name="c11"></a> 11 Secure Coding Standard

## <a name="c11-1"></a>Basic Principles

1. Programs must strictly verify external data. During external data processing, programmers must keep this in mind and not make any assumption that external data meets expectations. External data must be strictly checked before being used.
 Programmers must abide by this principle in the complex attack environment to ensure that the program execution process is in line with expected results.

2. The attack surface of code must be minimized. The code implementation should be as simple as possible to avoid unnecessary data exchange with external environments. Excess attack surfaces will increase the attack probability. Therefore, avoid exposing internal data processing of programs to external environments.

3. Defensive coding strategies must be used to compensate for potential negligence of programmers. Every man is liable to error. Due to uncertainties of external environments and the differences in the experience and habits of programmers, it is hard for the code execution process to fully meet expectations.
 Therefore, defensive strategies must be adopted in the coding process to minimize the defects caused by the negligence of programmers.
 The measures include:

- Defining an initial value for the declaration of variables.
- Exercise caution in using global variables.
- Avoid using complex and error-prone functions.
- Do not use error-prone mechanisms of compilers/operating systems.
- Deal with the resource access process carefully.
- Do not change the runtime environment of the operating system. For example, do not create temporary files, modify environment variables, or create processes.
- Rectify errors strictly.
- Use the debugging assertion (ASSERT) properly.

## <a name="c11-2"></a>Variables

### <a name="r11-2-1"></a>Rule 11.2.1: Define an initial value for the declaration of pointer variables, variables indicating resource descriptors, or BOOL variables.

Note: Defining an initial value for the declaration of variables can prevent programmers from referencing uninitialized variables.

Good example:

```cpp
SOCKET s = INVALID_SOCKET;
unsigned char *msg = nullptr;
int fd = -1;
```

Bad example: In the following code, no initial value is defined for the declaration of variables. As a result, an error occurs in the free step.

```cpp
char *message; // Error! char *message = nullptr; is required.

if (condition) {
  message = (char *)malloc(len);
}

if (message != nullptr) {
  free(message); //If the condition is not met, the uninitialized memory will be freed.
}
```

### <a name="r11-2-2"></a>Rule 11.2.2: Assign a new value to the variable pointing to a resource handle or descriptor immediately after the resource is freed.

Note: After a resource is freed, a new value must be immediately assigned to the corresponding variable to prevent the re-reference of the variable. If the release statement is in the last line of the scope, you do not need to assign a new value.

Good example:

```cpp
SOCKET s = INVALID_SOCKET;
...
closesocket(s);
s = INVALID_SOCKET;

unsigned char *msg = nullptr;
...
free(msg);
msg = nullptr;
```

### <a name="r11-2-3"></a>Rule 11.2.3: Ensure that local variables in a function do not take up too much space.

When a program is running, the local variables in the function are stored in the stack, and the stack size is limited. If a large static array is requested, an error may occur.
 It is recommended that the size of the static array not exceed 0x1000.

In the following code, buff requests a large stack but the stack space is insufficient. As a result, stack overflow occurs in the program.

```c++
constexpr int MAX_BUF = 0x1000000;
int Foo() {
    char buff[MAX_BUFF] = {0}; // Bad
    ...
}
```

## <a name="c11-3"></a>Assertions

### Principles

Assertions in code consist of ASSERT and CHECK_FATAL. ASSERT is used to determine conditions in DEBUG mode. If conditions are not met, the program exits directly. CHECK_FATAL is used to detect exceptions during program running. If the conditions are not met, the program exits.

CHECK_FATAL is applicable to scenarios where the input and resource application are not under control. Example:

```cpp
CHECK_FATAL(mplName.rfind(kMplSuffix) != std::string::npos, "File name %s does not contain .mpl", mplName.c_str()); // The file name does not meet the requirements.

CHECK_FATAL(intrinCall->GetReturnVec().size() == 1, "INTRN_JAVA_FILL_NEW_ARRAY should have 1 return value"); // The logic restriction is not met.

CHECK_FATAL(func->GetParamSize() <= 0xffff, "Error:the argsize is too large"); // The validity is verified.

void *MemPool::Malloc(size_t size) {
  ...
  CHECK_FATAL(b != nullptr, "ERROR: Malloc error"); // Failed to apply for memory.
}
```

ASSERT is applicable to scenarios where you want to locate bugs in the defensive programming mode. Example:

```cpp
ASSERT(false, "should not be here");

ASSERT(false, "Unknown opcode for FoldIntConstComparison");
```

### <a name="a11-3-1"></a>Recommendation 11.3.1 Do not use ASSERT to verify whether a pointer with security context is nullptr.

Note: The compiler is an offline compilation tool. The impact of process breakdown is much less than that of online services. Therefore, the defensive programming mode should be reduced. Not all input parameters require null pointer verification. Instead, the context logic is used to determine whether null pointer verification is required. An input parameter without the nullptr logic does not need to be verified. For details, see the assertion usage principles.

### <a name="a11-3-2"></a>Recommendation 11.3.2 Do not use ASSERT to verify whether a data array with security context exceeds the threshold.

Note: Similar to the null pointer rule, the context logic is used to determine whether to use assertions for out-of-threshold array verification. For details, see the assertion usage principles.

### <a name="a11-3-3"></a>Recommendation 11.3.3 Do not use ASSERT to verify integer overflow, truncation, or wraparound in the case of context security.

Note: In terms of integer overflow caused by addition or multiplication, verification is not required with the context logic guaranteed. In terms of integer truncation and wraparound caused by type conversion, verification is not required with the context logic guaranteed. For details, see the assertion usage principles.

To ensure that fault tolerance and logic continue to run, you can use conditional statements for verification.

### <a name="r11-3-1"></a>Rule 11.3.1 Do not use ASSERT to verify errors that may occur during program runtime.

Bad example:

```cpp
FILE *fp = fopen(path, "r");
ASSERT(fp != nullptr, "nullptr check"); //Incorrect code: Opening the file may fail.

char *str = (char *)malloc(MAX_LINE);
ASSERT(str != nullptr, "nullptr check"); //Incorrect code: Memory allocation may fail.
ReadLine(fp, str);
```

### <a name="r11-3-2"></a>Rule 11.3.2 Do not modify the runtime environment in ASSERT.

Note: In the formal release stage of a program, ASSERT is not compiled. To ensure the function consistency between the debugging version and formal version, do not perform any operation, such as value assignment, variable modification, resource operation, or memory application, in ASSERT.


In the following code, ASSERT configuration is incorrect.

```cpp
ASSERT(i++ > 1000); // p1 is modified.
ASSERT(close(fd) == 0); // fd is closed.
```

## <a name="c11-4"></a>Exception Mechanisms

### <a name="r11-4-1"></a>Rule 11.4.1 Do not use the C++ exception mechanism.

Note: Do not use the exception mechanism of C++. All errors must be transferred between functions and judged using error values, but not be handled using the exception mechanism.

Programmers must fully control the entire coding process, build the attacker mindset, enhance secure coding awareness, and attach importance to procedures with potential errors. Using the C++ exception mechanism to handle errors, however, will weaken the security awareness of programmers because it will:

Disrupt program execution, making the program structure more complex and used resources not cleared.

Reduce the reusability of code. The code that uses the exception mechanism cannot be reused by the code that does not use the exception mechanism.

Depend on the compiler, operating system, and processor. The execution performance of the program will deteriorate if the exception mechanism is used.

Increase the attack surface of a program in the binary layer after the program is loaded. The attacker can overwrite the abnormal processing function address to launch an attack.


## <a name="c11-5"></a>Memory

### <a name="r11-5-1"></a>Rule 11.5.1: Verify the requested memory size before requesting memory.

The requested memory size may come from external data and must be verified to prevent memory abuse. The requested memory size must not be 0.
Example:
```cpp
int Foo(int size) {
  if (size <= 0) {
    //error
    ...
  }
  ...
  char *msg = (char *)malloc(size);
  ...
}
```

### <a name="r11-5-2"></a>Rule 11.5.2: Check whether memory allocation is successful.

```cpp
char *msg = (char *)malloc(size);
if (msg != nullptr) {
  ...
}
```

## <a name="c11-6"></a>Dangerous Functions

### <a name="r11-6-1"></a>Rule 11.6.1: Do not use dangerous functions related to memory operations.
Many C functions do not use the destination buffer size as a parameter or consider memory overlapping and invalid pointers. As a result, security vulnerabilities such as buffer overflow may be caused.

The historical statistics about buffer overflow vulnerabilities show that a majority of the vulnerabilities are caused by memory operation functions that do not consider the destination buffer size.
The following lists the dangerous functions related to memory operations:
Memory copy functions: memcpy(), wmemcpy(), memmove(), wmemmove() 

Memory initialization function: memset() String copy functions: strcpy(), wcscpy(),strncpy(), wcsncpy() 

String concatenation functions: strcat(), wcscat(),strncat(), wcsncat() 

Formatted string output functions: sprintf(), swprintf(), vsprintf(), vswprintf(), snprintf(), vsnprintf() 

Formatted string input functions: scanf(), wscanf(), vscanf(), vwscanf(), fscanf(),fwscanf(),vfscanf(),vfwscanf(),sscanf(), swscanf(), vsscanf(), vswscanf() 

stdin stream-input function: gets() 
Use safe functions. For details, see huawei_secure_c.

Exceptions: In the following cases, external data processing is not involved, and no attack risks exist. Memory operations are complete in this function, and there is no possibility of failure.
Using safe functions causes redundant code, and therefore dangerous functions can be used in these cases.

(1) Initialize a fixed-length array, or initialize the memory of the structure with a fixed length:
```cpp
BYTE array[ARRAY_SIZE];

void Foo() {
  char destBuff[BUFF_SIZE];
  ...
  memset(array, c1, sizeof(array)); //Assign values to global fixed-length data.
  ...
  memset(destBuff, c2, sizeof(destBuff)); //Assign values to partial fixed-length data.
  ...
}

typedef struct {
  int type;
  int data;
} Tag;

Tag g_tag = {1, 2};

void Foo() {
  Tag dest;
  ...
  memcpy((void *)&dest, (const void *)&g_tag, sizeof(Tag)); //Assign values to fixed-length structure.
  ...
}
```

(2) Initialize memory if function parameters include memory parameters.
```cpp
void Foo(BYTE *buff1, size_t len1, BYTE *buff2, size_t len2) {
  ...
  memset(buff1, 0, len1); //Clear buff1.
  memset(buff2, 0, len2); //Clear buff2.
  ...
}
```

(3) Assign an initial value after allocating memory from the heap.
```cpp
size_t len = ...
char *str = (char *)malloc(len);
if (str != nullptr) {
  memset(str, 0, len);
  ...
}
```

(4) Copy memory with the same size as the source memory size.
The following code copies a memory block with the same size as srcSize:
```cpp
BYTE *src = ...
size_t srcSize = ...
BYTE *destBuff = new BYTE[srcSize];
memcpy(destBuff, src, srcSize);
```

The following code copies a memory block with the same size as the source character string:
```cpp
char *src = ...
size_t len = strlen(src);
if (len > BUFF_SIZE) {
  ...
}
char *destBuff = new char[len + 1];
strcpy(destBuff, src);
```

(5) The source memory stores static character string constants only. (Check whether the destination memory is sufficient during encoding.)
The following code directly copies the string constant "hello" to the array:
```cpp
char destBuff[BUFF_SIZE];
strcpy(destBuff, "hello");
```
The following code concatenates static character string constants:
```cpp
const char *list[] = {"red","green","blue"};
char destBuff[BUFF_SIZE];
sprintf(destBuff, "hello %s", list[i]);
```

