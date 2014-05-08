#ifndef _DC_PACKET_H_
#define _DC_PACKET_H_

#include <stdint.h>
#include <string.h>
#include "dc_connection.h"

typedef	struct	buffer_
{
	unsigned char	*buf;
	uint16_t		buf_len;
}buffer_t;

#endif /* _DC_PACKET_H_ */
