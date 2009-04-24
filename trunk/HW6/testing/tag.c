
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
   
   char* file = NULL;
   char tag[4096];
   
   // First argument is filename
   if (argc >= 2) {
      file = argv[1];
   }
   
   // If there is only 1 argument, we list the tags
   if (argc == 2) {

      errno = gettags(file, tag, sizeof(tag));

      if (!errno) {
         printf("File %s has tags %s\n", file, tag);
      } else {
         perror("ERROR (TAG): ");
      }

   // If there are 3 arguments, we're adding or removing tags
   } else if (argc == 4) {

      strcpy(tag, argv[3]);

      if (!strcmp("add", argv[2])) {
         
         // Adding

         errno = addtag(file, tag, strlen(tag));

         if (!errno) {
            printf("Added tag \"%s\" to file %s\n", tag, file);
         } else {
            perror("ERROR (TAG): "); 
         }
         
      } else if (!strcmp("rm", argv[2])) {
         
         // Removing

         errno = addtag(file, tag, strlen(tag));

         if (!errno) {
            printf("Removed tag \"%s\" from file %s\n", tag, file);
         } else {
            perror("ERROR (TAG): ");
         }

      } else {
         goto wrong_arg;
      }
      
   } else {
wrong_arg:
      errno = EINVAL;
      perror("Usage: tag file (optional:add/rm {tag})");
   }

   return errno;

}

