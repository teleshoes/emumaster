#ifndef MEMORY8_GLOBAL_H
#define MEMORY8_GLOBAL_H

#include <QtGlobal>

#if defined(MEMORY8_PROJECT)
# define MEMORY8_EXPORT Q_DECL_EXPORT
#else
# define MEMORY8_EXPORT Q_DECL_IMPORT
#endif

#endif // MEMORY8_GLOBAL_H
