#ifndef __BAS_I18N_H
#define __BAS_I18N_H

#include <libintl.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

void va_init_i18n(const char* locale_dir, ...);

#ifdef TEXT_DOMAIN

#define __stringify(x) #x
#define __to_string(x) __stringify(x)
#define _(s) dgettext(__to_string(TEXT_DOMAIN), (s))
#define N_(s) (s)

#if defined(__GNUC__) || defined(__clang__)
/* Use the GNU comma-swallowing extension */
#define init_i18n(loc, ...) va_init_i18n(loc, __to_string(TEXT_DOMAIN), ##__VA_ARGS__, NULL)
#elif __STDC_VERSION__ >= 202311L
/* Use the C23 standard way if available */
#define init_i18n(loc, ...) va_init_i18n(loc, __to_string(TEXT_DOMAIN) __VA_OPT__(, ) __VA_ARGS__, NULL)
#else
/* Fallback: requires at least one extra arg or a manual NULL */
#define init_i18n(loc, ...) va_init_i18n(loc, __to_string(TEXT_DOMAIN), __VA_ARGS__)
#endif /* __GNUC__ || __clang__ || __STDC_VERSION__ >= 202311L */

#else /* TEXT_DOMAIN */

#if defined(__GNUC__) || defined(__clang__)
/* Use the GNU comma-swallowing extension */
#define init_i18n(loc, ...) va_init_i18n(loc, ##__VA_ARGS__, NULL)
#elif __STDC_VERSION__ >= 202311L
/* Use the C23 standard way if available */
#define init_i18n(loc, ...) va_init_i18n(loc __VA_OPT__(, ) __VA_ARGS__, NULL)
#else
/* Fallback: requires at least one extra arg or a manual NULL */
#define init_i18n(loc, ...) va_init_i18n(loc, __VA_ARGS__)
#endif /* __GNUC__ || __clang__ || __STDC_VERSION__ >= 202311L */

#endif /* TEXT_DOMAIN */

#ifdef __cplusplus
}
#endif

#endif /* __BAS_I18N_H */
