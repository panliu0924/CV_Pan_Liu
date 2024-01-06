#include "readckt.h"

#include "read.cpp"
#include "TPG.cpp"
#include "logicsim.cpp"
#include "DFS.cpp"
#include "PODEM.cpp"
#include "ATPG.cpp"
#include "PFS.cpp"

int main()
{
   int com;
   char cline[MAXLINE], wstr[MAXLINE];

   while(!Done) {
      printf("\nCommand>");
      fgets(cline, MAXLINE, stdin);
      if(sscanf(cline, "%s", wstr) != 1) continue;
      cp = wstr;
      while(*cp){
	*cp= Upcase(*cp);
	cp++;
      }
      cp = cline + strlen(wstr) + 1;
      com = READ;
      while(com < NUMFUNCS && strcmp(wstr, command[com].name)) com++;
      if(com < NUMFUNCS) {
         if(command[com].state <= Gstate) (*command[com].fptr)();
         else printf("Execution out of sequence!\n");
      }
      else system(cline);
   }
}
