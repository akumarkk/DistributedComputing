#include <stdio.h>
#include "dc_packet.h"

buffer_t
get_buffer( int size)
{
	if(size == 0)
		return NULL;
	
	buffer_t	*buff = malloc(sizeof(buffer_t));
	buff->buf = malloc(size);
	memset(buff->buf, 0, size);
	buff->buf_len = size;

	return buff;
}
	

int 
process_message(connection_t	conn, buffer_t	*buff)
{
	dc_msg_t		*recv_msg = NULL;
	uint32_t		msg_id = 0;
	msg_handler_t	handler = NULL;
	uint32_t		payload_len = 0;

	recv_msg = malloc(buff->buf_len);
	memcpy(recv_msg, buff->buf, buff->len);

	msg_id = ntohl(recv_msg->msg_type);
	payload_len = ntohl(recv_msg->payload_len);
	
	n = sizeof(server_msg_table) / sizeof(server_msg_table_t);
	for(i = 0; i<n; i++)
	{
		if(server_msg_table[i].msg_id = msg_id)
			handler = server_msg_table[i].msg_handler;
	}
	if(handler == NULL)
	{
		printf("Message handler for msg_id %x not found\n", msg_id);
		return -1;
	}

	ret = handler(recv_msg->payload, payload_len, NULL, 0);

	return ret;
}

