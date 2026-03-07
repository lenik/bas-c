#ifndef DBGACTION_H
#define DBGACTION_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** List actions from UI stack (scriptables), top first, grouped by owner and kpriority, ordered by priority. */
void dbgaction_list(FILE *out, const char *prefix);

/** Run first action named 'name' from topmost scriptable that has it. Returns -1 if not found. */
int dbgaction_do(const char *name, const char* const* argv, int argc);

#ifdef __cplusplus
}
#endif

#endif /* DBGACTION_H */
