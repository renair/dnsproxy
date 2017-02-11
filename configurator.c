#include<stdio.h>
#include<string.h>
#include<errno.h>

#include "configurator.h"

#define BUFFSIZE 126

void load_config(char* filename, struct config* conf)
{
	//struct config conf = *configuration;
	conf->_blacklist = create_node("n");
	FILE* file = fopen(filename, "r");
	if(file == NULL)
	{
		conf->_status = errno;
		return;
	}
	char buff[BUFFSIZE];
	int config_length = 2;
	while(fgets(buff, BUFFSIZE, file) != NULL)
	{
		if(buff[0] != '#' && strlen(buff)-1 > 0)
		{
			buff[strlen(buff)-1] = '\0';
			switch(config_length)
			{
				case 2: //master dns
					strncpy(conf->_masterdns, buff, MAX_ADDRLEN);
					--config_length;
					break;
				case 1: //if addr blacklisted
					if(strcmp(buff,"noaddr") == 0)
					{
						conf->_blacklistresponse[0] = '\0';
					}
					else
					{
						strncpy(conf->_blacklistresponse, buff, MAX_ADDRLEN);
					}
					--config_length;
					break;
				case 0:
					tree_add(conf->_blacklist, buff);
			}
		}
	}
	conf->_status = 0;
	//print_tree(conf->_blacklist);
	fclose(file);
}
