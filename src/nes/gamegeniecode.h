#ifndef GAMEGENIECODE_H
#define GAMEGENIECODE_H

#include <imachine.h>

class GameGenieCode {
public:
	bool parse(const QString &s);
	u16 address() const;
	u8 expectedData() const;
	u8 replaceData() const;
	bool isEightCharWide() const;

	void setExpectedData(u8 data);
private:
	u16 m_address;
	u8 m_expected;
	u8 m_replace;
	bool m_eightChars;
};

inline u16 GameGenieCode::address() const
{ return m_address; }
inline u8 GameGenieCode::expectedData() const
{ return m_expected; }
inline u8 GameGenieCode::replaceData() const
{ return m_replace; }
inline bool GameGenieCode::isEightCharWide() const
{ return m_eightChars; }
inline void GameGenieCode::setExpectedData(u8 data)
{ m_expected = data; }

#endif // GAMEGENIECODE_H
