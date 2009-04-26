
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "asm/unistd.h"

// our system calls
_syscall3(int   , addtag , char*, path, char*, word  , size_t, len)
_syscall3(int   , rmtag  , char*, path, char*, word  , size_t, len)
_syscall3(size_t, gettags, char*, path, char*, buffer, size_t, sizecd)


int main (int argc, char *argv[]) {
   int status;
   char* file = NULL;
   char tag[4096];
   
   // First argument is filename
   if (argc >= 2) {
      file = argv[1];
   }
   
   // If there is only 1 argument, we list the tags
   if (argc == 2) {

      status = gettags(file, tag, sizeof(tag));

      if (status >= 0) {
         printf("File %s has tags of length %d:\n", file, status);
         char *tmp  = tag;
         while (tmp - tag < status) {
         	printf("\t%s\n",tmp);
         	tmp += strlen(tmp) + 1;
         }
      } else {
         perror("ERROR (TAG)");
      }

   // If there are 3 arguments, we're adding or removing tags
   } else if (argc == 4) {

      strcpy(tag, argv[3]);

      if (!strcmp("add", argv[2])) {
         
         // Adding

         status = addtag(file, tag, strlen(tag));

         if (status == 0) {
            printf("Added tag \"%s\" to file %s\n", tag, file);
         } else {
            perror("ERROR (TAG)"); 
         }
         
      } else if (!strcmp("rm", argv[2])) {
         
         // Removing

         status = rmtag(file, tag, strlen(tag));

         if (status == 0) {
            printf("Removed tag \"%s\" from file %s\n", tag, file);
         } else {
            perror("ERROR (TAG)");
         }

      } else {
         goto wrong_arg;
      }
      
   } else {
wrong_arg:
      printf("Usage: tag file (optional:add/rm {tag})\n");
      return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;

}

