#ifndef CRC32_H
#define CRC32_H

#include <QtGlobal>

#if defined(CRC32_PROJECT)
# define CRC32_EXPORT Q_DECL_EXPORT
#else
# define CRC32_EXPORT Q_DECL_IMPORT
#endif

Q_DECL_EXPORT quint32 qChecksum32(const char *data, uint len);

#endif // CRC32_H
