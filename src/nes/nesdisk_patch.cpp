#include "nesdisk.h"
#include "nesdisk_p.h"

void NesDisk::patchRom() {
	// Mapper 000
	if (m_crc == 0x57970078) {	// F-1 Race(J)
		m_rom[0x078C] = 0x6C;
		m_rom[0x3FE1] = 0xFF;
		m_rom[0x3FE6] = 0x00;
	}
	if (m_crc == 0xaf2bbcbc		// Mach Rider(JU)
	 || m_crc == 0x3acd4bf1		// Mach Rider(Alt)(JU)
	 || m_crc == 0x8bbe9bec) {
		m_rom[0x090D] = 0x6E;
		m_rom[0x7FDF] = 0xFF;
		m_rom[0x7FE4] = 0x00;

		m_header->m_flagsA = NesDiskHeader::VerticalFlagA;
	}

	if (m_crc == 0xe16bb5fe) {	// Zippy Race(J)
		m_header->m_flagsA &= 0xf6;
	}
	if (m_crc == 0x85534474) {	// Lode Runner(J)
		m_rom[0x29E9] = 0xEA;
		m_rom[0x29EA] = 0xEA;
		m_rom[0x29F8] = 0xEA;
		m_rom[0x29F9] = 0xEA;
	}

	// Mapper 001
	if (m_crc == 0x7831b2ff		// America Daitouryou Senkyo(J)
	 || m_crc == 0x190a3e11		// Be-Bop-Highschool - Koukousei Gokuraku Densetsu(J)
	 || m_crc == 0x52449508		// Home Run Nighter - Pennant League!!(J)
	 || m_crc == 0x0973f714		// Jangou(J)
	 || m_crc == 0x7172f3d4		// Kabushiki Doujou(J)
	 || m_crc == 0xa5781280		// Kujaku Ou 2(J)
	 || m_crc == 0x8ce9c87b		// Money Game, The(J)
	 || m_crc == 0xec47296d		// Morita Kazuo no Shougi(J)
	 || m_crc == 0xcee5857b		// Ninjara Hoi!(J)
	 || m_crc == 0xe63d9193		// Tanigawa Kouji no Shougi Shinan 3(J)
	 || m_crc == 0xd54f5da9		// Tsuppari Wars(J)
	 || m_crc == 0x1e0c7ea3) {	// AD&D Dragons of Flame(J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}
	if (m_crc == 0x1995ac4e) {	// Ferrari Grand Prix Challenge(J)
		m_rom[0x1F7AD] = 0xFF;
		m_rom[0x1F7BC] = 0x00;
	}

	if (m_crc == 0x20d22251) {	// Top rider(J)
		m_rom[0x1F17E] = 0xEA;
		m_rom[0x1F17F] = 0xEA;
	}

	if (m_crc == 0x11469ce3) {	// Viva! Las Vegas(J)
		m_vrom[0x0000] = 0x01;
	}

	if (m_crc == 0x3fccdc7b) {	// Baseball Star - Mezase Sankanou!!(J)
		m_rom[0x0F666] = 0x9D;
	}

	if (m_crc == 0xdb564628) {	// Mario Open Golf(J)
		m_rom[0x30195] = 0xC0;
	}

	// Mapper 002
	if (m_crc == 0x63af202f) {	// JJ - Tobidase Daisakusen Part 2(J)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	if (m_crc == 0x99a62e47) {	// Black Bass 2, The(J)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	if (m_crc == 0x0eaa7515		// Rod Land(J)
	 || m_crc == 0x22ab9694) {	// Rod Land(E)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	if (m_crc == 0x2061772a) {	// Tantei Jinguji Taburou Tokino Sugiyukumamani (J)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	// Mapper 003
	if (m_crc == 0x29401686) {	// Minna no Taabou no Nakayoshi Dai Sakusen(J)
	//	m_rom[0x2B3E] = 0x60;
	}
	if (m_crc == 0x932a077a) {	// TwinBee(J)
		m_mapperType = 87;
	}
	if (m_crc == 0x8218c637) {	// Space Hunter(J)
	//	m_header->m_flagsA &= 0xf6;
	//	m_header->m_flagsA |= NesDiskHeader::FourScreenFlagA;
		m_header->m_flagsA = NesDiskHeader::VerticalFlagA;
	}
	if (m_crc == 0x2bb6a0f8		// Sherlock Holmes - Hakushaku Reijou Yuukai Jiken(J)
	 || m_crc == 0x28c11d24		// Sukeban Deka 3(J)
	 || m_crc == 0x02863604) {	// Sukeban Deka 3(J)(Alt)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	// Mapper 004
	if (m_crc == 0x58581770) {	// Rasaaru Ishii no Childs Quest(J)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}
	if (m_crc == 0xf3feb3ab		// Kunio Kun no Jidaigeki Dayo Zenin Shuugou! (J)
	 || m_crc == 0xa524ae9b		// Otaku no Seiza - An Adventure in the Otaku Galaxy (J)
	 || m_crc == 0x46dc6e57		// SD Gundam - Gachapon Senshi 2 - Capsule Senki (J)
	 || m_crc == 0x66b2dec7		// SD Gundam - Gachapon Senshi 3 - Eiyuu Senki (J)
	 || m_crc == 0x92b07fd9		// SD Gundam - Gachapon Senshi 4 - New Type Story (J)
	 || m_crc == 0x8ee6463a		// SD Gundam - Gachapon Senshi 5 - Battle of Universal Century (J)
	 || m_crc == 0xaf754426		// Ultraman Club 3 (J)
	 || m_crc == 0xfe4e5b11		// Ushio to Tora - Shinen no Daiyou (J)
	 || m_crc == 0x57c12c17) {	// Yamamura Misa Suspense - Kyouto Zaiteku Satsujin Jiken (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}
	if (m_crc == 0x42e03e4a) {	// RPG Jinsei Game (J)
		m_mapperType = 118;
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}
	if (m_crc == 0xfd0299c3) {	// METAL MAX(J)
		m_rom[0x3D522] = 0xA9;
		m_rom[0x3D523] = 0x19;
	}
	if (m_crc == 0x1d2e5018		// Rockman 3(J)
	 || m_crc == 0x6b999aaf) {	// Mega Man 3(U)
	//	m_rom[0x3C179] = 0xBA;//
	//	m_rom[0x3C9CC] = 0x9E;
	}

	// Mapper 005
	if (m_crc == 0xe91548d8) {	// Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	if (m_crc == 0x255b129c) {	// Gun Sight (J) / Gun Sight (J)[a1]
		m_rom[0x02D0B] = 0x01;
		m_rom[0x0BEC0] = 0x01;
	}


	// Mapper 010
	if (m_crc == 0xc9cce8f2) {	// Famicom Wars (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 016
	if (m_crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament (J)
	 || m_crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi (J)
	 || m_crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai (J)
	 || m_crc == 0xbe06853f		// Datach - J League Super Top Players (J)
	 || m_crc == 0x0be0a328		// Datach - SD Gundam - Gundam Wars (J)
	 || m_crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight! (J)
	 || m_crc == 0xf51a7f46		// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai (J)
	 || m_crc == 0x31cd9903		// Dragon Ball Z - Kyoushuu! Saiya Jin (J)
	 || m_crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!! (J)
	 || m_crc == 0x09499f4d		// Dragon Ball Z 3 - Ressen Jinzou Ningen (J)
	 || m_crc == 0x2e991109		// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
	 || m_crc == 0x170250de) {	// Rokudenashi Blues(J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 019
	if (m_crc == 0x3296ff7a		// Battle Fleet (J)
	 || m_crc == 0x429fd177		// Famista '90 (J)
	 || m_crc == 0xdd454208		// Hydlide 3 - Yami Kara no Houmonsha (J)
	 || m_crc == 0xb1b9e187		// Kaijuu Monogatari (J)
	 || m_crc == 0xaf15338f) {	// Mindseeker (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 026
	if (m_crc == 0x836cc1ab) {	// Mouryou Senki Madara (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 033
	if (m_crc == 0x547e6cc1) {	// Flintstones - The Rescue of Dino & Hoppy, The(J)
		m_mapperType = 48;
	}

	// Mapper 065
	if (m_crc == 0xfd3fc292) {	// Ai Sensei no Oshiete - Watashi no Hoshi (J)
		m_mapperType = 32;
	}

	// Mapper 068
	if (m_crc == 0xfde79681) {	// Maharaja (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 069
	if (m_crc == 0xfeac6916		// Honoo no Toukyuuji - Dodge Danpei 2(J)
	 || m_crc == 0x67898319) {	// Barcode World(J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 080
	if (m_crc == 0x95aaed34		// Mirai Shinwa Jarvas (J)
	 || m_crc == 0x17627d4b) {	// Taito Grand Prix - Eikou heno License (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 082
	if (m_crc == 0x4819a595) {	// Kyuukyoku Harikiri Stadium - Heisei Gannen Ban (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 086
	if (m_crc == 0xe63f7d0b) {	// Urusei Yatsura - Lum no Wedding Bell(J)
		m_mapperType = 101;
	}

	// Mapper 118
	if (m_crc == 0x3b0fb600) {	// Ys 3 - Wonderers From Ys (J)
		m_header->m_flagsA |= NesDiskHeader::BatteryBackedRamFlagA;
	}

	// Mapper 180
	if (m_crc == 0xc68363f6) {	// Crazy Climber(J)
		m_header->m_flagsA &= 0xf6;
	}

	// VS-Unisystem
	if (m_crc == 0x70901b25) {	// VS Slalom
		m_mapperType = 99;
	}

	if (m_crc == 0xd5d7eac4) {	// VS Dr. Mario
		m_mapperType = 1;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0xffbef374		// VS Castlevania
	 || m_crc == 0x8c0c2df5) {	// VS Top Gun
		m_mapperType = 2;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0xeb2dba63		// VS TKO Boxing
	 || m_crc == 0x98cfe016		// VS TKO Boxing (Alt)
	 || m_crc == 0x9818f656) {	// VS TKO Boxing (f1)
		m_mapperType = 4;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0x135adf7c) {	// VS Atari RBI Baseball
		m_mapperType = 4;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0xf9d3b0a3		// VS Super Xevious
	 || m_crc == 0x9924980a		// VS Super Xevious (b1)
	 || m_crc == 0x66bb838f) {	// VS Super Xevious (b2)
		m_mapperType = 4;
		m_header->m_flagsA &= 0xF6;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0x17ae56be) {	// VS Freedom Force
		m_mapperType = 4;
		m_header->m_flagsA &= 0xF6;
		m_header->m_flagsA |= NesDiskHeader::FourScreenFlagA;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0xe2c0a2be) {	// VS Platoon
		m_mapperType = 68;
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_crc == 0xcbe85490		// VS Excitebike
	 || m_crc == 0x29155e0c		// VS Excitebike (Alt)
	 || m_crc == 0xff5135a3) {	// VS Hogan's Alley
		m_header->m_flagsA &= 0xF6;
		m_header->m_flagsA |= NesDiskHeader::FourScreenFlagA;
	}

	if (m_crc == 0x0b65a917) {	// VS Mach Rider(Endurance Course)
		m_rom[0x7FDF] = 0xFF;
		m_rom[0x7FE4] = 0x00;
	}

	if (m_crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
	 || m_crc == 0xae8063ef) {	// VS Mach Rider(Japan, Fighting Course)
		m_rom[0x7FDD] = 0xFF;
		m_rom[0x7FE2] = 0x00;
	}

	if (m_crc == 0x16d3f469) {	// VS Ninja Jajamaru Kun (J)
		m_header->m_flagsA &= 0xf6;
		m_header->m_flagsA |= NesDiskHeader::VerticalFlagA;
	}

	if (m_crc == 0xc99ec059) {	// VS Raid on Bungeling Bay(J)
		m_mapperType = 99;
		m_header->m_flagsA &= 0xF6;
		m_header->m_flagsA |= NesDiskHeader::FourScreenFlagA;
	}
	if (m_crc == 0xca85e56d) {	// VS Mighty Bomb Jack(J)
		m_mapperType = 99;
		m_header->m_flagsA &= 0xF6;
		m_header->m_flagsA |= NesDiskHeader::FourScreenFlagA;
	}


	if (m_crc == 0xeb2dba63		// VS TKO Boxing
	 || m_crc == 0x9818f656		// VS TKO Boxing
	 || m_crc == 0xed588f00		// VS Duck Hunt
	 || m_crc == 0x8c0c2df5		// VS Top Gun
	 || m_crc == 0x16d3f469		// VS Ninja Jajamaru Kun
	 || m_crc == 0x8850924b		// VS Tetris
	 || m_crc == 0xcf36261e		// VS Sky Kid
	 || m_crc == 0xe1aa8214		// VS Star Luster
	 || m_crc == 0xec461db9		// VS Pinball
	 || m_crc == 0xe528f651		// VS Pinball (alt)
	 || m_crc == 0x17ae56be		// VS Freedom Force
	 || m_crc == 0xe2c0a2be		// VS Platoon
	 || m_crc == 0xff5135a3		// VS Hogan's Alley
	 || m_crc == 0x70901b25		// VS Slalom
	 || m_crc == 0x0b65a917		// VS Mach Rider(Endurance Course)
	 || m_crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
	 || m_crc == 0xae8063ef		// VS Mach Rider(Japan, Fighting Course)
	 || m_crc == 0xcc2c4b5d		// VS Golf
	 || m_crc == 0xa93a5aee		// VS Stroke and Match Golf
	 || m_crc == 0x86167220		// VS Lady Golf
	 || m_crc == 0xffbef374		// VS Castlevania
	 || m_crc == 0x135adf7c		// VS Atari RBI Baseball
	 || m_crc == 0xd5d7eac4		// VS Dr. Mario
	 || m_crc == 0x46914e3e		// VS Soccer
	 || m_crc == 0x70433f2c		// VS Battle City
	 || m_crc == 0x8d15a6e6		// VS bad .nes
	 || m_crc == 0x1e438d52		// VS Goonies
	 || m_crc == 0xcbe85490		// VS Excitebike
	 || m_crc == 0x29155e0c		// VS Excitebike (alt)
	 || m_crc == 0x07138c06		// VS Clu Clu Land
	 || m_crc == 0x43a357ef		// VS Ice Climber
	 || m_crc == 0x737dd1bf		// VS Super Mario Bros
	 || m_crc == 0x4bf3972d		// VS Super Mario Bros
	 || m_crc == 0x8b60cc58		// VS Super Mario Bros
	 || m_crc == 0x8192c804		// VS Super Mario Bros
	 || m_crc == 0xd99a2087		// VS Gradius
	 || m_crc == 0xf9d3b0a3		// VS Super Xevious
	 || m_crc == 0x9924980a		// VS Super Xevious
	 || m_crc == 0x66bb838f		// VS Super Xevious
	 || m_crc == 0xc99ec059		// VS Raid on Bungeling Bay(J)
	 || m_crc == 0xca85e56d) {	// VS Mighty Bomb Jack(J)
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}

	if (m_mapperType == 99 || m_mapperType == 151) {
		m_header->m_flagsB |= NesDiskHeader::VSSystemFlagB;
	}
}
