

#include "mphalport.h"

#include <zephyr/kernel.h>

void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    printk("%.*s", len, str);
}