#ifndef _DC_CONNECTION_H_
#define _DC_CONNECTION_H_

typedef	struct	connection_
{
	int			sock_fd;
	struct	in_addr	dest_addr;
	uint16_t	port;
}connection_t;