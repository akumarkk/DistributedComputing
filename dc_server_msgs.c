#include <stdio.h>
#include <stdlib.h>

#include "dc_parse_utils.h"
#include "dc_connection.h"
#include "dc_tlv_defs.h"
#include "dc_msgdef.h"
#include "dc_server_msgs.h"

#define DC_SERVER_CONFIG_FILE "dc_server.conf"
#define DC_SERVER_MAX_CLIENT_SUPPORTED 100
#define MAX_AVAILABLE_GROUPS 10

#pragma weak get_message_from_msgid = server_get_message_from_msgid
#pragma weak get_msghandler_from_type = server_get_msghandler_from_type

typedef enum client_state_
{
	DC_SERVER_ADDME_RECEIVED,
	DC_SERVER_PUBLISH_GROUP_SENT,
	DC_SERVER_JOINED_GROUPS_RECEIVED,
	DC_SERVER_PROBLEM_REQUEST_SENT
} client_state_t;

/* Server message handler table */
msg_table_t server_msg_table[] =
{
        { DC_MSG_HANDSHAKE_ADDME,   dc_msg_server_addme_message  },
        { DC_MSG_HEARTBEAT_MESSAGE, dc_msg_heartbeat_message },
        { DC_MSG_PROBLEM_REQUEST,   dc_msg_problem_request   }
};


/* Server dc message logs */
dc_msgid_log_t  dc_msgid_messages[] =
{
    { DC_MSG_HANDSHAKE_ADDME,   "HANDSHAKE MESSAGE - Add me message"},
    { DC_MSG_HEARTBEAT_MESSAGE, "HEARDT-BEAT MESSAGE - Keep alive message" },
    { DC_MSG_PROBLEM_REQUEST,   "PRPBLEM_REQUEST_MESSAGE - Problem to be solved"}
};


client_data_t	connected_clients[DC_SERVER_MAX_CLIENT_SUPPORTED];
client_id_t 	client_index;

char *
server_get_message_from_msgid(msg_t    msgid)
{
    int     n=0, i=0;
    static  char    *msg = "Unknown msgid";

    n = sizeof(dc_msgid_messages)/ sizeof(dc_msgid_log_t);
    for(i = 0; i<n; i++)
    {
        if(dc_msgid_messages[i].msg_id == msgid)
            return dc_msgid_messages[i].message;
    }
    
    return msg;
}

msg_handler_t
server_get_msghandler_from_type(msg_t  msg_id)
{
    msg_handler_t   handler = NULL;
    uint16_t        n=0, i=0;

    n = sizeof(server_msg_table) / sizeof(server_msg_table);
    for(i = 0; i<n; i++)
    {
        if(server_msg_table[i].msg_id == msg_id)
        {
            handler = server_msg_table[i].msg_handler;
            //printf("Found handler fot msg_id : %d\n", msg_id);
            break;
        }
    }

    return handler;
}


is_client_new(struct	in_addr client_addr, uint16_t	client_port)
{
	int i = 0;

	if(client_index == 0)
	{
		printf("First client connection\n");
		return 1;
	}

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
	int16_t	nbytes = 0;
	struct	sockaddr_in	send_addr;
	int			ret = -1;
	avl_t		*av = NULL;

	FILE		*fp = fopen(DC_SERVER_CONFIG_FILE, "r");
	dc_msg_t	*dc_msg;

	/* Anyway there will be nothing in the payload of addme msg */
	printf("%s: preparing to publish available groups on socket %d...\n", __FUNCTION__, conn.sock_fd);
	if(is_client_new(conn.src_addr, conn.src_port))
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
	
	char *ptr = strdup(publish_groups);
	//printf("publish_groups = %s %s\n", publish_groups, ptr);
	if((group = strtok(ptr, ",")) == NULL)
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
	av = malloc(sizeof(avl_t) + strlen(publish_groups));
	av->attr_id = htonl(DC_ATTR_MULTICAST_GROUP);
	av->value_len = htons(strlen(group));
	memcpy(av->value, group, strlen(group));
#ifdef DUMP_AVP
	printf("attr_id		:	%d \n", av->attr_id);
	printf("value_len	:	%d \n", av->value_len);
	printf("value		:	%s \n", av->value);
#endif
	
	total_len = sizeof(avl_t) + strlen(group);
	pos = buf;
	memcpy(pos, av, total_len);
	pos = pos + total_len;
	
	while(group = strtok(NULL, " "))
	{
		uint16_t	len = 0;

		len = strlen(group);
		total_len += ( len + sizeof(avl_t));

		av->attr_id = htonl(DC_ATTR_MULTICAST_GROUP);
		av->value_len = htons(len);
		memcpy(av->value, group, strlen(group));
		
		memcpy(pos, av, total_len);
		pos+=(len + sizeof(avl_t));
#ifdef DUMP_AVP
    printf("attr_id     :   %d  %d\n", av->attr_id, DC_ATTR_MULTICAST_GROUP);
    printf("value_len   :   %d 	%d\n", av->value_len, len);
    printf("value       :   %s \n", av->value);
#endif
	}

	dc_msg = malloc(sizeof(dc_msg_t) + total_len);
	dc_msg->msg_type = htonl(DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS);
	dc_msg->payload_len = htonl(total_len);
	memcpy(dc_msg->payload, buf, total_len);

	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(conn.src_port);
	send_addr.sin_addr = conn.src_addr;
	nbytes = sendto(conn.sock_fd, buf, sizeof(dc_msg_t) + total_len, 0, &send_addr, sizeof(send_addr));

	if(nbytes == -1)
	{
		perror("sendto");
		return -1;
	}
	printf("Sent DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS message(%d) successfully\n", nbytes);

	return 0;
}


int
dc_msg_heartbeat_message(
        connection_t    conn,
        void            *payload,
        uint16_t        payload_len,
        void            **return_payload,
        uint16_t        return_len)
{
	printf("%s Processing heart-beat message\n", __FUNCTION__);
	return 0;
}


int
dc_msg_problem_request(
        connection_t    conn,
        void            *payload,
        uint16_t        payload_len,
        void            **return_payload,
        uint16_t        return_len)
{
	printf("%s Processing problem request message\n", __FUNCTION__);
	return 0;
}


