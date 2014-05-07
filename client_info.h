#ifndef _CLIENT_INFO_H_ 
#define _CLIENT_INFO_H_

/* Structure to hold client information */
typedef	struct	client_info_
{
	uint32_t	id;
	int8_t		status;
	char		ip[16];
	uint16_t	port;
	char		**groups;
}client_info_t;

/* ADDME message contents */
typedef struct	addme_info_
{
	avl_t		ipaddr_avl;
	uint16_t	port;
	uint8_t		no_of_groups;			
	avl_t		groups[0];
}addme_info_t;




