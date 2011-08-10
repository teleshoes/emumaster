#ifndef M6502_GLOBAL_H
#define M6502_GLOBAL_H

#include <QtGlobal>

#if defined(M6502_PROJECT)
# define M6502_EXPORT Q_DECL_EXPORT
#else
# define M6502_EXPORT Q_DECL_IMPORT
#endif

#endif // M6502_GLOBAL_H
