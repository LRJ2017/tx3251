#include <stdio.h>
#include "cgic.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>  
#include <unistd.h>  
#include <linux/reboot.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <time.h>

struct shared_use_st
{
	char upStatus;
};
int shmid;//共享内存标识符
struct shared_use_st *shared = NULL; //指向内存
char system_date_time[30] = {0};

const char* get_current_system_date_time()
{
	memset(system_date_time,0,30);
	time_t s;
	struct tm *p;
	s=time((time_t*)NULL);
	p=localtime(&s);
	sprintf(system_date_time,"%d-%d-%d %d:%d:%d",1900+p->tm_year,1+p->tm_mon, p->tm_mday,p->tm_hour,p->tm_min,p->tm_sec);
	return system_date_time;
}
void UpLoadUpdateFile(void)
{
	shared->upStatus = '1';
	cgiFilePtr file;
	FILE *fd;
	char name[1024];
	char path[50];
	char contentType[1024];
	int size;
	int i;
	unsigned int got;
	char *tmp = NULL;
	if (cgiFormFileName("updateFile", name, sizeof(name)) != cgiFormSuccess) {
		shared->upStatus = '2';
		return ;
	} 

#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "UpLoadUpdateFile name:%s \n", name);
	CGICDEBUGEND
#endif

	char nameBak[1024];
	strncpy(nameBak, name, 1024);
	int k = 0;
	for(i = 0; i < 1024; i++)
	{
		if(nameBak[i] == '\\')
		{
			k = 0;
			memset(name, 0, sizeof(name));
			continue;
		}
		name[k] = nameBak[i];
		k++;
	}

	cgiFormFileSize("updateFile", &size);
	cgiFormFileContentType("updateFile", contentType, sizeof(contentType));
	if (cgiFormFileOpen("updateFile", &file) != cgiFormSuccess) {
		shared->upStatus = '2';
		return ;
	}
	/*write file */
	tmp=(char *)malloc(sizeof(char)*size);
	strcpy(path , "/data/update/");
	strcat(path, name);  

	if(strcmp(name, "tandaUpdate.tar") != 0)
	{
		shared->upStatus = '2';
		return;
	}

#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "UpLoadUpdateFile path:%s \n", path);
	CGICDEBUGEND
#endif

	fd=fopen(path ,"w+");
	if(fd==NULL)
	{
		shared->upStatus = '2';
		return ;
	}
	while (cgiFormFileRead(file, tmp, size, &got) ==
		cgiFormSuccess)
	{
		fwrite(tmp, size, sizeof(char), fd);
	}
	cgiFormFileClose(file);
	free(tmp);
	fclose(fd);

	shared->upStatus = '3';
	return ;
}

void SubmitHandle(void)
{
	system("chmod 777 /data");
	system("chmod 777 /data/update");
	UpLoadUpdateFile();
}

void GetUploadState(void)
{
	FILE *fd;
	char c=5;
	if((fd = fopen("/data/uplog","r")) == NULL)
	{
		fprintf(cgiOut,"Can get the lognews");
		return;
	}
	c=fgetc(fd);
	fclose(fd);

	printf("Upload=%c", shared->upStatus);
	printf("\n");
	printf("Update=%c", c);
}

int cgiMain() 
{
	int ret;
	char *UserInput = NULL; 		// index the user input data.
   	char *request_method = NULL; 	//index the html transfer type.
	int data_len = 0;		// index the input data's length.
	int lock=0;			
	printf("Content-type:text/html;charset=utf-8\n\n");
	if( ! getenv("REQUEST_METHOD") )
	{
		return -1;
	}

	//创建共享内存
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
	//将共享内存连接到当前进程的地址空间
	shared = (struct shared_use_st *)shmat(shmid, 0, 0);

	request_method = getenv("REQUEST_METHOD"); // trans-type.
	 // trans-type : GET 
	if( ! strcmp( request_method, "GET" ) )
	{
		if( getenv( "QUERY_STRING" ) )
		{
			UserInput = getenv( "QUERY_STRING" );
		}
		data_len = strlen( UserInput ); 

		if( NULL == UserInput || 0 == data_len )
		{
			printf( "There's no input data !\n" );
            return -1;
		}
		UserInput[data_len] = '\0';
		if(!strncmp(UserInput,"getstatus" , 9))
		{
			GetUploadState();
		}
		if(!strncmp(UserInput,"uploadcmd" , 9))
		{
			shared->upStatus = '0';

			system("echo 7 > /data/uplog");

			printf("Upload=%c", shared->upStatus);
		}
		return 0;
	}

	if( ! strcmp( request_method, "POST" ) )
	{
		system("echo 5 > /data/uplog");
		if ((cgiFormSubmitClicked("updateFile") == cgiFormSuccess))
		{
			SubmitHandle();
		}
		else
		{
#ifdef CGICDEBUG
			CGICDEBUGSTART
			fprintf(dout, "update submit cgiFormFailed \n");
			CGICDEBUGEND
#endif
			shared->upStatus = '4';  //submit err
		}
	}

	return 0;
}





  

