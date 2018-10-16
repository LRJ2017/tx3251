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

char autoip[20] = { 0 };
char ipaddress[30] = { 0 };
char netmask[30] = { 0 };
char gateway[30] = { 0 };
char dns1[30] = { 0 };
char dns2[30] = { 0 };
char *autoiptable[] = { "1" };
char netmode[2] = { 0 };
char *netmodetable[] = { "0", "1", "2" };
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

static void CopyIndexStr(char *dest, char *src) {
	int i = 0;
	for (i = 0; i < 30; i++) {
		if ('=' != src[i]) {
			dest[i] = src[i];
		} else {
			break;
		}
	}
	dest[i] = '\0';
}
/*
 Find the '=' pos and get the config dat
 */
static int DatPos(char *dat, int index) {
	int i = 0;
	if (index == 0) //find the config file
			{
		while ((*dat++) != '=') {
			i++;
		}
		return i;
	} else //find the *.so file
	{
		while (*(dat + 1) && *dat) {
			if (*dat == 's' && *(dat + 1) == 'o') {
				return 1;
			}
			dat++;
		}
		return 0;
	}
}

/*When open html we should read configfile to fix it*/
void ReadTandaConf(void) {
	FILE *fd;
	char StrLine[200];
	char ptr[30];
	int i = 0;
	if ((fd = fopen("/data/modelip.conf", "r")) == NULL) {
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file failed------ReadTandaConf\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return;
	}
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "%s open file success\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
	while (!feof(fd)) {
		fgets(StrLine, 200, fd);
		if (StrLine[0] == '#' || StrLine[0] == ' ') {
			continue;
		} else {
			i = DatPos(StrLine, 0);
			memset(ptr, 0, sizeof(ptr));
			CopyIndexStr(ptr, StrLine);
			if (!strcmp(ptr, "netmode")) {
				strncpy(netmode, &StrLine[i + 1], sizeof(netmode));
				autoip[sizeof(netmode) - 1] = '\0';
			}
			if (!strcmp(ptr, "autoip")) {
				strncpy(autoip, &StrLine[i + 1], sizeof(autoip));
				autoip[sizeof(autoip) - 1] = '\0';
			} else if (!strcmp(ptr, "ipaddress")) {
				strncpy(ipaddress, &StrLine[i + 1], sizeof(ipaddress));
				ipaddress[sizeof(ipaddress) - 1] = '\0';
			} else if (!strcmp(ptr, "netmask")) {
				strncpy(netmask, &StrLine[i + 1], sizeof(netmask));
				netmask[sizeof(netmask) - 1] = '\0';
			} else if (!strcmp(ptr, "gateway")) {
				strncpy(gateway, &StrLine[i + 1], sizeof(gateway));
				gateway[sizeof(gateway) - 1] = '\0';
			} else if (!strcmp(ptr, "dns1")) {
				strncpy(dns1, &StrLine[i + 1], sizeof(dns1));
				dns1[sizeof(dns1) - 1] = '\0';
			} else if (!strcmp(ptr, "dns2")) {
				strncpy(dns2, &StrLine[i + 1], sizeof(dns2));
				dns2[sizeof(dns2) - 1] = '\0';
			} else
				;
		}

	}
	fclose(fd);
#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "%s close file success------ReadTandaConf\n",get_current_system_date_time());
	CGICDEBUGEND
#endif

}

int WriteConfDat(void) {

	FILE *fd;
	int SoChoice232;
	int bauChoice232;
	int SoChoice485;
	int bauChoice485;
	int controllertypechoice;
	int autoselect[1];
	int i;
	int result, invalid;
	ReadTandaConf();
	result = cgiFormCheckboxMultiple("autoip", autoiptable, 1, autoselect,
			&invalid);
	cgiFormStringNoNewlines("ipaddress", ipaddress, sizeof(ipaddress));
	cgiFormStringNoNewlines("netmask", netmask, sizeof(netmask));
	cgiFormStringNoNewlines("gateway", gateway, sizeof(gateway));
	cgiFormStringNoNewlines("dns1", dns1, sizeof(dns1));
	cgiFormStringNoNewlines("dns2", dns2, sizeof(dns2));
	if (cgiFormNotFound == result)
		strcpy(autoip, "0");
	else
		strcpy(autoip, "1");
	int netmodeselect[3] = { 0 };
	result = cgiFormCheckboxMultiple("netmode", netmodetable, 3, netmodeselect,
			&invalid);
	for (i = 0; i < 3; i++) {
		if (netmodeselect[i]) {
			char val[1] = { 0 };
			sprintf(val, "%d", i);
			strcpy(netmode, val);
			break;
		}
	}

	fprintf(cgiOut, "netmode=%s", netmode);
	fprintf(cgiOut, "autoip=%s", autoip);
	fprintf(cgiOut, "ipaddress=%s", ipaddress);
	fprintf(cgiOut, "netmask=%s", netmask);
	fprintf(cgiOut, "gateway=%s", gateway);
	fprintf(cgiOut, "dns1=%s", dns1);
	fprintf(cgiOut, "dns2=%s", dns2);

	fd = fopen("/data/modelip.conf", "w");
	if (fd) {
		fputs("###################################\n", fd);
		fputs("#    Tanda Adapter ipConfig File    #\n", fd);
		fputs("###################################\n\n", fd);

		fputs("#netmode\nnetmode=", fd);
		fputs(netmode, fd);
		fputs("\n\n", fd);

		fputs("#autoip\nautoip=", fd);
		fputs(autoip, fd);
		fputs("\n\n", fd);

		fputs("#ipaddress\nipaddress=", fd);
		fputs(ipaddress, fd);
		fputs("\n\n", fd);

		fputs("#netmask\nnetmask=", fd);
		fputs(netmask, fd);
		fputs("\n\n", fd);

		fputs("#gateway\ngateway=", fd);
		fputs(gateway, fd);
		fputs("\n\n", fd);

		fputs("#dns1\ndns1=", fd);
		fputs(dns1, fd);
		fputs("\n\n", fd);

		fputs("#dns2\ndns2=", fd);
		fputs(dns2, fd);
		fputs("\n\n", fd);

		fclose(fd);

	} else {
#ifdef CGICDEBUG
	CGICDEBUGSTART
		fprintf(dout, "%s open file:/data/modelip.conf failed!------WriteConfDat\n",get_current_system_date_time());
	CGICDEBUGEND
#endif
		return -3;
	}
	return 0;

}
void RequistConfig(void) {
	ReadTandaConf();
	fprintf(cgiOut, "netmode=%s/", netmode);
	fprintf(cgiOut, "autoip=%s/", autoip);
	fprintf(cgiOut, "ipaddress=%s/", ipaddress);
	fprintf(cgiOut, "netmask=%s/", netmask);
	fprintf(cgiOut, "gateway=%s/", gateway);
	fprintf(cgiOut, "dns1=%s/", dns1);
	fprintf(cgiOut, "dns2=%s/", dns2);

}

int cgiMain() {
	int ret;
	char *UserInput = NULL; 		// index the user input data.
	char *request_method = NULL; 	//index the html transfer type.
	int data_len = 0;		// index the input data's length.
	int lock = 0;
	printf("Content-type:text/html;charset=utf-8\n\n");
	if (!getenv("REQUEST_METHOD")) {
		return -1;
	}
	request_method = getenv("REQUEST_METHOD"); // trans-type.
	// trans-type : GET
	if (!strcmp(request_method, "GET")) {
		if (getenv("QUERY_STRING")) {
			UserInput = getenv("QUERY_STRING");
		}
		data_len = strlen(UserInput);

		if ( NULL == UserInput || 0 == data_len) {
#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "There's no input data !\n");
	CGICDEBUGEND
#endif
			return -1;
		}
		UserInput[data_len] = '\0';
		if (!strncmp(UserInput, "getconf", 7)) {
			RequistConfig();
		}

		return 0;
	}

	if ((cgiFormSubmitClicked("ipaddress") == cgiFormSuccess)) {
		ret = WriteConfDat();
		system("echo 6 > /data/uplog");

		return 0;
	}
	return 0;
}

