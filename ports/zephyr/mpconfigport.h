/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include <alloca.h>

// Include Zephyr's autoconf.h, which should be made first by Zephyr makefiles
// #include "autoconf.h"
// Included here to get basic Zephyr environment (macros, etc.)
#include <zephyr/kernel.h>

// Usually passed from Makefile
#ifndef MICROPY_HEAP_SIZE
#define MICROPY_HEAP_SIZE (16 * 1024)
#endif

// This allows MicroPython to track the source line of the Python script which helps in debugging.
#define MICROPY_ENABLE_SOURCE_LINE  (1)

// Gives nices error messages because the size of the stack is monitored.
#define MICROPY_STACK_CHECK         (1)

// TODO: what happens if this is not enabled?
#define MICROPY_ENABLE_GC           (1)

// This enables the MicroPython compiler which you need to compile the Python script.
#define MICROPY_ENABLE_COMPILER     (1)

// TODO: The REPL is currently not possible to remove entirely.
// REPL is the interractive console to on-the-fly execute MicroPython.
// Enable or disable the REPL helper, which can assist with auto-indentation and other features in the REPL.
#define MICROPY_HELPER_REPL         (0)

// Enable or disable auto-indentation in the REPL.
#define MICROPY_REPL_AUTO_INDENT    (0)

// Enable or disable keyboard interrupt exceptions (i.e., allow the program to be interrupted by pressing Ctrl+C). Assumed in the REPL.
#define MICROPY_KBD_EXCEPTION       (0)

// Enable Python 'Literal String Formatting', ie. formatting using f'val: {val}'
#define MICROPY_PY_FSTRINGS         (1)

// Makes modules behave more like CPython, at the cost of reduced performance and more memory usage.
#define MICROPY_CPYTHON_COMPAT      (0)

// Enable or disable support for the async and await keywords for asynchronous programming.
#define MICROPY_PY_ASYNC_AWAIT      (0)

// Enable or disable attribute access on tuples.
#define MICROPY_PY_ATTRTUPLE        (0)

// Virtual File System
#define MICROPY_VFS                 (0)

// Enable or disable specific built-in Python features or functions, like enumerate, filter, min, max, etc.
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS (0) // wether to include start, stop, step attributes in range objects. Does not effect function arguments.
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_SLICE   (0)

// Enable or disable specific Python modules.
#define MICROPY_PY_ARRAY            (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_STRUCT           (0)
#define MICROPY_PY_SYS_MODULES      (0)

// Choose the implementation for long integers. The option MICROPY_LONGINT_IMPL_LONGLONG uses long long types from C.
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)

// Choose the implementation for floating point numbers. The option MICROPY_FLOAT_IMPL_FLOAT uses float types from C.
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)

// Enable or disable support for complex numbers.
#define MICROPY_PY_BUILTINS_COMPLEX (0)

// These are compiler optimizations. For example, constant folding pre-calculates the results of computations with constant values at compile time, rather than runtime.
// However it comes at the cost of more space.
// Saving extra crumbs to make sure binary fits in 128K
#define MICROPY_COMP_CONST_FOLDING  (0)
#define MICROPY_COMP_CONST (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)

#ifdef CONFIG_BOARD
#define MICROPY_HW_BOARD_NAME "zephyr-" CONFIG_BOARD
#else
#define MICROPY_HW_BOARD_NAME "zephyr-generic"
#endif

#ifdef CONFIG_SOC
#define MICROPY_HW_MCU_NAME CONFIG_SOC
#else
#define MICROPY_HW_MCU_NAME "unknown-cpu"
#endif

typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM
