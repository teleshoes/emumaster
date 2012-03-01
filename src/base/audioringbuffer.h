/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef AUDIORINGBUFFER_H
#define AUDIORINGBUFFER_H

#include "emu.h"

template <int Size>
class AudioRingBuffer
{
public:
	void reset();
	void writeSample(s16 l, s16 r);
	int fillBuffer(char *stream, int maxSize);
private:
	int m_head;
	int m_tail;

	s16 m_buffer[Size];
};

template <int Size>
void AudioRingBuffer<Size>::reset()
{
	m_head = 0;
	m_tail = 0;
}

template <int Size>
void AudioRingBuffer<Size>::writeSample(s16 l, s16 r)
{
	m_buffer[m_head+0] = l;
	m_buffer[m_head+1] = r;
	m_head = (m_head+2) % Size;
}

template <int Size>
int AudioRingBuffer<Size>::fillBuffer(char *stream, int maxSize)
{
	// maxSize is size in bytes
	int maxLength = maxSize/2;
	int length = (m_head-m_tail+Size) % Size;
	if (length > maxLength)
		length = maxLength;
	if (length) {
		if (m_tail+length >= Size) {
			u32 partialLength = Size - m_tail;
			memcpy((s16 *)stream, m_buffer+m_tail, partialLength*2);
			m_tail = 0;
			u32 remainderLength = length - partialLength;
			memcpy((s16 *)stream+partialLength, m_buffer+m_tail, remainderLength*2);
			m_tail = remainderLength;
		} else {
			memcpy((s16 *)stream, m_buffer+m_tail, length*2);
			m_tail += length;
		}
	}
	// return size in bytes
	return length * 2;
}

#endif // AUDIORINGBUFFER_H
