#include "utils.h"

bool is_builtin(char * prog_name) {
    return strcmp(prog_name, "cd") == 0 || strcmp(prog_name, "path") == 0 || strcmp(prog_name, "exit") == 0;
}



bool is_space_str(char * str) {
    while (isspace(*str++));
    str--;
    return *str == 0;
}

char *trimwhitespace(char *str) {
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
