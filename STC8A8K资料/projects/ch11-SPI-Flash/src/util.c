/*
 * Copyright        : Shendong MCU studio
 * File Description : Help utilities
 * Created          : oliverzhang@shendongmcu.com
 */

#include "global.h"

/**
* strpbrk - Find the first occurrence of a set of characters
* @cs: The string to be searched
* @ct: The characters to search for
*/
char *strpbrk(const char *cs, const char *ct)
{
	const char *sc1, *sc2;

	for (sc1 = cs; *sc1 != '\0'; ++sc1) {
		for (sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (char *)sc1;
		}
	}
	return NULL;
}

/**
* strsep - Split a string into tokens
* @s: The string to be searched
* @ct: The characters to search for
*
* strsep() updates @s to point after the token, ready for the next call.
*
* It returns empty tokens, too, behaving exactly like the libc function
* of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
* Same semantics, slimmer shape. ;)
*/

char *strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}

/*CCITT: 0x11021: X^16 + X^12 + X^5 + 1, implemented nibble by nibble version*/
uint16_t crc_CalcCCITT16NibbleVer(void *vptr, uint16_t len) 
{
	uint8_t *ptr = (uint8_t *)vptr;
	uint16_t crc;
	uint8_t da;
	const uint16_t code crc_ta[16]= {
		0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
		0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	};
	crc=0;
	while(len--!=0) 
	{
		da=((uint8_t)(crc/256))/16;
		crc<<=4;
		crc^=crc_ta[da^(*ptr/16)];
		da=((uint8_t)(crc/256))/16; 
		crc<<=4; 
		crc^=crc_ta[da^(*ptr & 0x0f)]; 
		ptr++;
	}
	return(crc);
}

