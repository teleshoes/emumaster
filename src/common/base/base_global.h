#ifndef BASE_GLOBAL_H
#define BASE_GLOBAL_H

#include <QtGlobal>

#if defined(BASE_PROJECT)
# define BASE_EXPORT Q_DECL_EXPORT
#else
# define BASE_EXPORT Q_DECL_IMPORT
#endif

#endif // BASE_GLOBAL_H
