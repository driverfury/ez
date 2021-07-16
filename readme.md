# ez

Easy-to-use general purpose C library written in ANSI C99.

## Quick Start

Just copy ez.h into your project and include it like this:

```c
#define EZ_IMPLEMENTATION
#include "ez.h"
```

If you do not want to rely on C runtime library you have to specify it like so:

```c
#define EZ_IMPLEMENTATION
#define EZ_NO_CRT_LIB
#include "ez.h"
```

Supported platforms (when you define EZ_NO_CRT_LIB):
- [x] Windows
- [ ] Linux
- [ ] MacOS
