#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main (int argc, char **argv)
{
    
    char dir[100];

    sprintf(dir, "./%s", argv[1]);
    
    //result = (char *) malloc ((q + 1) * sizeof(char));
/*
    for (int i = 0; i < strlen(a); i++)
    {
        *(result + i) = *(a + i);
    }

     for (int i = 0; i < strlen(b); i++)
    {
        *(result + i + strlen(a)) = *(b + i);
    }
*/
   
    // some test message

    mkdir (dir, 0777);

    //free (result);
    return 0;
}