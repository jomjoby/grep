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

void search_pattern(FILE *file, const char* pattern, const char* location)
{
	char line[1024];
	int line_number = 1;

	while(fgets(line, sizeof(line), file))
	{
		if(strstr(line, pattern))
		{
      if(location != NULL)
      {
        printf("File: %s Line: %d - %s", location, line_number, line);
		  }
      else
      {
        printf("Line: %d - %s", line_number, line);
      }
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
    line_number++;
	}
}

void search_pattern_inverted(FILE *file, const char* pattern, const char* location)
{
	char line[1024];
	int line_number = 1;

	while(fgets(line, sizeof(line), file))
	{
		if(!strstr(line, pattern))
		{
      if(location != NULL)
      {
        printf("File: %s Line: %d - %s", location, line_number, line);
		  }
      else
      {
        printf("Line: %d - %s", line_number, line);
      }
    }
		line_number++;
	}
}


void search_directory(const char* path, const char* pattern, char flag)
{
  DIR *dir = opendir(path);
  char full_path[1024];
  char org_path[1024];
  char file_full_path[1024];

  strcpy(full_path, path);

  if(dir == NULL)
  {
    perror("opendir");
    return;
  }

  struct dirent *entry;
  errno = 0;

  while((entry = readdir(dir)) != NULL)
  {
    if(strcmp(entry->d_name,".") == 0 || strcmp(entry->d_name, "..") == 0)
    {
      continue;
    }
    if(entry->d_type == DT_DIR)
    {
      // printf("full_path: %s\n", full_path);
      strcpy(org_path, full_path);
      strcat(full_path, entry->d_name);
      strcat(full_path, "/");
      // printf("full path: %s\n", full_path);
      search_directory(full_path, pattern, '\0');
      strcpy(full_path, org_path);
    }
    else if(entry->d_type == DT_REG)
    {
      // printf("full_path: %s\n", full_path);
      strcpy(file_full_path, full_path);
      strcat(file_full_path, entry->d_name);
      FILE *file = fopen(file_full_path , "r");
      if(flag == 'r')
      {
        search_pattern_regex(file, pattern);
      }
      else if(flag == 'c')
      {
        search_pattern_case_sens(file, pattern);
      }
      else if(flag == 'v')
      {
        search_pattern_inverted(file, pattern, file_full_path);
      }
      search_pattern(file, pattern, file_full_path);
      fclose(file);
    }
    // printf("item: %s\n", entry->d_name);
  }

  assert(errno == 0);

  closedir(dir);
}

int main(int argc, char* argv[])
{
	if(argc < 3 || argc > 4)
	{
		printf("Usage: %s <flag> <pattern> <filename>\n", argv[0]);
		return 1;
	}

  // If there is a flag
  char flag;
  if(argc == 4)
	{
  		FILE *file = fopen(argv[3], "r");
    // DIR *dir = readdir(argv[3]);

		if(!file)
		{
			printf("ERROR: couldn't open file");
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
      if(argv[1][0] == '-')
      {
        if(argv[1][1])
        {
          flag = argv[1][1];
        }
        else
        {
          printf("ERROR: Incorrect flag");
          return 1;
        }
        if(flag == 'c' || 'r' || 'v')
        {
          search_directory(argv[3], argv[2], flag);
        }
        else
        {
          printf("ERROR: Incorrect flag");
        }
      }
    }
    else if(S_ISREG(type.st_mode))
    {
      //printf("Is a file\n");

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
        else if(argv[1][1] == 'v')
        {
          search_pattern_inverted(file, argv[2], NULL);
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
      search_directory(argv[2], argv[1], '\0');
    }
    else if(S_ISREG(type.st_mode))
    {
      FILE *file = fopen(argv[2], "r");

      if(!file)
      {
        printf("ERROR: couldn't open file");
        return 1;
      }

      search_pattern(file, argv[1], NULL);

      fclose(file);
    }

  	return 0;
	}

}
