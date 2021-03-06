#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "dc_connection.h"
#include "dc_parse_utils.h"
#include "dc_msgdef.h"

#define CLIENT_CONFIG_FILE 			"dc_client.conf"
#define PARSE_SERVER_ADDRESS_TAG 	"multi_cast_server"
#define SUCCESS 0
#define FAILURE -1


int
get_mcast_server(char	*server)
{
	FILE 	*fp = fopen(CLIENT_CONFIG_FILE, "r");
	int		len = 0;
	char	f_line[1024], *value = NULL, *line=NULL;

	if(fp == NULL || (server == NULL))
		return FAILURE;

	while( value=fgets(f_line, sizeof(f_line), fp) )
	{
		//printf("line : %s\n", f_line);
		line = f_line;

		if(value = strchr(line, '='))
		{
			*(value) = '\0';
			value++;
			/* Remove the leading white spaces */
			while( isspace(value[0]) )
				value++;

			/*Remove the trailing white spaces */
			len = strlen(value);
			while( isspace(value[len - 1]) )
				value[--len] = '\0';

			while( isspace(line[0]) )
				line++;
			len = strlen(line);
			while( isspace(line[len-1]) )
				line[--len] = '\0';

			//printf("key = #%s# \tValue = #%s#\n", line, value);

			if(strncmp(line, PARSE_SERVER_ADDRESS_TAG, strlen(PARSE_SERVER_ADDRESS_TAG)) == 0)
			{
				strcpy(server, value);
				return SUCCESS;
			}
		}
		memset(line, 0, sizeof(line));
	}

	return FAILURE;
}

void
init_connection(connection_t *conn, int fd)
{
	struct	sockaddr_in	myaddr;
	struct	in_addr dest_adr;
	int		len = sizeof(myaddr);
	char	server[16] = "";

	memset(&myaddr, 0, sizeof(myaddr));
	memset(&dest_adr, 0, sizeof(dest_adr));

	getsockname(fd, &myaddr, &len);
	conn->sock_fd = fd;
	conn->src_addr = myaddr.sin_addr;
	conn->src_port = ntohs(myaddr.sin_port);

	FILE *fp = fopen(CLIENT_CONFIG_FILE, "r");

	ret = get_value_of_option(fp, cDestMulticasteAddr, server);
	if(ret == -1)
	{
		printf("%s : get_value_of_option failed to get server\n", __FUNCTION__);
		goto end;
	}

	inet_pton(AF_INET, server, &dest_adr);
	conn->dest_addr = dest_adr;
	conn->dest_port = htons(1234);
end:
	fclose(fp);
	return;
}



dc_msg_t *
retainer_compose_addme_message()
{
	addme_info_t	msg;
	char			*payload = NULL;
	unsigned	char*addme_msg = NULL;

	/* Message buffer = type
	 *					length of payload
	 *					Payload
	 */
	addme_msg = malloc(sizeof(msg_t) + 4 + sizeof(addme_info_t));

	memset(&msg, 0, sizeof(addme_info_t));
	memcpy(addme_msg, DC_MSG_HANDSHAKE_ADDME, sizeof(msg_t));
	tmp = addme_msg + sizeof(msg_t);
	memcpy(tmp, sizeof(addme_info_t), 4);

	tmp = tmp + 4;
	memcpy(tmp, msg, sizeof(msg));

	return addme_msg;
}

int
send_message(connection_t *conn, const unsigned char *msg, uint32_t len)
{
	int	nbytes = 0;

	if(msg == NULL || (len == 0))
		return -1;

	nbytes = sendto(conn->sock_fd, msg, len, 0, &conn->dest_addr, sizeof(conn->dest_addr));
	return nbytes;
}

	
int
main()
{
	int			sock_fd = -1, ret = FAILURE;
	dc_msg_t 	*add_msg;	
	char 		server[1024] = "";
	int 		ret=-1; 


	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1)
	{
		perror("SOcket");
		return -1;
	}

	init_connection(connection, sock_fd);
	
	add_msg = retainer_compose_addme_message();
	if(add_msg)
		ret = send_message(&connection, add_msg, sizeof(msg_t) + sizeof(addme_info_t) +4);
	
	printf("Sent %d bytes Successfully\n", ret);

	{
		struct	sockaddr_in	from_addr;
		char	from_ip[16] = "";
		char	msg[1024] = "";
		int		len = sizeof(from_addr);

		ret = recvfrom(sock_fd, msg, sizeof(msg), 0, &from_addr, &len);
		inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));
		printf("********************************************************\n");
		printf("* \tReceived %d bytes from %s:%d\n", ret, from_ip, ntohs(from_addr.sin_port));
		printf("* \tMessage: %s\n", msg);
		printf("********************************************************\n");
	}

	return 0;
}

}

