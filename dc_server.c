#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAX_PACKET_SIZE (4* 1024)
void
server_loop(connection_t	conn)
{
	int			nbytes = 0, addr_len = 0;
	struct		sockaddr_in	from_addr;
	char		from_ip[16] = "";
	uint16_t 	port=0;
	buffer_t	*recv_buf = NULL;

	recv_buf = get_buffer(MAX_PACKET_SIZE);

	nbytes = recvfrom(conn.sock_fd, recv_buf->buf, MAX_PACKET_SIZE, 0, &from_addr, &addr_len);
	inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));

	printf("Received %d bytes from %s:%hu\n", nbytes, from_ip, ntohs(from_addr.sin_port));
	
	recv_buf->buf_len = nbytes;


int
main()
{
	struct		sockaddr_in	join_addr;
	struct		ip_mreq		join_mc;
	uint16_t	join_port = 0;
	int			nbytes = 0, ret = -1;
	int			sock_fd = -1;
	char		value[1024] = "";

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

	ret = get_value_of_option(fp, sJoinPort, value);
	if(ret == -1)
	{
		printf("Failed to Read Join Port. \nExiting...\n");
		return -1;
	}

	join_port = atoi(value);

	memset(&join_addr, 0, sizeof(join_addr));
	memset(&join_mc, 0, sizeof(join_mc));

	join_addr.sin_family = AF_INET;
	inet_pton(AF_INET, join_ip, &join_addr.sin_addr);

	join_mc.imr_multiaddr = join_addr.sin_addr;
	join_mc.imr_interface = INADDR_ANY;

	printf("Joining Multicast Group : %s@%u\n", join_ip, join_port);

	ret = setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_mc, sizeof(join_mc));
	if(ret == -1)
	{
		printf("Joining Multicast group %s failed\n", join_ip);
		return -1;
	}
	printf("Successfully Joined Multicast Group : %s\n", join_ip);
}
