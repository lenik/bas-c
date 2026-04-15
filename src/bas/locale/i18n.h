#ifndef __BAS_I18N_H
#define __BAS_I18N_H

#include <libintl.h>
#include <locale.h>

#ifndef I18N_DOMAIN
#ifdef PACKAGE_NAME
#define I18N_DOMAIN PACKAGE_NAME
#endif
#endif

#ifdef I18N_DOMAIN
#define tr(s) dgettext(I18N_DOMAIN, s)
#else
#define tr(s) gettext(s)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void va_init_i18n(const char* exe_path, const char* locale_dir, const char* text_domain, ...);

#if defined(__GNUC__) || defined(__clang__)
/* Use the GNU comma-swallowing extension */
#define init_i18n(argv0, loc, dom, ...) va_init_i18n(argv0, loc, dom, ##__VA_ARGS__, NULL)
#elif __STDC_VERSION__ >= 202311L
/* Use the C23 standard way if available */
#define init_i18n(argv0, loc, dom, ...)                                                            \
    va_init_i18n(argv0, loc, dom __VA_OPT__(, ) __VA_ARGS__, NULL)
#else
/* Fallback: requires at least one extra arg or a manual NULL */
#define init_i18n(argv0, loc, dom, ...) va_init_i18n(argv0, loc, dom, __VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif

#endif
