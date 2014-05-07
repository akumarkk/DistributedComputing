#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

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

