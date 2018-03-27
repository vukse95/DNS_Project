#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int getCfgString(char *section, char *keyname, char *defaultres, char *data, int size, char *cfgfilename){
	char strSsection[256];
	char *offset;
	char line[300];
  int dataOffset;
	FILE *iniFile = fopen(cfgfilename, "r+");
	
	//section search
	strcpy(data, defaultres);
	sprintf(strSsection, "[%s]", section);
	while(1){
    if(NULL==fgets(line, 300, iniFile)) {
      fclose(iniFile);
      return (int)strlen(data);
    }
    int tmp = strlen(line);
    while( (line[strlen(line)-1] == '\r') || (line[strlen(line)-1] == '\n')){
      line[strlen(line)-1] = 0x00;
    }
		if(strcmp(line, strSsection)==0) break;
	}
	
	//Keyname search
	while(1){
    if(NULL==fgets(line, 300, iniFile)) {
      fclose(iniFile);
      return (int)strlen(data);
    }
    if(line[0]=='[') {
      fclose(iniFile);
      return (int)strlen(data);//check if it is end of section
    }
    while( (line[strlen(line)-1] == '\r') || (line[strlen(line)-1] == '\n')){
      line[strlen(line)-1] = 0x00;
    }
		offset = strchr(line, '=');
    if(offset == NULL){
      continue;
    }
		*offset = 0; 
    while( (line[strlen(line)-1] == ' ') ){
      line[strlen(line)-1] = 0x00;
      if(strlen(line) == 0){
        break;
      }
    }
    offset++;
    while( *offset  == ' ' ){
      offset++;
    }
		if(strcmp(line, keyname)==0){
      if(strlen(offset+1) < (size-1)){
			  strcpy(data, offset);
			  return (int)strlen(data);
      }
		}
	}//end while
	return 0;
}

int getCfgInt(char *section, char *keyname, int defaultres, char *cfgfilename){
  char data[30];
  char df[30];
  sprintf(df, "%d", defaultres);

  if(getCfgString(section, keyname, df, data, sizeof(data), cfgfilename) != 0){
    return atoi(data);
  }
  return defaultres;
}

