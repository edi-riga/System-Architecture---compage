# Usage
## Component
1.  **Include API header.** The header contains interface with the API and some particular pre-processor directives.
```C
#include "compage.h"
```
2. **Declare the "private" data structure of your component.** This data structure should incorporate your block's parameters. Later on these parameters can be marked for configuration.
```C
// an example "private data" structure
typedef struct{
    unsigned ip_address;
    short    port;
    char*    message;
} pdata_t;
```

3. **Define the default "private" data structure**
```C
pdata_t pdata = {
    0x7f000001,
    8888,
    "Hello, lazy world!"
};
```
4. **Write your component (worker)**
5. **Register your component (worker)**
```C
COMPAGE_REGISTER(component, pdata);
```

6. **Mark "private data" parameters for configuration**
```C
COMPAGE_PDATA_ADD_CONFIGS(component, pdata_t, )
```
# Support
 Currently supported data types are:
```C
uint8_t
int8_t
uint16_t
int16_t
uint32_t
int32_t
uint64_t
int64_t
float
double
char*
```
