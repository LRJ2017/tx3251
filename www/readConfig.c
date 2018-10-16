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
#include <time.h>

#define SOFILENUM 	10
#define SOFILELEN	20

char Ip[20]={0};
char Port[20]={0};
char HardVersion[20]={0};
char SoftVersion[20]={0};
char UpdateTime[20]={0};
char IMEI[20]={0};
char CPUID[30]={0};
char ControllerID[30]={0};
char ControllerType[30]={0};
char MAC[30]={0};
char Baudrate232[20]={0};
char Baudrate485[20]={0};
char Lib232[30]={0};
char Lib485[30]={0};
char PortSelect[30]={0};
char TotleSoFile[50][40];
char TotleSoNum=0;
const char *BaudrateTable[] = 
{
	"2400",
	"4800",
	"9600",
	"14400",
	"19200",
	"38400",
	"57600",
	"115200",
	"230400",
	"460800",
	"921600"
};

const char *ControllerTypeTable[]=
{
	"1",
	"11",
	"22",
	"31",
	"34"
};

char *PortSelectTable[]=
{
	"1",
	"2"
};

static void CopyIndexStr(char *dest , char *src)
{
	int i=0;
	for(i=0 ; i<30 ; i++)
	{
		if('='!=src[i])
		{
			dest[i]=src[i];
		}
		else
		{
			break;
		}
	}
	dest[i]='\0';
}
/*
Find the '=' pos and get the config dat
*/
static int DatPos(char *dat , int index )
{
	int i=0;
	if(index==0)//find the config file
	{
		while((*dat++)!='=')
		{
			i++;
		}
		return i;
	}
	else //find the *.so file
	{
		while(*(dat+1)&&*dat)
		{
			if(*dat=='s'&&*(dat+1)=='o')
			{
				return 1;
			}
			dat++;
		}
		return 0;
	}
}

void GetMAC(void)
{
	FILE   *stream;  
	stream = popen( "cat /etc/cpuid", "r" ); 
	fread( CPUID, sizeof(char), sizeof(CPUID), stream); 
	pclose( stream );  
	CPUID[sizeof(CPUID)] = '\0';
	stream = popen( "cat /etc/mac", "r" ); 
	fread( MAC, sizeof(char), sizeof(MAC), stream); 
	pclose( stream );  
	MAC[sizeof(MAC)] = '\0';
}

void GeUpdateTime(void)
{
	FILE   *stream;
	stream = popen( "cat /data/updateTime", "r" );
	fread(UpdateTime, sizeof(char), sizeof(UpdateTime), stream);
	pclose(stream);
	UpdateTime[sizeof(UpdateTime)] = '\0';
}

/*When open html we should read configfile to fix it*/
void ReadTandaConf(void)
{
	FILE *fd;
	char StrLine[200];  
	char ptr[30];
	int i=0;
	if((fd = fopen("/data/Tanda.conf","r")) == NULL) 
	{ 
		printf("cant't open file\n");
		//fclose(fd);
		return; 
	} 
	
	while (!feof(fd)) 
	{     
		fgets(StrLine,200,fd);  
		if(StrLine[0]=='#'||StrLine[0]==' ')
		{
			continue;
		}		
		else
		{
			i=DatPos(StrLine , 0);
			memset(ptr , 0 , sizeof(ptr));
			CopyIndexStr(ptr,StrLine);
			if(!strcmp(ptr , "HardVersion"))
			{
				strncpy(HardVersion,&StrLine[i+1],sizeof(HardVersion));
				HardVersion[sizeof(HardVersion)-1]='\0';
			}			
			else if(!strcmp(ptr , "SoftVersion"))
			{
				strncpy(SoftVersion,&StrLine[i+1],sizeof(SoftVersion));
				SoftVersion[sizeof(SoftVersion)-1]='\0';
			}
			else if(!strcmp(ptr , "ControllerType"))
			{
				strncpy(ControllerType,&StrLine[i+1],sizeof(ControllerType));
				ControllerType[sizeof(ControllerType)-1]='\0';
			}
			else if(!strcmp(ptr , "ControllerID"))
			{
				strncpy(ControllerID,&StrLine[i+1],sizeof(ControllerID));
				ControllerID[sizeof(ControllerID)-1]='\0';
			}
			else if(!strcmp(ptr , "IMEI"))
			{
				strncpy(IMEI,&StrLine[i+1],sizeof(IMEI));
				IMEI[sizeof(IMEI)-1]='\0';
			}
			else if(!strcmp(ptr , "ip"))
			{
				strncpy(Ip,&StrLine[i+1],sizeof(Ip));
				Ip[sizeof(Ip)-1]='\0';
			}
			else if(!strcmp(ptr , "port"))
			{
				strncpy(Port,&StrLine[i+1],sizeof(Port));
				Port[sizeof(Port)-1]='\0';
			}
			else if(!strcmp(ptr , "portenable"))
			{
				strncpy(PortSelect,&StrLine[i+1],sizeof(PortSelect));
				PortSelect[sizeof(PortSelect)-1]='\0';
			}
			else if(!strcmp(ptr , "baudrate232"))
			{
				strncpy(Baudrate232,&StrLine[i+1],sizeof(Baudrate232));
				Baudrate232[sizeof(Baudrate232)-1]='\0';
			}			
			else if(!strcmp(ptr , "baudrate485"))
			{
				strncpy(Baudrate485,&StrLine[i+1],sizeof(Baudrate485));
				Baudrate485[sizeof(Baudrate485)-1]='\0';
			}
			else if(!strcmp(ptr , "protocolName232"))
			{
				strncpy(Lib232,&StrLine[i+3],sizeof(Lib232));
				Lib232[sizeof(Lib232)-1]='\0';
			}
			else if(!strcmp(ptr , "protocolName485"))
			{
				strncpy(Lib485,&StrLine[i+3],sizeof(Lib485));
				Lib485[sizeof(Lib485)-1]='\0';
			}
			else
				;
		}
		
	} 
	
	fclose(fd);                    
	
}

void RequistConfig(void)
{
	FILE *fd;
	char StrLine[1024];
	int i=0;	
	GetMAC();
	ReadTandaConf();
	fprintf(cgiOut, "SoftVersion=%s/",SoftVersion);
	fprintf(cgiOut, "HardVersion=%s/",HardVersion);
	fprintf(cgiOut, "MAC=%s/",MAC);
	fprintf(cgiOut, "controllertype=%s/",ControllerType);
	fprintf(cgiOut, "controllerid=%s/",ControllerID);
	fprintf(cgiOut, "CPUID=%s/", CPUID);
	fprintf(cgiOut, "IMEI=%s/",IMEI);
	fprintf(cgiOut, "Ip=%s/",Ip);
	fprintf(cgiOut, "Port=%s/",Port);
	if(PortSelect[0]=='1')
	{		
		fprintf(cgiOut, "Portselect=1\n/");
	}
	else if(PortSelect[0]=='2')
	{
		fprintf(cgiOut, "Portselect=2\n/");
	}
	else if(PortSelect[0]=='3')
	{
		fprintf(cgiOut, "Portselect=3\n/");
	}
	fprintf(cgiOut, "bau232=%s/",Baudrate232);
	fprintf(cgiOut, "Lib232=%s/",Lib232);
	fprintf(cgiOut, "bau485=%s/",Baudrate485);
	fprintf(cgiOut, "Lib485=%s/",Lib485);
	for(i=0 ; i<TotleSoNum ; i++)
	{
		if(strcmp(Lib232 , TotleSoFile[i]))
		{			
			fprintf(cgiOut, "setnet232=%s/",TotleSoFile[i]);
		}
		if(strcmp(Lib485 , TotleSoFile[i]))
		{			
			fprintf(cgiOut, "setnet485=%s/",TotleSoFile[i]);
		}
	}
	GeUpdateTime();
	time_t now;
	struct tm *tm_now;
	char    curDate[20];
	time(&now);
	tm_now = localtime(&now);
	strftime(curDate, 20, "%Y-%m-%d %H:%M", tm_now);
	fprintf(cgiOut, "UpdateTime=%s 当前时间:%s/",UpdateTime, curDate);

	fprintf(cgiOut, "readend");
}

char GetNetState(void)
{
	FILE *fd;
	char c=5;
	if((fd = fopen("/data/NetStatus","r")) == NULL)
	{
		//fprintf(cgiOut,"Can get the lognews");
		return 0;
	}
	c=fgetc(fd);
	fclose(fd);
	return c;
}

void GetSysState(void)
{
	FILE   *stream;  
	char   buf[50]; 
	char c;
	memset( buf,0,sizeof(buf));
	stream = popen( "ps | grep SteamSensor | grep -v grep | awk '{print $1}'", "r" );
	if(stream == NULL)
	{
		return;
	}
	//chmod((char*)stream ,777);
	fread(buf,sizeof(char),sizeof(buf),stream);
	buf[strlen(buf)]='\0';
	if(strlen(buf)>2)
	{
		printf("Server=Run");
	}
	else
	{
		printf("Server=Stop");
	}

	printf("\n");
	c = GetNetState();
	if(c == '1')
	{
		printf("Net=Run");
	}
	else
	{
		printf("Net=Stop");
	}

	pclose( stream );  
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
		if(!strncmp(UserInput,"getconf" , 7))
		{
			RequistConfig();
		}
		else if(!strncmp(UserInput,"getstatus" , 9))
		{
			GetSysState();
		}
	}

	return 0;
}





  

