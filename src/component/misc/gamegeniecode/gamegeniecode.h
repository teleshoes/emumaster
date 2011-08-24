#ifndef GAMEGENIECODE_H
#define GAMEGENIECODE_H

#include <QObject>

#if defined(GAMEGENIECODE_PROJECT)
# define GAMEGENIECODE_EXPORT Q_DECL_EXPORT
#else
# define GAMEGENIECODE_EXPORT Q_DECL_IMPORT
#endif

class GAMEGENIECODE_EXPORT GameGenieCode {
public:
	bool parse(const QString &s);
	uint address() const;
	quint8 expectedData() const;
	quint8 replaceData() const;
	bool isEightCharWide() const;

	void setExpectedData(quint8 data);
private:
	uint m_address;
	quint8 m_expected;
	quint8 m_replace;
	bool m_eightChars;
};

inline uint GameGenieCode::address() const
{ return m_address; }
inline quint8 GameGenieCode::expectedData() const
{ return m_expected; }
inline quint8 GameGenieCode::replaceData() const
{ return m_replace; }
inline bool GameGenieCode::isEightCharWide() const
{ return m_eightChars; }
inline void GameGenieCode::setExpectedData(quint8 data)
{ m_expected = data; }

#endif // GAMEGENIECODE_H
