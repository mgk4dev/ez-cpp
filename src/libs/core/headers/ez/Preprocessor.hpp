#pragma once

#define __EZ_CONCAT(a, b) a##b
#define EZ_CONCAT(a, b) __EZ_CONCAT(a, b)
#define EZ_ANONYMOUS_VARIABLE(prefix) EZ_CONCAT(prefix, __COUNTER__)
