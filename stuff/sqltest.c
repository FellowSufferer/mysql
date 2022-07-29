
//  sql handling stuff
#include <mysql/mysql.h>
#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <setjmp.h>

//  dir creation suff
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char **argv)
{  
  
    MYSQL *con = mysql_init(NULL);
    char current_dir[100];
    char *dir_name = "Gramoti";
    
    sprintf (current_dir, "./%s", dir_name);
    mkdir (current_dir, 0700);
    

    if (con == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "shambler", "1", 
            NULL, 0, NULL, 0) == NULL) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }  

    //    We need the participants' FIO to be in uppercase, and FI only - hence, the following SQL query
    if (mysql_query(con, "SELECT UPPER(SUBSTRING_INDEX(CONCAT(LTRIM(FIOp),' '), ' ', 2)), RTRIM(LTRIM(FIOt)), RTRIM(LTRIM(Place)), LTRIM(RTRIM(email)) FROM mydb.participants ORDER BY email asc")) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }
    
        MYSQL_RES *result = mysql_store_result (con);

        if ( result == NULL )
        {
            fprintf(stderr, "%s\n", mysql_error(con));
            mysql_close(con);
            exit(1);
        }

        //  int num_fields = mysql_num_fields (result);
        int filenum = 0;
        MYSQL_ROW row;
        
        int j = 0;

        while ((row = mysql_fetch_row(result)))
        {
            
            /*
            for (int i = 0; i < num_fields; i++)
            {
                printf ("%s | ", row[i] ? row[i] : "NULL");
            }
            printf ("\n");
            */
            
            sprintf(current_dir, "/home/shambler/Documents/progs/mysql/%s/%s", dir_name, row[3]);
            //  printf ("Current dir is %s \n", current_dir);
            
            struct stat st = {0};
            if (stat(current_dir, &st) == -1)
            {
                mkdir (current_dir, 0700);
                filenum = 0;
            }
            
            
            //  makePDF (row[0], row[1], row[2], row[3], current_dir, filenum);
            filenum++;

            j++; printf (" - file no %d\n", j);
        
        }
        mysql_free_result (result);
    
    mysql_close(con);
    
    printf("\n SUCCESS!! \n");
    exit(0);
}
