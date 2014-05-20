#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include "dc_connection.h"
#include "dc_msgdef.h"
#include "dc_parse_utils.h"
#include "client_info.h"
#include "dc_packet.h"


#define CLIENT_CONFIG_FILE 			"dc_client.conf"
#define PARSE_SERVER_ADDRESS_TAG 	"multi_cast_server"
#define SUCCESS 0
#define FAILURE -1

connection_t connection;

int
client_test_message(int sock_fd);

void
init_connection(connection_t *conn, int fd)
{
	struct	sockaddr_in	myaddr;
	struct	in_addr dest_adr;
	int		ret = 0, len = sizeof(myaddr);
	char	server[16] = "";
	char	port_str[10] = "";
	uint16_t	port = 0;

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
	close(fp);
	fp = fopen(CLIENT_CONFIG_FILE, "r");
	
	ret = get_value_of_option(fp, cDestMultiCasteport, port_str);
	if(ret < 0)
	{
		printf("%s: Failed to get dest port\n", __FUNCTION__);
		goto end;
	}
	port = atoi(port_str);

	printf("Initialising connection to %s@%u...\n", server, htons(port));

	inet_pton(AF_INET, server, &dest_adr);
	conn->dest_addr = dest_adr;
	conn->dest_port = port;
end:
	fclose(fp);
	return;
}



dc_msg_t *
retainer_compose_addme_message()
{
	addme_info_t	msg;
	char			*payload = NULL;
	dc_msg_t		*addme_msg = NULL;

	/* Message buffer = type
	 *					length of payload
	 *					Payload
	 */
	addme_msg = malloc(sizeof(dc_msg_t) + sizeof(addme_info_t));
	addme_msg->msg_type = htonl( DC_MSG_HANDSHAKE_ADDME );
	addme_msg->payload_len = sizeof( addme_info_t );
	memset(addme_msg->payload, 0, sizeof(addme_info_t));

#ifdef DUMP
	int i=0;
	unsigned char *ch = (char *)addme_msg;
	int len = (sizeof(dc_msg_t) + sizeof(addme_info_t));
	printf("# of bytes = %d\n", len);
	for(i=0; i< len; i++)
		printf("%02x ", *(ch++));
	printf("\n");
#endif

	return addme_msg;
}

int
send_message(connection_t *conn, const char *msg, uint32_t len)
{
	int		nbytes = 0;
	struct	sockaddr_in	dest_addr;

	memset(&dest_addr, 0, sizeof(dest_addr));

	if(msg == NULL || (len == 0))
		return -1;

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_addr = conn->dest_addr;
	dest_addr.sin_port = htons(conn->dest_port);

	nbytes = sendto(conn->sock_fd, msg, len, 0, &dest_addr, sizeof(dest_addr));
	return nbytes;
}

int
client_loop(connection_t conn)
{
	struct		sockaddr_in	from_addr;
	buffer_t	*recv_buf = NULL;
	char		from_ip[16] = "";
	int			nbytes = 0;
	int			len = sizeof(from_addr);

	recv_buf = get_buffer(MAX_PACKET_SIZE);

	while(1)
	{
		printf("%s: Waiting to receive message ...\n", __FUNCTION__);
		nbytes = recvfrom(conn.sock_fd, recv_buf->buf, MAX_PACKET_SIZE, 0, &from_addr, &len);
		if(nbytes == -1)
		{
			perror("recvfrom");
			return -1;
		}

		conn.src_addr = from_addr.sin_addr;
		conn.src_port = ntohs(from_addr.sin_port);
		inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));

		printf("Received %d bytes from %s@%hu\n", nbytes, from_ip, ntohs(from_addr.sin_port));
#ifdef DUMP
		int i=0;                                                                                                       
		unsigned char *ch = (char *)recv_buf->buf;                                                                            
		printf("# of bytes : %d\n", nbytes);
		for(i=0; i< nbytes; i++)                                                
			printf("%02x ", *(ch++));
		printf("\n");                                                                          
#endif
		process_message(conn, recv_buf);

	}

	return 0;
}

	


	
int
main()
{
	int			sock_fd = -1, ret = FAILURE;
	dc_msg_t 	*add_msg;	
	char 		server[1024] = "";


	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock_fd == -1)
	{
		perror("SOcket");
		return -1;
	}

	init_connection(&connection, sock_fd);
	/*if(client_test_message(sock_fd) != 0)
	{
		printf("Testing failed..\n");
		return -1;
	}*/
	
	add_msg = retainer_compose_addme_message();
	if(add_msg)
		ret = send_message(&connection, add_msg, sizeof(dc_msg_t) + sizeof(addme_info_t));

	if(ret == -1)
	{
		perror("Sendto");
		return -1;
	}
	printf("Sent %d bytes Successfully\n", ret);
	
	client_loop(connection);

	{
		struct	sockaddr_in	from_addr;
		char	from_ip[16] = "";
		char	msg[1024] = "";
		int		len = sizeof(from_addr);

		printf("Waiting to receive message:\n");

		ret = recvfrom(sock_fd, msg, sizeof(msg), 0, &from_addr, &len);
		inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));
		printf("********************************************************\n");
		printf("* \tReceived %d bytes from %s:%d\n", ret, from_ip, ntohs(from_addr.sin_port));
		printf("* \tMessage: %s\n", msg);
		printf("********************************************************\n");
	}

	return 0;
}


int
client_test_message(int sock_fd)
{
    struct  	sockaddr_in from_addr;
    uint16_t    from_addr_len = 0;
    int         nbytes = 0;
    char        msg[1024] = "";
	char		from_ip[1024] = "";

    printf("Testing at Socket %d...\n", sock_fd);

    from_addr_len = sizeof(from_addr);
	strcpy(msg, "Hi, I am a retainer. Trying to Connect to the server");
	
	from_addr.sin_family = AF_INET;
	from_addr.sin_port = htons(45);
	from_addr.sin_addr.s_addr = inet_addr("225.12.13.14");

    nbytes = sendto(sock_fd, msg, strlen(msg), 0, &from_addr, from_addr_len);
    if(nbytes == -1)
    {
        perror("sendto");
        return -1;
    }
    printf("Successfully send %d bytes to 225.12.13.14\n", nbytes);

	memset(msg, 0, sizeof(msg));
    nbytes = recvfrom(sock_fd, msg, sizeof(msg), 0, &from_addr, &from_addr_len);
    inet_ntop(AF_INET, &from_addr.sin_addr, from_ip, sizeof(from_ip));

	printf("+-------------------------- CLIENT : TEST MESSAGE -------------------+\n");
    printf("+%s: Read %d bytes from %s@%u\n", __FUNCTION__, nbytes, from_ip, ntohs(from_addr.sin_port));
    printf("+%s: Message %s\n", __FUNCTION__, msg);
	printf("+-------------------------- CLIENT : END MESSAGE --------------------+\n");

    //strcpy(msg, "I have received your message retainer. Thank you!!!");

   // nbytes = sendto(sock_fd, msg, strlen(msg), 0, &from_addr, sizeof(from_addr));
    //printf("Successfully send %d bytes to %s\n", nbytes, from_ip);

    return 0;
}


