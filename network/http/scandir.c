//
// Created by xyx on 10/21/20.
//
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
int main()
{
    struct dirent **namelist;
    int file_num;
    file_num = scandir(".", &namelist, NULL, alphasort);
    if(file_num < 0)
    {
        perror("scandir error");
    }
    else
    {
        int index = 0;
        while(index < file_num)
        {
            printf("%s\n", namelist[index]->d_name);
            free(namelist[index]);
            index++;
        }
        free(namelist);
    }
    return 0;
}