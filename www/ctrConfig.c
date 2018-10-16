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

char Ip[20]={0};
char Port[20]={0};
char HardVersion[20]={0};
char SoftVersion[20]={0};
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
char TotleSoFile[1000][200]={0};
char TotleSoFileName[1000][40]={0};
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
	"10",
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

static int IndexName(char *dat )
{
	int i = 0;
	int index = 0;
	while(*(dat))
	{
		if(*dat == '@')
		{
			index = i;
		}
		dat++;
		i++;
	}
	return index;
}

void GetFileOfSo(void)
{
	FILE   *stream;
	FILE    *wstream;
	char   buf[1024];
	memset( buf,0,sizeof(buf));
	stream = popen( " ls /data/so/*.so", "r" );
	wstream = fopen( "/data/Numberso", "w+");
	system("chmod 777 /data/Numberso");
	if(stream)
	{
		fread( buf, sizeof(char), sizeof(buf),  stream);
		pclose( stream );
	}
	if(wstream)
	{
		fwrite( buf, 1, sizeof(buf), wstream );
		fclose( wstream );
	}
}

void GetSoFileList(void)
{
	FILE *fd;
	TotleSoNum=0;
	char StrLine[1024];
	char SoName[40];
	int i;
	if((fd = fopen("/data/SoList","r")) == NULL)
	{
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/SoList failed!------GetSoFileList\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return;
	}
	while (!feof(fd))
	{
		fgets(StrLine,1024,fd);

		i=DatPos(StrLine,1);
		if(i==1) //find a so file
		{
			strcpy(TotleSoFile[TotleSoNum], StrLine);
			int index = IndexName(StrLine);
			memset(SoName, 0, sizeof(SoName));
			strcpy(SoName, &StrLine[index + 1]);
			for(i = 0; i< 50; i++)
			{
				if(SoName[i] == '\r' || SoName[i] == '\n')
				{
					SoName[i] = '\0';
				}
			}
			strcpy(TotleSoFileName[TotleSoNum], SoName);

			TotleSoNum++;
		}
	}
	fclose(fd);

	GetFileOfSo();

	if((fd = fopen("/data/Numberso","r")) == NULL)
	{
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/Numberso failed!------GetSoFileList\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return;
	}
	while (!feof(fd))
	{
		fgets(StrLine,1024,fd);
		i=DatPos(StrLine,1);
		if(i==1) //find a so file
		{
			memset(SoName, 0, sizeof(SoName));
			strcpy(SoName, &StrLine[6]);

			for(i = 0; i< 50; i++)
			{
				if(SoName[i] == '\r' || SoName[i] == '\n')
				{
					SoName[i] = '\0';
				}
			}
			for(i = 0; i< 57; i++)
			{
				SoName[i] = SoName[i + 3];
			}

			int find = 0;
			for(i = 0; i < 1000; i++)
			{
				if(strcmp(SoName, TotleSoFileName[i]) == 0)
				{
					find = 1;
					break;
				}
			}
			if(find == 1)
			{
				continue;
			}

			memset(TotleSoFile[TotleSoNum], 0, 200);
			strcat(TotleSoFile[TotleSoNum], "@");
			strcat(TotleSoFile[TotleSoNum], "@");
			strcat(TotleSoFile[TotleSoNum], SoName);
			strcat(TotleSoFile[TotleSoNum], "@");
			strcat(TotleSoFile[TotleSoNum], SoName);
			strcat(TotleSoFile[TotleSoNum], "\n");
			strcpy(TotleSoFileName[TotleSoNum], SoName);
			TotleSoNum++;
		}
		else
		{
			break;
		}
	}
	fclose(fd);
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
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/Tanda.conf failed!------ReadTandaConf\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
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

int WriteConfDat(void)
{
	FILE *fd;
	int SoChoice232;
	int bauChoice232;
	int SoChoice485;
	int bauChoice485;
	int controllertypechoice;
	int portselect[2] = {0};
	int i;
	int result , invalid;
	ReadTandaConf();
	cgiFormSelectSingleNormal("controllertype", ControllerTypeTable, 6, &controllertypechoice, 0);
	cgiFormStringNoNewlines("controllerid", ControllerID, sizeof(ControllerID));
	result = cgiFormSelectMultiple("portcheck", PortSelectTable, 2, portselect, &invalid);
	if (cgiFormNotFound == result) 
	{
		return -1;
	}
	else 
	{	
		result=0;
		for (i = 0; i < 2; i++)
		{
			if (portselect[i]) 
			{
				result = (i+1);
				break;
			}
		}
	}
	
	GetSoFileList();
	if(1 == result)
	{
		PortSelect[0]='1';

	}
	else if(2 == result)
	{
		PortSelect[0]='2';;
	}
	else
	{
		PortSelect[0]='1';
	}

	cgiFormSelectSingleNormal("baudRate", BaudrateTable, 11, &bauChoice232, 0);
	cgiFormSelectSingle("controllermode", TotleSoFileName, TotleSoNum, &SoChoice232, 0);

#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "SoChoice232:%d\n", SoChoice232);
		CGICDEBUGEND
#endif

    system("chmod 777 /data/Tanda.conf");
	fd=fopen("/data/Tanda.conf" ,"w");
	if(fd)
	{			
		fputs("###################################\n",fd);
		fputs("#    Tanda Adapter Config File    #\n",fd);
		fputs("###################################\n\n",fd);
		
		fputs("#HardVersion\nHardVersion=",fd);
		fputs(HardVersion, fd);
		fputs("\n\n",fd);

		
		fputs("#SoftVersion\nSoftVersion=",fd);
		fputs(SoftVersion, fd);
		fputs("\n\n",fd);
		
		fputs("#ControllerType\nControllerType=",fd);
		fputs(ControllerTypeTable[controllertypechoice], fd);
		fputs("\n\n",fd);

		fputs("#ControllerID\nControllerID=",fd);
		fputs(ControllerID, fd);
		fputs("\n\n",fd);
		
		fputs("#IMEI\nIMEI=",fd);
		fputs(IMEI, fd);
		fputs("\n\n",fd);

		fputs("#Service IP\nip=",fd);
		fputs(Ip , fd);
		fputs("\n\n",fd);

		fputs("#Service Port\nport=",fd);
		fputs(Port , fd);
		fputs("\n\n",fd);

		fputs("#1 is 232 , 2 is 485 , 3 is all\nportenable=",fd);
		fputc(PortSelect[0], fd);
		fputs("\n\n",fd);

		fputs("#Baudrate\nbaudrate232=",fd);
		fputs(BaudrateTable[bauChoice232], fd);
		fputs("\n\n",fd);
		fputs("#protocolName232\nprotocolName232=./",fd);
		fputs(TotleSoFileName[SoChoice232] , fd);
		fputs("\n\n",fd);
		fputs("#Baudrate485\nbaudrate485=",fd);
		fputs(Baudrate485, fd);
		fputs("\n\n",fd);
		fputs("#protocolName\nprotocolName485=./",fd);
		fputs(Lib485, fd);
		fputs("\n\n",fd);

		fclose(fd);

	}
	else
	{
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/Tanda.conf failed!------WriteConfDat\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return -3;
	}
	return 0;

}

int WriteServerConfDat(void)
{
	FILE *fd;
	int i;
	int result , invalid;
	ReadTandaConf();
	cgiFormStringNoNewlines("ip", Ip, sizeof(Ip));
	cgiFormStringNoNewlines("port", Port, sizeof(Port));

    system("chmod 777 /data/Tanda.conf");
	fd=fopen("/data/Tanda.conf" ,"w");
	if(fd)
	{
		fputs("###################################\n",fd);
		fputs("#    Tanda Adapter Config File    #\n",fd);
		fputs("###################################\n\n",fd);

		fputs("#HardVersion\nHardVersion=",fd);
		fputs(HardVersion, fd);
		fputs("\n\n",fd);


		fputs("#SoftVersion\nSoftVersion=",fd);
		fputs(SoftVersion, fd);
		fputs("\n\n",fd);

		fputs("#ControllerType\nControllerType=",fd);
		fputs(ControllerType, fd);
		fputs("\n\n",fd);

		fputs("#ControllerID\nControllerID=",fd);
		fputs(ControllerID, fd);
		fputs("\n\n",fd);

		fputs("#IMEI\nIMEI=",fd);
		fputs(IMEI, fd);
		fputs("\n\n",fd);

		fputs("#Service IP\nip=",fd);
		fputs(Ip , fd);
		fputs("\n\n",fd);

		fputs("#Service Port\nport=",fd);
		fputs(Port , fd);
		fputs("\n\n",fd);

		fputs("#1 is 232 , 2 is 485 , 3 is all\nportenable=",fd);
		fputc(PortSelect[0], fd);
		fputs("\n\n",fd);

		fputs("#Baudrate\nbaudrate232=",fd);
		fputs(Baudrate232, fd);
		fputs("\n\n",fd);
		fputs("#protocolName232\nprotocolName232=./",fd);
		fputs(Lib232 , fd);
		fputs("\n\n",fd);
		fputs("#Baudrate485\nbaudrate485=",fd);
		fputs(Baudrate485, fd);
		fputs("\n\n",fd);
		fputs("#protocolName\nprotocolName485=./",fd);
		fputs(Lib485, fd);
		fputs("\n\n",fd);

		fclose(fd);

	}
	else
	{
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/Tanda.conf failed!------WriteServerConfDat\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return -3;
	}
	return 0;

}

void SubmitHandle(void)
{
	system("chmod 777 /data");
	system("chmod 777 /data/update");
}

void RequistConfig(void)
{
	FILE *fd;
	char StrLine[1024];
	int i=0;	
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
	GetSoFileList();
	for(i=0 ; i<TotleSoNum ; i++)
	{
		fprintf(cgiOut, "setnet232=%s/",TotleSoFile[i]);
	}
	fprintf(cgiOut, "readend");

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

		return 0;
	}

	if ((cgiFormSubmitClicked("baudRate") == cgiFormSuccess))
	{
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "ctrConfig submit cgiFormSuccess \n");
		CGICDEBUGEND
#endif
		ret=WriteConfDat();
		if(0==ret)
		{
			SubmitHandle();
			system("echo 6 > /data/uplog");
		}

		return 0;
	}

	if ((cgiFormSubmitClicked("ip") == cgiFormSuccess))
	{
#ifdef CGICDEBUG
		CGICDEBUGSTART
		fprintf(dout, "ipConfig server ip submit cgiFormSuccess \n");
		CGICDEBUGEND
#endif

		ret=WriteServerConfDat();
		if(0==ret)
		{
			system("echo 6 > /data/uplog");
		}

		return 0;
	}

	return 0;
}





  

