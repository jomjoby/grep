#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

void search_pattern(FILE *file, const char* pattern)
{
	char line[1024];
	int line_number = 1;

	while(fgets(line, sizeof(line), file))
	{
		if(strstr(line, pattern))
		{
			printf("%d: %s", line_number, line);
		}
		line_number++;
	}
}

void search_pattern_regex(FILE *file, const char* pattern)
{
	char line[1024];
	int line_number = 1;
	regex_t regex;

	if(regcomp(&regex, pattern, REG_EXTENDED))
	{
		printf("ERROR: compiling regex: %s\n", pattern);
		return;
	}

	while(fgets(line, sizeof(line), file))
	{
		if(!regexec(&regex, line, 0, NULL, 0))
		{
			printf("%d: %s", line_number, line);
		}
		line_number++;
	}
	regfree(&regex);
}

void search_pattern_case_sens()
{

}

int main(int argc, char* argv[])
{
	if(argc < 3 || argc > 4)
	{
		printf("Usage: %s [flag] <pattern> <filename>\n", argv[0]);
		return 1;
	}
	
	if(argc == 3)
	{
		FILE *file = fopen(argv[2], "r");

		if(!file)
		{
			printf("ERROR: Couldn't open file\n");
			return 1;
		}

		printf("Simple grep (non-regex):\n");
		
		search_pattern(file, argv[1]);

		rewind(file);
		printf("\nGrep with regex:\n");
		search_pattern_regex(file, argv[1]);

		fclose(file);
		return 0;
	}
	else
	{
		FILE *file = fopen(argv[3], "r");
		
		if(!file)
		{
			printf("ERROR: Couldn't open file");
			return 1;
		}

		if(argv[1][0] != '-')
		{
			printf("ERROR: Incorrect flag use '-'\n");
			return 1;

		}

		if(argv[1][1] == 'c')
		{
			search_pattern_case_sens();
		}
		else
		{
			printf("ERROR: Unknown flag\n");
			return 1;
		}
	}
}
