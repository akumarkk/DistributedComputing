#ifndef _DC_PARSE_UTILS_H_
#define _DC_PARSE_UTILS_H_

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <ctype.h>

#define FAILURE -1

typedef struct	keyword_
{
	char		key[256];
	uint16_t	opcode;
}keyword_t;

typedef	enum opcode_
{
	sBindAddress,
	sBindport,
	sJoinAddr,
	sJoinPort,
	cDestMulticasteAddr,
	cDestMultiCasteport,
	BadOption
}opcode_t;

/*keyword_t	keywords[] = 
{
	{ "sServerBindAddress", sBindAddress },
	{ "sServerBindPort", 	sBindport },
	{ "sServerJoinAddress", sJoinAddr },
	{ "sServerJoinPort", sJoinPort },
	{ "cDestMultiCasteAddress", cDestMulticasteAddr },
	{ "cDestMultiCastePort", cDestMultiCasteport }
};*/

#endif /* _DC_PARSE_UTILS_H_ */
