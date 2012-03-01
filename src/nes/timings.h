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

#ifndef NESTIMINGS_H
#define NESTIMINGS_H

#define NES_BASE_NTSC_CLK			21477270.0
#define NES_BASE_PAL_CLK			26601714.0

#define NES_CPU_NTSC_CLK			NES_BASE_NTSC_CLK/12.0
#define NES_CPU_PAL_CLK				NES_BASE_PAL_CLK/15.0

#define NES_NTSC_FRAMERATE			60.0
#define NES_PAL_FRAMERATE			50.0

#define NES_NTSC_SCANLINE_CLOCKS	1364
#define NES_PAL_SCANLINE_CLOCKS		1278

#define NES_NTSC_SCANLINE_COUNT		262
#define NES_PAL_SCANLINE_COUNT		312

#endif // NESTIMINGS_H
