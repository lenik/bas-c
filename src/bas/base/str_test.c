#include "str.h"

#include <stdio.h>
#include <string.h>
#include <bas/locale/i18n.h>
#include <bas/proc/env.h>


void qstr_btok_test(char *s) {
    char *tok;
    int c = 0;
    s = str_dup(s);

    while (tok = qstr_btok(s, &s, false)) {
        printf(" <%s>", tok);
        if (c++ > 10) {
            printf("~\n");
            c = 0;
        }
    }
    printf("\n");
}

int main() {
    const char *exe = self_exe();
    init_i18n(LOCALEDIR);
    qstr_btok_test("hello");
    qstr_btok_test("   hello, world   ");
    qstr_btok_test("foo   'bar  baz'    end");
    qstr_btok_test("first second\0third-is-bad");
    return 0;
}
