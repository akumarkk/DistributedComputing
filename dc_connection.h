#ifndef _DC_CONNECTION_H_
#define _DC_CONNECTION_H_

#include <sys/socket.h>
#include <netinet/in.h>

typedef	uint16_t client_id_t;

typedef	struct	connection_
{
	int			sock_fd;
	struct		in_addr	dest_addr;
	uint16_t	src_port;
	struct		in_addr src_addr;
	uint16_t	dest_port;
}connection_t;

typedef	struct	group_
{
	uint8_t		group_len;
	char		*group_ip;
}group_t;

typedef	struct	client_data_
{
    client_id_t		clientid;
	uint16_t	status; /*ACTIVE or BUSY */
	uint16_t	state; /*addme received, groups joined received */
	uint16_t	in_port;
	struct	in_addr in_adr;
	uint8_t		no_of_groups;
	group_t		**groups;
}client_data_t;

#endif /* _DC_CONNECTION_H_ */
