#ifndef _DC_CONNECTION_H_
#define _DC_CONNECTION_H_

#include <sys/socket.h>
#include <netinet/in.h>

typedef	struct	connection_
{
	int			sock_fd;
	struct		in_addr	dest_addr;
	uint16_t	src_port;
	struct		in_addr src_addr;
	uint16_t	dest_port;
}connection_t;

#endif /* _DC_CONNECTION_H_ */
