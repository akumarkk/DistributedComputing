#include <stdio.h>
#include <stdlib.h>

#include "dc_parse_utils.h"
#include "dc_connection.h"
#include "dc_tlv_defs.h"
#include "dc_msgdef.h"

#define DC_SERVER_CONFIG_FILE "dc_server.conf"
#define DC_SERVER_MAX_CLIENT_SUPPORTED 100
#define MAX_AVAILABLE_GROUPS 10

typedef enum client_state_
{
	DC_SERVER_ADDME_RECEIVED,
	DC_SERVER_PUBLISH_GROUP_SENT,
	DC_SERVER_JOINED_GROUPS_RECEIVED,
	DC_SERVER_PROBLEM_REQUEST_SENT
} client_state_t;

client_data_t	connected_clients[DC_SERVER_MAX_CLIENT_SUPPORTED];
client_id_t 	client_index;

is_client_new(struct	in_addr client_addr, uint16_t	client_port)
{
	int i = 0;
	for(i=1; i <= client_index; i++)
	{
		if(memcmp(&connected_clients[i].in_adr, &client_addr, sizeof(client_addr)) == 0)
			if( connected_clients[i].in_port == client_port )
			return 0;
	}

	return 1;
}


int
dc_msg_server_addme_message(
		connection_t 	conn,
		void 			*payload, 
		uint16_t 		payload_len,
		void 			**return_payload, 
		uint16_t		return_len)
{
	char		publish_groups[MAX_AVAILABLE_GROUPS * 16];
	char		*group = NULL, *pos = NULL;
	char		buf[1024 * 5] = "";
	uint16_t	total_len = 0;
	uint16_t	nbytes = 0;
	struct	sockaddr_in	send_addr;
	int			ret = -1;
	avl_t		av;

	FILE		*fp = fopen(DC_SERVER_CONFIG_FILE, "r");
	dc_msg_t	*dc_msg;

	/* Anyway there will be nothing in the payload of addme msg */
	if(!is_client_new(conn.src_addr, conn.src_port))
	{
		client_index++;
		connected_clients[client_index].clientid = client_index;
		connected_clients[client_index].in_port = conn.src_port;
		connected_clients[client_index].in_adr = conn.src_addr;
		connected_clients[client_index].state = DC_SERVER_ADDME_RECEIVED;
	}

	ret = get_value_of_option(fp, sAvailableGroups, publish_groups);
	if(ret == -1)
	{
		printf("%s : Failed to get Available Groups\n", __FUNCTION__);
		return -1;
	}

	if(group = strtok(publish_groups, " ") == NULL)
	{
		printf("%s: No AvailableGroups found in the %s\n", __FUNCTION__, DC_SERVER_CONFIG_FILE);
		return -1;
	}
	
	/* Start filling the buffer with the group info.
	 * 
	 *  type 	= DC_ATTR_MULTICAST_GROUP
	 *	length  = length of group
	 *	value 	= Group
	 */
	av.attr_id = htonl(DC_ATTR_MULTICAST_GROUP);
	av.value_len = htons(strlen(group));
	memcpy(&av.value, group, strlen(group));
	
	total_len = sizeof(avl_t) + strlen(group);
	memcpy(pos, &av, total_len);
	pos = pos + total_len;
	
	while(group = strtok(NULL, " "))
	{
		uint16_t	len = 0;

		len = strlen(group);
		total_len += ( len + sizeof(avl_t));

		av.attr_id = htonl(DC_ATTR_MULTICAST_GROUP);
		av.value_len = htons(len);
		memcpy(&av.value, group, strlen(group));
		
		memcpy(pos, &av, total_len);
		pos+=(len + sizeof(avl_t));

	}

	dc_msg = malloc(sizeof(dc_msg_t) + total_len);
	dc_msg->msg_type = htonl(DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS);
	dc_msg->payload_len = htonl(total_len);
	memcpy(dc_msg->payload, buf, total_len);

	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(conn.src_port);
	send_addr.sin_addr = conn.src_addr;
	nbytes = sendto(AF_INET, buf, sizeof(dc_msg_t) + total_len, 0, &send_addr, sizeof(send_addr));

	if(nbytes == -1)
	{
		perror("sendto");
		return -1;
	}
	printf("Sent DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS message(%d) successfully\n", nbytes);

	return 0;
}
