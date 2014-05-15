#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "dc_connection.h"
#include "dc_packet.h"
#include "dc_parse_utils.h"

#define DUMP

#define MAX_PACKET_SIZE (4* 1024)

int
server_test_message(int sock_fd);

connection_t	connection;

int
server_process_addme_message(
		char 		*payload,
		uint16_t	payload_len,
		char		**return_payload,
		uint16_t	return_payload_len)
{
	printf("%s: Processing addme message\n", __FUNCTION__);
	return 0;

}

void
server_loop(connection_t	conn)
{
	int			nbytes = 0, addr_len = 0;
	struct		sockaddr_in	from_addr;
	char		from_ip[16] = "";
	uint16_t 	port=0;
	buffer_t	*recv_buf = NULL;

	recv_buf = get_buffer(MAX_PACKET_SIZE);
	addr_len = sizeof(from_addr);

	printf("Waiting to receive message at %d....\n", conn.sock_fd);
	nbytes = recvfrom(conn.sock_fd, recv_buf->buf, MAX_PACKET_SIZE, 0, &from_addr, &addr_len);
	inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));

	printf("Received %d bytes from %s:%hu\n", nbytes, from_ip, ntohs(from_addr.sin_port));
	
	conn.src_addr = from_addr.sin_addr;
	conn.src_port = ntohs(from_addr.sin_port);

	recv_buf->buf_len = nbytes;
#ifdef DUMP
    int i=0;
    unsigned char *ch = (char *)recv_buf->buf;
	printf("# of bytes : %d\n", nbytes);
    for(i=0; i< nbytes; i++)
        printf("%02x ", *(ch++));
    printf("\n");
#endif

	process_message(conn, recv_buf);

	char msg_to_send[] = "Good, I have received yoor message retainer";

	nbytes = sendto(conn.sock_fd, msg_to_send, strlen(msg_to_send), 0, &from_addr, sizeof(from_addr));
	printf("Successfully sent %d bytes to %s:%d\n", nbytes, from_ip, ntohs(from_addr.sin_port));

	return;
}

#define DC_SERVER_CONFIG_FILE "dc_server.conf"
int
main()
{
	struct		sockaddr_in	myaddr;
	struct		ip_mreq		join_mc;
	uint16_t	bind_port = 0;
	char		join_ip[16] = "";
	int			nbytes = 0, ret = -1;
	int			sock_fd = -1;
	char		value[1024] = "";
	FILE		*fp = fopen(DC_SERVER_CONFIG_FILE, "r");

	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1)
	{
		perror("Socket");
		return -1;
	}

	ret = get_value_of_option(fp, sJoinAddr, join_ip);
	if(ret == -1)
	{
		printf("Failed to Read Join IP Address. \nExiting...\n");
		return -1;
	}

	ret = get_value_of_option(fp, sBindport, value);
	if(ret == -1)
	{
		printf("Failed to Read Bind Port. \nExiting...\n");
		return -1;
	}


	bind_port = atoi(value);
	memset(&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(bind_port);
	myaddr.sin_addr.s_addr = INADDR_ANY;

	ret = bind(sock_fd, &myaddr, sizeof(myaddr));
	if(ret == -1)
	{
		perror("Bind");
		return -1;
	}
	printf("Bound to INADDR_ANY@%u\n", htons(bind_port));

	memset(&join_mc, 0, sizeof(join_mc));
	join_mc.imr_multiaddr.s_addr = inet_addr(join_ip);
	join_mc.imr_interface.s_addr = INADDR_ANY;

	printf("Joining Multicast Group : %s...\n", join_ip);

	ret = setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_mc, sizeof(join_mc));
	if(ret == -1)
	{
		printf("Joining Multicast group %s failed\n", join_ip);
		return -1;
	}
	printf("Successfully Joined Multicast Group : %s at %d\n", join_ip, sock_fd);

	connection.sock_fd = sock_fd;
	connection.src_addr.s_addr = inet_addr(join_ip);

	if(server_test_message(sock_fd) == 0)
		server_loop(connection);
}

int
server_test_message(int sock_fd)
{
	struct	sockaddr_in	from_addr;
	uint16_t	from_addr_len = 0;
	int			nbytes = 0;
	char		msg[1024] = "";
	char		from_ip[1024] = "";

	printf("Testing at Socket %d...\n", sock_fd);

	from_addr_len = sizeof(from_addr);
	nbytes = recvfrom(sock_fd, msg, sizeof(msg), 0, &from_addr, &from_addr_len);
	if(nbytes == -1)
	{
		perror("recvfrom");
		return -1;
	}

	inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));
	printf("\n+-----------------------SERVER:TEST MESSAGE------------------------+\n+");
	printf("%s: Read %d bytes from %s@%u\n", __FUNCTION__, nbytes, from_ip, ntohs(from_addr.sin_port));
	printf("+%s: Message %s\n", __FUNCTION__, msg);
	printf("+-------------------------SERVER: END TEST MESSAGE-------------------+\n");

	strcpy(msg, "I have received your message retainer. Thank you!!!");

	nbytes = sendto(sock_fd, msg, strlen(msg), 0, &from_addr, sizeof(from_addr));
	printf("Successfully send %d bytes to %s\n", nbytes, from_ip);

	return 0;
}
