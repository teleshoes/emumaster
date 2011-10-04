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

///////////////////////////////////////////////////////////////////////////////
static void gpuDrawS(const PS gpuSpriteSpanDriver)
{
	s32 x0, x1;
	s32 y0, y1;
	s32 u0;
	s32 v0;

	x1 = x0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[2]) + DrawingOffset[0];
	y1 = y0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[3]) + DrawingOffset[1];
	x1+= PacketBuffer.S2[6];
	y1+= PacketBuffer.S2[7];

	{
		s32 xmin, xmax;
		s32 ymin, ymax;
		xmin = DrawingArea[0];	xmax = DrawingArea[2];
		ymin = DrawingArea[1];	ymax = DrawingArea[3];

		{
			int rx0 = Max2(xmin,Min2(x0,x1));
			int ry0 = Max2(ymin,Min2(y0,y1));
			int rx1 = Min2(xmax,Max2(x0,x1));
			int ry1 = Min2(ymax,Max2(y0,y1));
			if( rx0>=rx1 || ry0>=ry1) return;
		}

		u0 = PacketBuffer.U1[8];
		v0 = PacketBuffer.U1[9];

		r4 = s32(PacketBuffer.U1[0]);
		g4 = s32(PacketBuffer.U1[1]);
		b4 = s32(PacketBuffer.U1[2]);

		{
			s32 temp;
			temp = ymin - y0;
			if (temp > 0) { y0 = ymin; v0 += temp; }
			if (y1 > ymax) y1 = ymax;
			if (y1 <= y0) return;
			
			temp = xmin - x0;
			if (temp > 0) { x0 = xmin; u0 += temp; }
			if (x1 > xmax) x1 = xmax;
			x1 -= x0;
			if (x1 <= 0) return;
		}
	}

	{
		u16 *Pixel = &((u16*)GPU_FrameBuffer)[FRAME_OFFSET(x0, y0)];
		const u32 masku=TextureWindow[2];
		const u32 maskv=TextureWindow[3];

		for (;y0<y1;++y0) {
			gpuSpriteSpanDriver(Pixel,x1,FRAME_OFFSET(u0,v0),masku);
			Pixel += FRAME_WIDTH;
			v0 = (v0+1)&maskv;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
static void gpuDrawT(const PT gpuTileSpanDriver)
{
	s32 x0, y0;
	s32 x1, y1;

	x1 = x0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[2]) + DrawingOffset[0];
	y1 = y0 = GPU_EXPANDSIGN_SPRT(PacketBuffer.S2[3]) + DrawingOffset[1];
	x1+= PacketBuffer.S2[4];
	y1+= PacketBuffer.S2[5];

	{
		s32 xmin, xmax;
		s32 ymin, ymax;
		xmin = DrawingArea[0];	xmax = DrawingArea[2];
		ymin = DrawingArea[1];	ymax = DrawingArea[3];

		{
			int rx0 = Max2(xmin,Min2(x0,x1));
			int ry0 = Max2(ymin,Min2(y0,y1));
			int rx1 = Min2(xmax,Max2(x0,x1));
			int ry1 = Min2(ymax,Max2(y0,y1));
			if(rx0>=rx1 || ry0>=ry1) return;
		}

		if (y0 < ymin) y0 = ymin;
		if (y1 > ymax) y1 = ymax;
		if (y1 <= y0) return;

		if (x0 < xmin) x0 = xmin;
		if (x1 > xmax) x1 = xmax;
		x1 -= x0;
		if (x1 <= 0) return;
	}
	
	{
		u16 *Pixel = &((u16*)GPU_FrameBuffer)[FRAME_OFFSET(x0, y0)];
		const u16 Data = GPU_RGB16(PacketBuffer.U4[0]);

		for (; y0<y1; ++y0)
		{
			gpuTileSpanDriver(Pixel,x1,Data);
			Pixel += FRAME_WIDTH;
		}
	}
}
