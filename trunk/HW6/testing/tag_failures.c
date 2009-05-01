
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "asm/unistd.h"

// our system calls
_syscall3(int   , addtag , char*, path, char*, word  , size_t, len)
_syscall3(int   , rmtag  , char*, path, char*, word  , size_t, len)
_syscall3(size_t, gettags, char*, path, char*, buffer, size_t, sizecd)

#define MAX_TAGS 16

void print_tags(char *filename) {
	char tag[4096];
	int status = gettags(filename, tag, sizeof(tag));
	
	if (status >= 0) {
		printf("File %s has tags of length %d:\n", filename, status);
		char *tmp  = tag;
		while (tmp - tag < status) {
			printf("\t%s\n",tmp);
			tmp += strlen(tmp) + 1;
		}
	} else {
		perror("Error reading tags");
	}
}

int main (int argc, char *argv[]) {
   int status;
   char* file = NULL;
   char tag[4096];
   char errmsg[4096];
   
   // First argument is filename
   if (argc >= 2) {
      file = argv[1];
   } else {
      fprintf(stderr, "Please pass a filename for tagging experiments.\n");
      exit(1);
   }
   
   
   // ADD TAG TWICE 
   
   printf("--------------------------------------------------------------------------\n");
   strcpy(tag, "this is tag 1");
   size_t curlen = strlen(tag);
   status = addtag(file, tag, curlen);
   puts("[1] Adding the same tag to a file, twice");
   if (status) perror("[1] Unexpected failure in test");
   status = addtag(file, tag, curlen);
   if (status) perror("[1] Failure adding second tag");
   print_tags(file);
   if (rmtag(file,tag,curlen)) perror("[1] Failure cleaning up");
   
   // TOO MANY TAGS

   printf("--------------------------------------------------------------------------\n");
   printf("[2] Let's try to add %i tags to a single file\n", MAX_TAGS + 1);
   int len = 0;
   for (int i=1; i <= MAX_TAGS + 1; i++) {
      len = sprintf(tag, "%s%d", "tag",i);
      status = addtag(file, tag, len);
      if (status) {
         status = sprintf(errmsg, "[2] Failure adding %s", tag);
         perror(errmsg);
         break;
      }
   }

   // Clean-up
   for (int i=1; i < MAX_TAGS + 1; i++) {
      len = sprintf(tag, "%s%d", "tag",i);
      status = rmtag(file, tag, len);
   }
   
   // TAGS TOO LONG
   
   
   // REMOVE NON-EXISTENT TAG
   printf("--------------------------------------------------------------------------\n");
   printf("[4] Let's remove a non-existent tag\n");
   char badtag[] = "Go Canadiens! (it's an hockey team)";
   status = rmtag(file, badtag, strlen(badtag));
   if (status)
     perror("[4] Tried to remove non-existent tag");
   
   // WRONG FS
   printf("--------------------------------------------------------------------------\n");
   printf("[5] Let's tag this file: %s (it shouldn't work if the fs is non-ext2)\n", argv[0]);
   status =  addtag(argv[0], "tag", 3);
   if (status)
     perror("[5] Tried to tag a file in a non-ext2 file system");
   
   // NO SUCH FILE
   printf("--------------------------------------------------------------------------\n");
   printf("[6] Let's try tag a non-existing file\n");
   char badfile[] = "Freaking great file";
   status = addtag(badfile, badtag, strlen(badtag));
   if (status)
     perror("[6] Tried to tag a non-existing file");


   // BUFFER TOO SHORT (gettags)
   printf("--------------------------------------------------------------------------\n");
   printf("[7] Let's try size_t shorter than buffer\n");
   strcpy(tag, "this is a loooooooong tag");
   curlen = strlen(tag);
   status =  addtag(file, tag, curlen);
   if (status)
      perror("[7] Unexpected error, couldn't add tag for test");
   print_tags(file);
   char shortbuf[10];
   status = gettags(file, shortbuf, sizeof(shortbuf));
   if (status)
      perror("[7] Tried to call gettags with a short buffer");
   if (rmtag(file,tag,curlen)) perror("[7] Failure cleaning up");

   // size_t shorter than buffer
   printf("--------------------------------------------------------------------------\n");
   printf("[8] Let's try a short buffer in gettags\n");
   strcpy(tag, "longer than 3");
   status =  addtag(file, tag, 3);
   if (status)
      perror("[8] Tried size_t shorter than buffer");
   else
      if (rmtag(file,tag,3)) perror("[8] Failure cleaning up");

   // NEGATIVE size_t
   printf("--------------------------------------------------------------------------\n");
   printf("[9] Let's try using a negative size_t\n");
   status =  addtag(file, "negative", -8);
   if (status)
     perror("[9] Tried to tag a file using negative size_t");
   
/*   // If there is only 1 argument, we list the tags
   if (argc == 2) {


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
*/
}

