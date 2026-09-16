#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <assert.h>
#include <errno.h>

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

void search_pattern_case_sens(FILE *file, const char* pattern)
{
	char line[1024];
	char lc_line[1024];
	char lc_pattern[1024];
	int line_number = 1;

	strcpy(lc_pattern, pattern);

	for(int i = 0; pattern[i]; i++)
	{
		lc_pattern[i] = tolower(pattern[i]);
	}

	while(fgets(line, sizeof(line), file))
	{
		for(int i = 0; line[i]; i++)
		{
			lc_line[i] = tolower(line[i]);
		}
		if(strstr(lc_line, lc_pattern))
		{
			printf("%d: %s", line_number, line);
		}
	}

	line_number++;
}

void search_directory(const char* path)
{
  DIR *dir = opendir(path);

  if(dir == NULL)
  {
    perror("opendir");
    return;
  }

  struct dirent *entry;
  errno = 0;

  while((entry = readdir(dir)) != NULL)
  {
    printf("%s\n", entry->d_name);
  }

  assert(errno == 0);
}

int main(int argc, char* argv[])
{
	if(argc < 3 || argc > 4)
	{
		printf("Usage: %s <flag> <pattern> <filename>\n", argv[0]);
		return 1;
	}
  
  // If there is a flag
	if(argc == 4)
	{
  		FILE *file = fopen(argv[3], "r");
    // DIR *dir = readdir(argv[3]);

		if(!file)
		{
			printf("ERROR: couldn't open file");
			return 1;
		}

		if(argv[1][0] == '-')
		{
			if(argv[1][1] == 'c')
			{
				search_pattern_case_sens(file, argv[2]);
			}
			else if(argv[1][1] == 'r')
			{
				search_pattern_regex(file, argv[2]);
			}
			else
			{
				printf("ERROR: unknown flag");
        return 1;
			}
		}
		else 
		{
			printf("Usage: %s [flag] <pattern> <filename>\n", argv[0]);
			return 1;
		}

    struct stat type;

    if(stat(argv[3], &type) != 0)
    {
      printf("ERROR: of some kind");
      return 1;
    }
    if(S_ISDIR(type.st_mode))
    {
      printf("Is a directory\n");
      search_directory(argv[3]);
    }
    else if(S_ISREG(type.st_mode))
    {
      printf("Is a file\n");
    }
}
  // If there isn't a flag
	else
	{
    struct stat type;

    if(stat(argv[2], &type) != 0)
    {
      printf("ERROR: of some kind");
      return 1;
    }
    if(S_ISDIR(type.st_mode))
    {
      search_directory(argv[2]);
    }
    else if(S_ISREG(type.st_mode))
    {
      FILE *file = fopen(argv[2], "r");

      if(!file)
      {
        printf("ERROR: couldn't open file");
        return 1;
      }

      search_pattern(file, argv[1]);

      fclose(file);
    }

  	return 0;
	}

}
