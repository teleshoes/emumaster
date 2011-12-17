/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

// TODO consolide two gamegenie files and make one cheats
// TODO make case-insensitive

#include "gamegeniecode.h"

bool GameGenieCode::parse(const QString &s) {
	m_address = 0;
	m_replace = 0;
	m_expected = 0;

	u8 table[8];

	if (s.size() != 6 && s.size() != 8)
		return false;
	m_eightChars = (s.size() == 8);

	for (int i = 0; i < s.size(); i++) {
		// TODO use strchr
		switch (s.at(i).toLatin1()) {
		case 'A': table[i] = 0x00; break;
		case 'P': table[i] = 0x01; break;
		case 'Z': table[i] = 0x02; break;
		case 'L': table[i] = 0x03; break;
		case 'G': table[i] = 0x04; break;
		case 'I': table[i] = 0x05; break;
		case 'T': table[i] = 0x06; break;
		case 'Y': table[i] = 0x07; break;
		case 'E': table[i] = 0x08; break;
		case 'O': table[i] = 0x09; break;
		case 'X': table[i] = 0x0A; break;
		case 'U': table[i] = 0x0B; break;
		case 'K': table[i] = 0x0C; break;
		case 'S': table[i] = 0x0D; break;
		case 'V': table[i] = 0x0E; break;
		case 'N': table[i] = 0x0F; break;
		default: return false; break;
		}
	}

	if (!m_eightChars) {
		m_address |= (table[3] & 0x07) <<12;
		m_address |= (table[4] & 0x08) << 8;
		m_address |= (table[5] & 0x07) << 8;
		m_address |= (table[1] & 0x08) << 4;
		m_address |= (table[2] & 0x07) << 4;
		m_address |= (table[3] & 0x08);
		m_address |= (table[4] & 0x07);

		m_replace |= (table[0] & 0x08) << 4;
		m_replace |= (table[1] & 0x07) << 4;
		m_replace |= (table[5] & 0x08);
		m_replace |= (table[0] & 0x07);
	} else {
		m_address |= (table[3] & 0x07) <<12;
		m_address |= (table[4] & 0x08) << 8;
		m_address |= (table[5] & 0x07) << 8;
		m_address |= (table[1] & 0x08) << 4;
		m_address |= (table[2] & 0x07) << 4;
		m_address |= (table[3] & 0x08);
		m_address |= (table[4] & 0x07);

		m_replace |= (table[0] & 0x08) << 4;
		m_replace |= (table[1] & 0x07) << 4;
		m_replace |= (table[7] & 0x08);
		m_replace |= (table[0] & 0x07);

		m_expected |= (table[6] & 0x08) << 4;
		m_expected |= (table[7] & 0x07) << 4;
		m_expected |= (table[5] & 0x08);
		m_expected |= (table[6] & 0x07);
	}
	return true;
}
