#ifndef NES2C0X_GLOBAL_H
#define NES2C0X_GLOBAL_H

#include <QtGlobal>

#if defined(NES2C0X_PROJECT)
# define NES2C0X_EXPORT Q_DECL_EXPORT
#else
# define NES2C0X_EXPORT Q_DECL_IMPORT
#endif

#endif // NES2C0X_GLOBAL_H
