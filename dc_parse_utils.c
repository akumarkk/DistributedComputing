#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <ctype.h>

#include "dc_parse_utils.h"
#define MAX_AVAILABLE_GROUPS 10

keyword_t	keywords[] = 
{
	{ "sServerBindAddress", sBindAddress },
	{ "sServerBindPort", 	sBindport },
	{ "sServerJoinAddress", sJoinAddr },
	{ "sServerJoinPort", sJoinPort },
	{ "sServerAvailableGroups", sAvailableGroups },
	{ "cDestMultiCasteAddress", cDestMulticasteAddr },
	{ "cDestMultiCastePort", cDestMultiCasteport }
};

uint16_t
get_opcode_from_keyword(char	*key)
{
	uint16_t	n = sizeof(keywords)/sizeof(keyword_t);
	int			i = 0;
	for(i=0; i<n; i++)
	{
		if(strncasecmp(keywords[i].key, key,strlen(key)) == 0)
			return keywords[i].opcode;
	}
	//printf("n = %d i=%d\treturning BadOption \n", n, i);

	return BadOption;
}
	
int
get_value_of_option(FILE *fp, opcode_t	op, char *get_value)
{
    //FILE    *fp = fopen(CLIENT_CONFIG_FILE, "r");
    int     len = 0;
    char    f_line[1024], *value = NULL, *line=NULL;
	opcode_t	tmp_opc;

	//printf("Processing opcode : %d\n", op);

    if(fp == NULL || (get_value == NULL))
        return -1;

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
			tmp_opc = get_opcode_from_keyword( line );
			//printf("op = %d \tGot_op = %d\n", op, tmp_opc);
			if(tmp_opc == op)
			{
				//printf("DEBUG: Found the opcode\n");
				strcpy(get_value, value);
				get_value[strlen(value) ] = '\0';

				return 0;
			}
			else if(tmp_opc == BadOption)
			{
				//printf("Bad option ...\n");
				//return -1;
			}

        }
        memset(line, 0, sizeof(line));
    }

	return -1;
}

