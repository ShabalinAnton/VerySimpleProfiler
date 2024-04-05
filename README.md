# VerySimpleProfiler
This simple profiler allows you to measure the execution time of a block of code or a function, and the number of calls.

## Usage example:
```c++
#define VERY_SIMPLE_PROFILER
#include "verySimpleProfiler.hpp"

void some_function()
{
    VERY_SIMPLE_FUNC_PROFILE();

    {
        VERY_SIMPLE_BLOCK_PROFILE("Calculating 1");
        // some calculations
    }
    // some other processing
    {
        VERY_SIMPLE_BLOCK_PROFILE("Calculating 2");
        // some calculations
    }
}
```
