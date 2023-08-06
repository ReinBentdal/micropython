/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2016-2017 Linaro Limited
 * Copyright (c) 2020 NXP
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <zephyr/kernel.h>

#include "py/mperrno.h"
#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/stackctrl.h"
#include "py/obj.h"
#include "py/objfun.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main, 4);

static char heap[CONFIG_MICROPY_HEAP_SIZE];

int main(void) {
    LOG_DBG("booting...");

    mp_stack_ctrl_init();
    mp_stack_set_limit(CONFIG_MAIN_STACK_SIZE - 512);

    gc_init(heap, heap + sizeof(heap));

    mp_init();
    mp_obj_list_init(mp_sys_path, 0);
    mp_obj_list_init(mp_sys_argv, 0);

    const char* script = "def test(num):\n"
                        "    print(f'Hello number {num}')\n";

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        LOG_DBG("Before lexer\n");
        mp_lexer_t *lex = mp_lexer_new_from_str_len(0, script, strlen(script), false);
        LOG_DBG("After lexer\n");

        LOG_DBG("Before parsing\n");
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        LOG_DBG("After parsing\n");

        LOG_DBG("Before compilation\n");
        mp_obj_t obj_fun = mp_compile(&parse_tree, lex->source_name, false);

        mp_obj_fun_bc_t* fun_bc = MP_OBJ_TO_PTR(obj_fun);
        
        mp_code_state_t code_state;
        code_state.fun_bc = fun_bc;
        code_state.n_state = 0; // No local state initially.
        mp_setup_code_state(&code_state, 0, 0, NULL);
        LOG_DBG("After compilation\n");
        
        LOG_DBG("Before bytecode execution\n");
        mp_vm_return_kind_t ret = mp_execute_bytecode(&code_state, NULL);
        if (ret != MP_VM_RETURN_NORMAL) {
            LOG_DBG("Error executing bytecode, %d", (int)ret);
        }
        LOG_DBG("After bytecode execution\n");
        
        nlr_pop();
    } else {
        LOG_WRN("exception");
        mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
    }

    LOG_DBG("Before calling fn\n");

    mp_obj_t test_fn = mp_load_global(MP_QSTR_test);

    if (test_fn == MP_OBJ_NULL) {
        // Function not found, handle error
        LOG_DBG("'test' function not found");
    } else {
        // Call the function
        mp_obj_t num = mp_obj_new_int(112);
        mp_obj_t result = mp_call_function_1(test_fn, num);
        // Here you can do something with the result, if the function returns anything.
        (void)result;
    }

    mp_deinit();

    LOG_DBG("End of main\n");

    k_sleep(K_FOREVER);

    return 0;
}

void gc_collect(void) {
    // WARNING: This gc_collect implementation doesn't try to get root
    // pointers from CPU registers, and thus may function incorrectly.
    void *dummy;
    gc_collect_start();
    gc_collect_root(&dummy, ((mp_uint_t)MP_STATE_THREAD(stack_top) - (mp_uint_t)&dummy) / sizeof(mp_uint_t));
    gc_collect_end();
}

#if !MICROPY_READER_VFS
mp_lexer_t *mp_lexer_new_from_file(const char *filename) {
    mp_raise_OSError(ENOENT);
}
#endif

#if !MICROPY_VFS
mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif

NORETURN void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif