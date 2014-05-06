#ifndef _CLIENT_INFO_H_ 
#define _CLIENT_INFO_H_

/* Structure to hold client information */
typedef	struct	client_info_
{
	int8_t		status;
	char		ip[16];
	uint16_t	port;
	char		**groups;
}client_info_t;

/* ADDME message contents */
typedef struct	addme_info_
{
	uint8_t		ip_len;
	char		ip16[16];
	uint16_t	port;
}addme_info_t;



