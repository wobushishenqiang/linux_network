/*************************************************************************
	> File Name: ififcontinue.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Mon 19 Oct 2020 09:05:45 AM PDT
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>

int main()
{
	do{
		if(1)
		{
			if(1)
			{
				printf("ifif\n");
				continue;
			}
		}
		break;
	}while(1);
	return 0;
}
