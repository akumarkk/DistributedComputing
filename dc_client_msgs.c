#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "dc_msgdef.h"
#include "dc_client_msgs.h"
#include "dc_tlv_defs.h"

#pragma weak get_message_from_msgid = client_get_message_from_msgid
#pragma weak get_msghandler_from_type = client_get_msghandler_from_type
//#pragma weak dc_msg_handshake_addme = dc_msg_server_addme_message

msg_table_t	client_msg_table[] = 
{
		{ DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS, 	dc_msg_clientprocess_availgroup_message	 },
		{ DC_MSG_HEARTBEAT_MESSAGE, dc_msg_heartbeat_message },
		{ DC_MSG_PROBLEM_REQUEST, 	dc_msg_problem_request	 }
};


dc_msgid_log_t	dc_client_msgid_messages[] = 
{
	{ DC_MSG_HANDSHAKE_ADDME, 	"HANDSHAKE_MESSAGE - Add me message"},
	{ DC_MSG_SERVER_PUBLISH_AVAILABLE_GROUPS, "HANDSHAKE_MESSAGE - Publishing available Groups" },
	{ DC_MSG_HEARTBEAT_MESSAGE, "HEARDT-BEAT MESSAGE - Keep alive message" },
	{ DC_MSG_PROBLEM_REQUEST,	"PRPBLEM_REQUEST_MESSAGE - Problem to be solved"}
};

char *
client_get_message_from_msgid(msg_t	msgid)
{
	int		n=0, i=0;
	static	char	*msg = "Unknown msgid";

	n = sizeof(dc_client_msgid_messages)/ sizeof(dc_msgid_log_t);
	for(i = 0; i<n; i++)
	{
		if(dc_client_msgid_messages[i].msg_id == msgid)
			return dc_client_msgid_messages[i].message;
	}

	return msg;
}

msg_handler_t
client_get_msghandler_from_type(msg_t	msg_id)
{
	msg_handler_t	handler = NULL;
	uint16_t		n=0, i=0;

	n = sizeof(client_msg_table) / sizeof(client_msg_table);
    for(i = 0; i<n; i++)
    {
        if(client_msg_table[i].msg_id == msg_id)
		{
            handler = client_msg_table[i].msg_handler;
    		//printf("Found handler fot msg_id : %d\n", msg_id);
			break;
		}
	}

	return handler;
}

/*int
dc_msg_server_addme_message(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len);
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

} 
*/

int
dc_msg_heartbeat_message(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

int
dc_msg_problem_request(
		connection_t conn,	
        char        *payload,
        uint16_t    payload_len,
        char        **return_payload,
        uint16_t    return_payload_len)
{
    printf("%s: Processing addme message\n", __FUNCTION__);
    return 0;

}

int
dc_msg_clientprocess_availgroup_message(
		connection_t 	conn,
		void 			*payload,
		uint16_t		payload_len,
		void			**return_payload,
		uint16_t		return_len)
{
	avl_t		*group_avp = malloc(sizeof(avl_t) + DC_MAX_GROUP_SIZE);
	char		*ptr = payload;
	attr_t		attr = 0;
	char		join_ip[17] = "";
	char		*send_buf=NULL, *send_pos=NULL;
	struct		ip_mreq join_req;
	uint16_t	total_len = 0, len = 0;
	int			ret = -1;
	uint16_t	value_len = 0;

	printf("%s Processing AVAILABLE_GROUPS messages\n", __FUNCTION__);
	
	/* Send buffer size will be atmost sizeof the received payload
	 * as # of join groups will be <= requested groups.
	 */
	send_buf = malloc(payload_len);
	while(total_len < payload_len)
	{
		printf("In while processing avp\n");
		memcpy(&attr, ptr, sizeof(attr_t));
		attr = ntohl(attr);
		memcpy(&value_len, ptr+sizeof(attr_t), 2);
		printf("value_len %u %u\n", value_len, ntohs(value_len));
		value_len = ntohs(value_len);
		//memcpy(group_avp, ptr, sizeof(avl_t) + 12);
		
		memcpy(group_avp->value, ptr + 2 + sizeof(attr_t),  value_len);
		len = sizeof(avl_t) + ntohs(group_avp->value_len);
#ifdef DUMP_AVP
		printf("attr		:	%d	%d\n", attr, htonl(attr));
		printf("value_len	:	%d	%d\n", value_len, htons(value_len));
		printf("Value		:	%s\n", group_avp->value);
#endif
		printf("Copied group_avp: len = %d \n", ntohs(group_avp->value_len));
		strncpy(join_ip, group_avp->value, ntohs(group_avp->value_len));
		printf("Copied group_avp, join ip %s\n", join_ip);
		
		join_req.imr_multiaddr.s_addr = inet_addr(join_ip);
		join_req.imr_interface.s_addr = INADDR_ANY;
		
		ret = setsockopt(conn.sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_ip, sizeof(join_ip));
		if(ret == -1)
		{
			printf("Join failed for ip %s err: %s\n", join_ip, strerror(errno));
		}
		printf("Joined IP %s\n", join_ip);
		
		memcpy(send_pos, group_avp, len);
		send_pos += len;
		ptr = ptr + len;
		total_len += len;
	}


	dc_msg_t	*msg = malloc(sizeof(dc_msg_t) + total_len);
	msg->msg_type = DC_MSG_CLIENT_JOINED_GROUPS;
	msg->payload_len = total_len;
	memcpy(msg->payload, send_buf, total_len);

	ret = send_message(&conn, send_buf, total_len + sizeof(dc_msg_t));
	if(ret < 1)
	{
		printf("%s: Sendto failed!!!\n", __FUNCTION__);
		return -1;
	}

	return 0;
}
