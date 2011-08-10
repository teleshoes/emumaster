#ifndef NES_GLOBAL_H
#define NES_GLOBAL_H

#include <QtGlobal>

#if defined(NES_PROJECT)
# define NES_EXPORT Q_DECL_EXPORT
#else
# define NES_EXPORT Q_DECL_IMPORT
#endif

#endif // NES_GLOBAL_H
