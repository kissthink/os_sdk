/***************************************************************************
    begin                : Sat Oct 11 2003
    copyright            : (C) 2003 - 2007 by Alper Akcan
    email                : distchx@yahoo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/


#include "../../../lib/xynth_.h"
#include "server.h"
#include "helper.h"
#if defined(VIDEO_HELPER_MODES)

static s_video_helper_mode_info_t s_video_helper_modetable[MAX_MODES] =
{    /*  xdim   ydim    colors   xbytes   bpp  name             mode */
/* 0 */	{80,	25,	16,	 160,	   0, "TEXT",		TEXT},
	{320,	200,	16,	 40,	   0, "320x200x16",	M320x200x16},
	{640,	200,	16,	 80,	   0, "640x200x16",	M640x200x16},
	{640,	350,	16,	 80,	   0, "640x350x16",	M640x350x16},
	{640,	480,	16,	 80,	   0, "640x480x16",	M640x480x16},
	{320,	200,	256,	 320,	   1, "320x200x256",	M320x200x256},
	{320,	240,	256,	 80,	   0, "320x240x256",	M320x240x256},
	{320,	400,	256,	 80,	   0, "320x400x256",	M320x400x256},
	{360,	480,	256,	 90,	   0, "360x480x256",	M360x480x256},
	{640,	480,	2,	 80,	   0, "640x480x2",	M640x480x2},
/* 10 */{640,	480,	256,	 640,	   1, "640x480x256",	M640x480x256},
	{800,	600,	256,	 800,	   1, "800x600x256",	M800x600x256},
	{1024,	768,	256,	 1024,	   1, "1024x768x256",	M1024x768x256},
	{1280,	1024,	256,	 1280,	   1, "1280x1024x256",	M1280x1024x256},
	{320,	200,	1 << 15, 640,	   2, "320x200x32K",	M320x200x32K},
	{320,	200,	1 << 16, 640,	   2, "320x200x64K",	M320x200x64K},
	{320,	200,	1 << 24, 320 * 3,  3, "320x200x16M",	M320x200x16M},
	{640,	480,	1 << 15, 640 * 2,  2, "640x480x32K",	M640x480x32K},
	{640,	480,	1 << 16, 640 * 2,  2, "640x480x64K",	M640x480x64K},
	{640,	480,	1 << 24, 640 * 3,  3, "640x480x16M",	M640x480x16M},
/* 20 */{800,	600,	1 << 15, 800 * 2,  2, "800x600x32K",	M800x600x32K},
	{800,	600,	1 << 16, 800 * 2,  2, "800x600x64K",	M800x600x64K},
	{800,	600,	1 << 24, 800 * 3,  3, "800x600x16M",	M800x600x16M},
	{1024,	768,	1 << 15, 1024 * 2, 2, "1024x768x32K",	M1024x768x32K},
	{1024,	768,	1 << 16, 1024 * 2, 2, "1024x768x64K",	M1024x768x64K},
	{1024,	768,	1 << 24, 1024 * 3, 3, "1024x768x16M",	M1024x768x16M},
	{1280,	1024,	1 << 15, 1280 * 2, 2, "1280x1024x32K",	M1280x1024x32K},
	{1280,	1024,	1 << 16, 1280 * 2, 2, "1280x1024x64K",	M1280x1024x64K},
	{1280,	1024,	1 << 24, 1280 * 3, 3, "1280x1024x16M",	M1280x1024x16M},
	{800,	600,	16,	 100,	   0, "800x600x16",	M800x600x16},
/* 30 */{1024,	768,	16,	 128,	   0, "1024x768x16",	M1024x768x16},
	{1280,	1024,	16,	 160,	   0, "1280x1024x16",	M1280x1024x16},
	{720,	348,	2,	 90,	   0, "720x348x2",	M720x348x2},
	{320,	200,	1 << 24, 320 * 4,  4, "320x200x16M32",	M320x200x16M32},
	{640,	480,	1 << 24, 640 * 4,  4, "640x480x16M32",	M640x480x16M32},
	{800,	600,	1 << 24, 800 * 4,  4, "800x600x16M32",	M800x600x16M32},
	{1024,	768,	1 << 24, 1024 * 4, 4, "1024x768x16M32",	M1024x768x16M32},
	{1280,	1024,	1 << 24, 1280 * 4, 4, "1280x1024x16M32",M1280x1024x16M32},
	{1152,	864,	16,	 144,	   0, "1152x864x16",	M1152x864x16},
	{1152,	864,	256,	 1152,	   1, "1152x864x256",	M1152x864x256},
/* 40 */{1152,	864,	1 << 15, 1152 * 2, 2, "1152x864x32K",	M1152x864x32K},
	{1152,	864,	1 << 16, 1152 * 2, 2, "1152x864x64K",	M1152x864x64K},
	{1152,	864,	1 << 24, 1152 * 3, 3, "1152x864x16M",	M1152x864x16M},
	{1152,	864,	1 << 24, 1152 * 4, 4, "1152x864x16M32",	M1152x864x16M32},
	{1600,	1200,	16,	 200,	   0, "1600x1200x16",	M1600x1200x16},
	{1600,	1200,	256,	 1600,	   1, "1600x1200x256",	M1600x1200x256},
	{1600,	1200,	1 << 15, 1600 * 2, 2, "1600x1200x32K",	M1600x1200x32K},
	{1600,	1200,	1 << 16, 1600 * 2, 2, "1600x1200x64K",	M1600x1200x64K},
	{1600,	1200,	1 << 24, 1600 * 3, 3, "1600x1200x16M",	M1600x1200x16M},
	{1600,	1200,	1 << 24, 1600 * 4, 4, "1600x1200x16M32",M1600x1200x16M32},
/* 50 */{320,	240,	256,	 320,	   1, "320x240x256V",	M320x240x256V},
	{320,	240,	1 << 15, 320 * 2,  2, "320x240x32K",	M320x240x32K},
	{320,	240,	1 << 16, 320 * 2,  2, "320x240x64K",	M320x240x64K},
	{320,	240,	1 << 24, 320 * 3,  3, "320x240x16M",	M320x240x16M},
	{320,	240,	1 << 24, 320 * 4,  4, "320x240x16M32",	M320x240x16M32},
	{400,	300,	256,	 400,	   1, "400x300x256",	M400x300x256},
	{400,	300,	1 << 15, 400 * 2,  2, "400x300x32K",	M400x300x32K},
	{400,	300,	1 << 16, 400 * 2,  2, "400x300x64K",	M400x300x64K},
	{400,	300,	1 << 24, 400 * 3,  3, "400x300x16M",	M400x300x16M},
	{400,	300,	1 << 24, 400 * 4,  4, "400x300x16M32",	M400x300x16M32},
/* 60 */{512,	384,	256,	 512,	   1, "512x384x256",	M512x384x256},
	{512,	384,	1 << 15, 512 * 2,  2, "512x384x32K",	M512x384x32K},
	{512,	384,	1 << 16, 512 * 2,  2, "512x384x64K",	M512x384x64K},
	{512,	384,	1 << 24, 512 * 3,  3, "512x384x16M",	M512x384x16M},
	{512,	384,	1 << 24, 512 * 4,  4, "512x384x16M32",	M512x384x16M32},
	{960,	720,	256,	 960,	   1, "960x720x256",	M960x720x256},
	{960,	720,	1 << 15, 960 * 2,  2, "960x720x32K",	M960x720x32K},
	{960,	720,	1 << 16, 960 * 2,  2, "960x720x64K",	M960x720x64K},
	{960,	720,	1 << 24, 960 * 3,  3, "960x720x16M",	M960x720x16M},
	{960,	720,	1 << 24, 960 * 4,  4, "960x720x16M32",	M960x720x16M32},
/* 70 */{1920,	1440,	256,	 1920,	   1, "1920x1440x256",	M1920x1440x256},
	{1920,	1440,	1 << 15, 1920 * 2, 2, "1920x1440x32K",	M1920x1440x32K},
	{1920,	1440,	1 << 16, 1920 * 2, 2, "1920x1440x64K",	M1920x1440x64K},
	{1920,	1440,	1 << 24, 1920 * 3, 3, "1920x1440x16M",	M1920x1440x16M},
	{1920,	1440,	1 << 24, 1920 * 4, 4, "1920x1440x16M32",M1920x1440x16M32},
	{320,	400,	1 << 8,  320,      1, "320x400x256V",	M320x400x256V},
	{320,	400,	1 << 15, 320 * 2,  2, "320x400x32K",	M320x400x32K},
	{320,	400,	1 << 16, 320 * 2,  2, "320x400x64K",	M320x400x64K},
	{320,	400,	1 << 24, 320 * 3,  3, "320x400x16M",	M320x400x16M},
	{320,	400,	1 << 24, 320 * 4,  4, "320x400x16M32",	M320x400x16M32},
/* 80 */{640,	400,	256,	 640,	   1, "640x400x256",	M640x400x256},
	{640,	400,	1 << 15, 640 * 2,  2, "640x400x32K",	M640x400x32K},
	{640,	400,	1 << 16, 640 * 2,  2, "640x400x64K",	M640x400x64K},
	{640,	400,	1 << 24, 640 * 3,  3, "640x400x16M",	M640x400x16M},
	{640,	400,	1 << 24, 640 * 4,  4, "640x400x16M32",	M640x400x16M32},
	{320,	480,	256,     320,      1, "320x480x256",	M320x480x256},
	{320,	480,	1 << 15, 320 * 2,  2, "320x480x32K",	M320x480x32K},
	{320,	480,	1 << 16, 320 * 2,  2, "320x480x64K",	M320x480x64K},
	{320,	480,	1 << 24, 320 * 3,  3, "320x480x16M",	M320x480x16M},
	{320,	480,	1 << 24, 320 * 4,  4, "320x480x16M32",	M320x480x16M32},
/* 90 */{720,	540,	256,	 720,	   1, "720x540x256",	M720x540x256},
	{720,	540,	1 << 15, 720 * 2,  2, "720x540x32K",	M720x540x32K},
	{720,	540,	1 << 16, 720 * 2,  2, "720x540x64K",	M720x540x64K},
	{720,	540,	1 << 24, 720 * 3,  3, "720x540x16M",	M720x540x16M},
	{720,	540,	1 << 24, 720 * 4,  4, "720x540x16M32",	M720x540x16M32},
	{848,	480,	256,	 848,	   1, "848x480x256",	M848x480x256},
	{848,	480,	1 << 15, 848 * 2,  2, "848x480x32K",	M848x480x32K},
	{848,	480,	1 << 16, 848 * 2,  2, "848x480x64K",	M848x480x64K},
	{848,	480,	1 << 24, 848 * 3,  3, "848x480x16M",	M848x480x16M},
	{848,	480,	1 << 24, 848 * 4,  4, "848x480x16M32",	M848x480x16M32},
/*100 */{1072,	600,	256,	 1072,	   1, "1072x600x256",	M1072x600x256},
	{1072,	600,	1 << 15, 1072 * 2, 2, "1072x600x32K",	M1072x600x32K},
	{1072,	600,	1 << 16, 1072 * 2, 2, "1072x600x64K",	M1072x600x64K},
	{1072,	600,	1 << 24, 1072 * 3, 3, "1072x600x16M",	M1072x600x16M},
	{1072,	600,	1 << 24, 1072 * 4, 4, "1072x600x16M32",	M1072x600x16M32},
	{1280,	720,	256,	 1280,	   1, "1280x720x256",	M1280x720x256},
	{1280,	720,	1 << 15, 1280 * 2, 2, "1280x720x32K",	M1280x720x32K},
	{1280,	720,	1 << 16, 1280 * 2, 2, "1280x720x64K",	M1280x720x64K},
	{1280,	720,	1 << 24, 1280 * 3, 3, "1280x720x16M",	M1280x720x16M},
	{1280,	720,	1 << 24, 1280 * 4, 4, "1280x720x16M32",	M1280x720x16M32},
/*110 */{1360,	768,	256,	 1360,	   1, "1360x768x256",	M1360x768x256},
	{1360,	768,	1 << 15, 1360 * 2, 2, "1360x768x32K",	M1360x768x32K},
	{1360,	768,	1 << 16, 1360 * 2, 2, "1360x768x64K",	M1360x768x64K},
	{1360,	768,	1 << 24, 1360 * 3, 3, "1360x768x16M",	M1360x768x16M},
	{1360,	768,	1 << 24, 1360 * 4, 4, "1360x768x16M32",	M1360x768x16M32},
	{1800,	1012,	256,	 1800,	   1, "1800x1012x256",	M1800x1012x256},
	{1800,	1012,	1 << 15, 1800 * 2, 2, "1800x1012x32K",	M1800x1012x32K},
	{1800,	1012,	1 << 16, 1800 * 2, 2, "1800x1012x64K",	M1800x1012x64K},
	{1800,	1012,	1 << 24, 1800 * 3, 3, "1800x1012x16M",	M1800x1012x16M},
	{1800,	1012,	1 << 24, 1800 * 4, 4, "1800x1012x16M32",M1800x1012x16M32},
/*120 */{1920,	1080,	256,	 1920,	   1, "1920x1080x256",	M1920x1080x256},
	{1920,	1080,	1 << 15, 1920 * 2, 2, "1920x1080x32K",	M1920x1080x32K},
	{1920,	1080,	1 << 16, 1920 * 2, 2, "1920x1080x64K",	M1920x1080x64K},
	{1920,	1080,	1 << 24, 1920 * 3, 3, "1920x1080x16M",	M1920x1080x16M},
	{1920,	1080,	1 << 24, 1920 * 4, 4, "1920x1080x16M32",M1920x1080x16M32},
	{2048,	1152,	256,	 2048,	   1, "2048x1152x256",	M2048x1152x256},
	{2048,	1152,	1 << 15, 2048 * 2, 2, "2048x1152x32K",	M2048x1152x32K},
	{2048,	1152,	1 << 16, 2048 * 2, 2, "2048x1152x64K",	M2048x1152x64K},
	{2048,	1152,	1 << 24, 2048 * 3, 3, "2048x1152x16M",	M2048x1152x16M},
	{2048,	1152,	1 << 24, 2048 * 4, 4, "2048x1152x16M32",M2048x1152x16M32},
/*130 */{2048,	1536,	256,	 2048,	   1, "2048x1536x256",	M2048x1536x256},
	{2048,	1536,	1 << 15, 2048 * 2, 2, "2048x1536x32K",	M2048x1536x32K},
	{2048,	1536,	1 << 16, 2048 * 2, 2, "2048x1536x64K",	M2048x1536x64K},
	{2048,	1536,	1 << 24, 2048 * 3, 3, "2048x1536x16M",	M2048x1536x16M},
	{2048,	1536,	1 << 24, 2048 * 4, 4, "2048x1536x16M32",M2048x1536x16M32},
	{512,	480,	256,	 512,	   1, "512x480x256",	M512x480x256},
	{512,	480,	1 << 15, 512 * 2,  2, "512x480x32K",	M512x480x32K},
	{512,	480,	1 << 16, 512 * 2,  2, "512x480x64K",	M512x480x64K},
	{512,	480,	1 << 24, 512 * 3,  3, "512x480x16M",	M512x480x16M},
	{512,	480,	1 << 24, 512 * 4,  4, "512x480x16M32",	M512x480x16M32},
/*140 */{400,	600,	256,	 400,	   1, "400x600x256",	M400x600x256},
	{400,	600,	1 << 15, 400 * 2,  2, "400x600x32K",	M400x600x32K},
	{400,	600,	1 << 16, 400 * 2,  2, "400x600x64K",	M400x600x64K},
	{400,	600,	1 << 24, 400 * 3,  3, "400x600x16M",	M400x600x16M},
	{400,	600,	1 << 24, 400 * 4,  4, "400x600x16M32",	M400x600x16M32},
	{400,	300,	256,	 100,	   0, "400x300x256X",	M400x300x256X},
	{320,	200,	256,	 320,	   1, "320x200x256V",	M320x200x256V},
	{480,	272,	256,	 480,	   1, "480x272x256",    M480x272x256},
	{480,	272,	1 << 15, 480 * 2,  2, "480x272x32K",    M480x272x32K},
	{480,	272,	1 << 16, 480 * 2,  2, "480x272x64K",    M480x272x64K},
	{480,	272,	1 << 24, 480 * 3,  3, "480x272x16M",    M480x272x16M},
	{480,	272,	1 << 24, 480 * 4,  4, "480x272x16M32",  M480x272x16M32},
	{240,	320,	256,	 240,	   1, "240x320x256",    M240x320x256},
	{240,	320,	1 << 15, 240 * 2,  2, "240x320x32K",    M240x320x32K},
	{240,	320,	1 << 16, 240 * 2,  2, "240x320x64K",    M240x320x64K},
	{240,	320,	1 << 24, 240 * 3,  3, "240x320x16M",    M240x320x16M},
	{240,	320,	1 << 24, 240 * 4,  4, "240x320x16M32",  M240x320x16M32},
	{320,   240,    1 << 18, 320 * 4,  4, "320x240x256K",   M320x240x256K},
	{720,   480,    1 << 16, 720 * 2,  2, "720x480x64K",    M720x480x64K},
	{640,   448,    1 << 16, 640 * 2,  2, "640x448x64K",    M640x448x64K},
	{1280,  800,    1 << 16, 1280 * 2,  2, "1280x800x64K",  M1280x800x64K},
};

int s_video_helper_mode_find (s_server_conf_t *cfg, s_video_helper_mode_info_t **gmode)
{
	int i;
	for (i = 0; i < MAX_MODES; i++) {
		if (strcasecmp(cfg->general.mode, s_video_helper_modetable[i].name) == 0) {
			*gmode = &s_video_helper_modetable[i];
			return 0;
		}
	}
	return -1;
}

#endif /* VIDEO_HELPER_MODES */
