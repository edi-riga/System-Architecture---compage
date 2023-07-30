__[toc]__

# 1 Concept
![Overall concept of the compage framework](/diagrams/concept.svg)
***compage*** is a component-based system architecture implementation framework. It essentially provides means for implementing an efficient thread/process-based black-board programming pattern, i.e. the overall system consists of multiple components that can be instantiated as required.

The framework can be used to "register" different software components that are further instantiated and configured according to the configuration described in _.ini_ files. Such an approach enables efficient system-level experimentation, i.e. one system implementation can be utilized together with a set of configuration files to enable different component interactions and setups, e.g. test simple acquisition-display setup, verify different processing algorithms with data in real-time, and switch between recorded and sensor data. Note that _compage_ does not handle inter-component communications.

The framework can be characterized by its:
- lightweight codebase,
- efficiency,
- minimal dependencies (pthread library),
- compatibility with C and C++ codebase.

# 2 How does it work?
The framework heavily relies on preprocessor macro functions and a particular feature of the _Executable Linked Files (.elf)_ - custom data sections that can be parsed during execution. By registering a component, the framework essentially stores its unique id and corresponding handlers, default data and parameters configurable via _.ini_ file. Consult the following diagram for an approximate memory layout.
![Memory layout of the compage framework](/diagrams/memory_layout.svg)

# 3 Installation
The ***compage*** framework utilizes [_cmake_](https://cmake.org/) build system for its compilation and installation.

Build framework with:
```shell
$ mkdir build && cd build
$ cmake ..
$ make
```

Install framework with:
```
# make install
```

# 4 Usage
## 4.1 Overall introduction
Simplifying, the executable holds optional _init_ (for initialization), _loop_ (for repeated execution), _exit_ (for deinitialization) callbacks and corresponding default argument, which is a pointer to a C-struct. Further, the framework parses the _.ini_ file to decide on the instantiation of the components and configuration to be updated in the default C-struct before handing data to the callbacks.
![General usage of the compage framework](/diagrams/usage.svg)

## 4.2 Using the framwork
## 4.2.1 Calling the framework
The framework provides a simple and more fine-grained APIs for framework's instantiation. The simplest imaginable usage looks like:
```c
#include <edi/compage.h>

int main(int argc, const char *argv[]){
  return compage_main(argc, argv);
}
```

Default framework's instantiation provides a simple command line interface for listing available components and their configuration, generating and applying configuration files. Consult the examples for different methods of using the framework.

## 4.2.2 Defining and registering a component
Let's assume a simple component with just an _init_ call:
```c
/* declare component's default configuration */
typedef struct{
  const char *example_parameter;
}pdata_t;

/* define component's default configuration */
pdata_t pdata = {
  "Hello, compage!"
};

/* define component's init handler (static ensures reuse of function name in other file) */
static compageStatus_t init(pdata_t *p){
  printf("%s\n", p);
  return COMPAGE_SUCCESS;
}
```

We can register the component as follows:
```c
/* register component's id (string to show up as ini file's section) */
COMPAGE_REGISTER(example);
/* register component's default (private) data*/
COMPAGE_REGISTER_PDATA(example, pdata);
/* register component's init handler*/
COMPAGE_REGISTER_INIT(example, pdata);
/* add data to be configured via ini file */
COMPAGE_ADD_CONFIG(example, pdata, example_parameter);
```

Similarly one can register _loop_ and _exit_ callbacks, as well as mark additional parameters for the configuration.

## 4.2.3 Configuration file
The configuration file follows an _INI_ format specification and every section in the file corresponds to a component that should be instantiated. Nonetheless, each component has a default "enable" parameter that can be used for temporary disabling the component's execution. Each parameter in the section corresponds to the C-struct member. Two instantiations of the previously declared component could look like this:
```ini
[example]
enabled=1
example_parameter="String to be displayed upon execution"

[example]
enabled=1
example_parameter="Another string to be displayed upon execution"
```

## 4.3 Supported data types for the configuration
Currently supported data types are:
```c
bool
int8_t
uint8_t
int16_t
uint16_t
int32_t
uint32_t
int64_t
uint64_t
float
double
char
char*
const char*
```

## 4.4 Important considerations
When using compiler optimizations, the compiler can reorder the contents of the compage section, which most probably will result in segmentation fault as the framework depends on the actual layout of the memory. This can be fixed by adding the _-fno-toplevel-reorder_ compilation option.
