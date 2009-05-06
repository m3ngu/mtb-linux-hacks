
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
   
   puts("--------------------------------------------------------------------------");
   strcpy(tag, "this is tag 1");
   size_t curlen = strlen(tag);
   status = addtag(file, tag, curlen);
   puts("[1] Adding the same tag to a file, twice (should appear only once)");
   if (status) perror("[1] Unexpected failure in test");
   status = addtag(file, tag, curlen);
   if (status) perror("[1] Failure adding second tag");
   print_tags(file);
   if (rmtag(file,tag,curlen)) perror("[1] Failure cleaning up");
   
   // TOO MANY TAGS
   puts("--------------------------------------------------------------------------");
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
   
   puts("--------------------------------------------------------------------------");
   // TAGS TOO LONG
   
   puts("[3a] trying to add a tag that does not fit in the tag block");
   for (int i = 0; i < sizeof(tag); i++) tag[i] = 'q';
   status = addtag(file, tag, 4096);
   if (status) perror("[3a] Failed as expected");
   else fputs("[3a] Unexpected success (oh dear)\n", stderr);
   
   puts("[3b] trying to add multiple tags which combined do not fit");
   for (int i = 0; i < 400; i++) tag[i] = 'a';
   tag[400] = '\0';
   int i;
   for (i = 0; i < 12; i++) {
   	  tag[0] = i + 48;
      status = addtag(file,tag,400);
      if (status) {
      	 fprintf(stderr, 
      	    "[3b] Finally got an error on %d: %s\n", i, strerror(errno));
      	    break;
      } else {
      	 fprintf(stderr, "[3b] Succeeded in adding tag %d\n", i);
      }
   }
   // cleanup...
   for ( --i ; i >= 0; i--) {
   	  tag[0] = i;
   	  status = rmtag(file,tag,400);
   	  if (status) perror("[3b] Unexpected error cleaning up");
   }
   
   // REMOVE NON-EXISTENT TAG
   puts("--------------------------------------------------------------------------");
   puts("[4] Let's remove a non-existent tag");
   char badtag[] = "Go Canadiens! (it's an hockey team)";
   status = rmtag(file, badtag, strlen(badtag));
   if (status)
     perror("[4] Tried to remove non-existent tag");
   
   // WRONG FS
   puts("--------------------------------------------------------------------------");
   printf("[5] Let's tag this executable: %s (this shouldn't work given the fs is non-ext2)\n", argv[0]);
   status =  addtag(argv[0], "tag", 3);
   if (status)
     perror("[5] Tried to tag a file in a non-ext2 file system");
   
   // NO SUCH FILE
   puts("--------------------------------------------------------------------------");
   puts("[6] Let's try tag a non-existing file");
   char badfile[] = "Freaking great file";
   status = addtag(badfile, badtag, strlen(badtag));
   if (status)
     perror("[6] Tried to tag a non-existing file");


   // BUFFER TOO SHORT (gettags)
   puts("--------------------------------------------------------------------------");
   puts("[7] Let's try with a short buffer (len: 10)");
   strcpy(tag, "this is a loooooooong tag (at least, longer than 10)");
   curlen = strlen(tag);
   status =  addtag(file, tag, curlen);
   if (status)
     perror("[7] Unexpected error, couldn't add tag for test");
   print_tags(file);
   char shortbuf[10] = {'\1','\1','\1','\1','\1','\1','\1','\1','\1','\1'};
   status = gettags(file, shortbuf, sizeof(shortbuf));
   if (status > 10) {
      printf("[7] gettags wants buffer of length %d \n", status);
      if ( 1 != shortbuf[0] )
        printf("[7] gettags wrote data to short buffer\n");
      else 
        puts("[7] gettags left buffer untouched");
	} else {
	   puts("[7] foiled by not having enough tags to fill the short buffer!");
	}
   if (rmtag(file,tag,curlen)) perror("[7] Failure cleaning up");

   // size_t shorter than buffer
   puts("--------------------------------------------------------------------------");
   puts("[8] Let's try size_t shorter than buffer (size_t=12)");
   strcpy(tag, "tag longer than 12");
   curlen = strlen(tag);
   printf("[8] Trying to add tag \"%s\" but using size_t=12 instead of %i\n", tag, curlen);
   curlen = 12; // instead of strlen(tag);
   status = addtag(file, tag, curlen);

   if (status)
     perror("[8] Tried size_t shorter than buffer");
   else {
      puts("[8] gettags should show tag truncated to size_t (=12)");
   	 print_tags(file);
   }
   if (rmtag(file,tag,curlen)) perror("[8] Failure cleaning up");

   // NEGATIVE size_t
   puts("--------------------------------------------------------------------------");
   puts("[9] Let's try using a very large size_t");
   status =  addtag(file, "negative", -8); // hey, it's unsigned, so...
   if (status)
     perror("[9] Tried to tag a file using huge size_t");

   puts("--------------------------------------------------------------------------");
   puts("[10] trying to access kernel memory");
   
   if (addtag(file, (char *) 42,100) ) 
     perror("[10a] addtag won't read kernel memory");
   else 
   	 fputs("[10a] no error returned when addtag passed bad pointer\n", stderr);
   
   if (gettags(file, (char *) 42,100) ) 
   	 perror("[10b] gettag won't write kernel memory");
   else
     fputs("[10b] no error returned when gettags passed bad pointer\n", stderr);
   
puts("--------------------------------------------------------------------------");
   puts("[11] eeeevil tag tests");
   
   puts("[11a] trying to add evil tag to file");
   char evil[12] = { 'e','e','e','e','v','i','l','\0','\0','\0','\0','\0'};
   status = addtag(file, evil, sizeof(evil));
   if (status) {
   	perror("[11a] unable to add tag");
   }
   else {
      puts("[11a] apparently successful");
      print_tags(file);
   }
   puts("[11b] trying to remove evil tag to file");
   status = rmtag(file,evil,sizeof(evil));
   if (status) perror("[11b] Could not remove evil tag");
   else puts("[11b] apparently successful");
}

