/**
 ******************************************************************************
 * @author: GaoDen
 * @date:   27/05/2025
 ******************************************************************************
**/

#include "lt_console.h"

void lt_console_init(pf_console_putc pf_putc) {
    if (pf_putc == NULL) {
        return;
    }

    /* assign the console putc function */
    xfunc_output = (void(*)(int))pf_putc;
}
