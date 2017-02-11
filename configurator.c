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
		switch(errno)
		{
			case EACCES:
				printf("File '%s' doens't exist or reading access to the file is not allowed.\n", filename);
				break;
			default:
				printf("Unexpected error during reading config file(%d).\n",errno);
		}
		conf->_status = errno;
		return;
	}
	char buff[BUFFSIZE];
	while(fgets(buff, BUFFSIZE, file) != NULL)
	{
		if(buff[0] != '#' && strlen(buff)-1 >= 0)
		{
			buff[strlen(buff)-1] = '\0';
			tree_add(conf->_blacklist, buff);
		}
	}
	conf->_status = 0;
	print_tree(conf->_blacklist);
	fclose(file);
}