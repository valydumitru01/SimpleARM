
#pragma once

#include <assert.h>
#include <stddef.h>

static inline void halt_op(ExecResult *result) {
    assert(result != NULL);
    result->should_halt = 1;
}