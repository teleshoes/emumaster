#include "mapper0.h"

// TODO DWORD	crc = nes->rom->GetPROM_CRC();
//	if( crc == 0x4e7db5af ) {	// Circus Charlie(J)
//		nes->SetRenderMethod( NES::POST_RENDER );
//	}
//	if( crc == 0x57970078 ) {	// F-1 Race(J)
//		nes->SetRenderMethod( NES::POST_RENDER );
//	}
//	if( crc == 0xaf2bbcbc		// Mach Rider(JU)
//	 || crc == 0x3acd4bf1 ) {	// Mach Rider(Alt)(JU)
//		nes->SetRenderMethod( NES::POST_RENDER );
//	}

NES_MAPPER_PLUGIN_EXPORT(0, "-")
