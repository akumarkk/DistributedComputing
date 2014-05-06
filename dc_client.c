#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


#define CLIENT_CONFIG_FILE 			"dc_client.conf"
#define PARSE_SERVER_ADDRESS_TAG 	"multi_cast_server"
#define SUCCESS 0
#define FAILURE -1


int
get_mcast_server(char	*server)
{
	FILE 	*fp = fopen(CLIENT_CONFIG_FILE, "r");
	int		len = 0;
	char	f_line[1024], *value = NULL, *line=NULL;

	if(fp == NULL || (server == NULL))
		return FAILURE;

	while( value=fgets(f_line, sizeof(f_line), fp) )
	{
		//printf("line : %s\n", f_line);
		line = f_line;

		if(value = strchr(line, '='))
		{
			*(value) = '\0';
			value++;
			/* Remove the leading white spaces */
			while( isspace(value[0]) )
				value++;

			/*Remove the trailing white spaces */
			len = strlen(value);
			while( isspace(value[len - 1]) )
				value[--len] = '\0';

			while( isspace(line[0]) )
				line++;
			len = strlen(line);
			while( isspace(line[len-1]) )
				line[--len] = '\0';

			//printf("key = #%s# \tValue = #%s#\n", line, value);

			if(strncmp(line, PARSE_SERVER_ADDRESS_TAG, strlen(PARSE_SERVER_ADDRESS_TAG)) == 0)
			{
				strcpy(server, value);
				return SUCCESS;
			}
		}
		memset(line, 0, sizeof(line));
	}

	return FAILURE;
}




int
main()
{
	char server[1024] = "";
	int ret = get_mcast_server(server);

	if(ret == SUCCESS)
		printf("Server is %s\n", server);
	else
		printf("Server not found\n");
}
