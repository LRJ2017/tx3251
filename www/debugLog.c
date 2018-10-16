/*
 * debugLog.c
 *
 *  Created on: 2018年7月10日
 *      Author: linux
 */
#define FILE_LINE_LEN 4096
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

long g_cur_offset = 0;
char text[FILE_LINE_LEN] = { 0 };
int debug_flag = 0;
int type = 0;

int32_t c_tail(const char *file) {
	memset(text, 0x00, FILE_LINE_LEN);
	if (!file)
		return -1;
	FILE *fp = fopen(file, "r");
	if (!fp) {
		return -2;
	}

	int ret = fseek(fp, g_cur_offset, SEEK_SET);
	int32_t len = 0;
	if (ret == 0) {
		while (!feof(fp)) {
			char line[FILE_LINE_LEN] = { 0 };
			fgets(line, FILE_LINE_LEN, fp);
			int32_t count = strlen(line);
			if((len + count) > FILE_LINE_LEN)
			{
				break;
			}
			memcpy(text + len, line, count);
			len += count;
		}
	}
	g_cur_offset += len;
	fclose(fp);
	return 0;
}
//分割字符串
/*****************************************
 *output-> dst:子串数组
 *intput-> str：待分割字符串
 *intput-> spl：分隔符
 *return-> n 子串的个数
 * ***************************************/
int split(unsigned char dst[][20], char* str, char* spl) {
	int n = 0;
    int len = 0;
    char* tmp = str;
	while (*str != 0x00) {
		if(*str == *spl)
		{
			strncpy(dst[n], tmp, len);
			tmp = str + 1;
			len = -1;
			n++;
		}

		str++;
		len++;
	}

	if(len > 0)
	{
		strncpy(dst[n], tmp, len);
		n++;
	}
	return n;
}

void phase_get_request_params(char* user_input) {
	unsigned char dst[3][20] = { 0x00 };
	char split_flag = '&';
	int cnt = split(dst, user_input, &split_flag);
#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "dst0:%s\n",dst[0]);
			fprintf(dout, "dst1:%s\n",dst[1]);
			fprintf(dout, "dst2:%s\n",dst[2]);
	CGICDEBUGEND
#endif
	unsigned char param[2][20] = { 0x00 };
	long flag[3] = { 0 };
	int i;
	split_flag = '=';
	for (i = 0; i < cnt - 1; i++) {
		if (2 == split(param, dst[i], &split_flag)) {
			flag[i] = atol(param[1]);
		}
		else
		{
		#ifdef CGICDEBUG
			CGICDEBUGSTART
					fprintf(dout, "test:%s\n", dst[i]);
			CGICDEBUGEND
		#endif
		}
	}
	debug_flag = flag[0] & 0xFF;
	type = flag[1] & 0xFF;
	g_cur_offset = flag[2];
#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "debug_flag:%d\n",debug_flag);
			fprintf(dout, "type:%d\n",type);
			fprintf(dout, "g_cur_offset:%ld\n",g_cur_offset);
	CGICDEBUGEND
#endif
}

void get_requist_log_content(void) {
	int32_t ret = 0;
	char debug_log[50] = "/data/log/";
	char logDate[20] = {0x00};
	time_t s;
	struct tm *p;
	s = time((time_t*) NULL);
	p = localtime(&s);

	char tmp_log_type;
	if (type == 0)
		tmp_log_type = 'a';
	else if (type == 1)
		tmp_log_type = 'c';
	else if (type == 2)
		tmp_log_type = 's';
	else if (type == 3)
			tmp_log_type = 'd';
	sprintf(logDate, "%d-%02d-%02d-%c", 1900 + p->tm_year,
			1 + p->tm_mon, p->tm_mday, tmp_log_type);
	strcat(debug_log, logDate);
	strcat(debug_log, ".log");

	FILE *stream = NULL;
	//比较文件名，发生变化时，重新从0读取
	char cmdFileName[100] = { 0 };
	char fileName[50] = { 0 };
	sprintf(cmdFileName, "cat /data/logFileName | awk '{print $1}'");
	stream = popen(cmdFileName, "r");
	fgets(fileName, 256, stream);
	pclose(stream);
	if(strncmp(fileName, debug_log, strlen(debug_log)) != 0)
	{
		g_cur_offset = 0;
	}

	memset(cmdFileName, 0, 100);
	system("chmod 777 /data/logFileName");
	sprintf(cmdFileName, "echo \"%s\" > /data/logFileName", debug_log);
	system(cmdFileName);

	//记录停止记录日志时间（一小时后停止）
	char logTime[20] = {0};
	sprintf(logTime, "%d-%02d-%02d %02d:%02d", 1900 + p->tm_year,
			1 + p->tm_mon, p->tm_mday, p->tm_hour + 1, p->tm_min);
	char cmd[200] = {0};
	system("chmod 777 /data/logTime");
	sprintf(cmd, "echo \"%s\" > /data/logTime", logTime);
	system(cmd);

#ifdef CGICDEBUG
	CGICDEBUGSTART
	fprintf(dout, "g_cur_offset:%d\n", g_cur_offset);
	fprintf(dout, "debug_log:%s len=%d\n", debug_log, strlen(debug_log));
	fprintf(dout, "prefileName:%s len=%d\n", fileName, strlen(fileName));
	CGICDEBUGEND
#endif

	//首次发起请求时获取当前文件的字节数大小
	//例如：wc -c 2018-07-03-c.log | awk {'print$1'}
	if (g_cur_offset == 0) {
		stream = NULL;
		char cmdBuf[256] = { 0 };
		char recvBuf[256] = { 0 };
		sprintf(cmdBuf, "wc -c %s | awk '{print $1}'", debug_log);
		stream = popen(cmdBuf, "r");
		if (!stream) {
			fprintf(cgiOut, "cur_offset**=**%ld**/**", g_cur_offset);
			fprintf(cgiOut, "cur_log_content**=**%s**/**", text);

			return;
		}
		fgets(recvBuf, 256, stream);
		pclose(stream);
		g_cur_offset = atol(recvBuf);
	}

	ret = c_tail(debug_log);

#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "debug_flag:%d\n",debug_flag);
			fprintf(dout, "cur_offset:%ld\n",g_cur_offset);
			fprintf(dout, "cur_log_content:%s\n",text);
	CGICDEBUGEND
#endif

	if (ret == -2) {
#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "%s open file failed\n", debug_log);
	CGICDEBUGEND
#endif
	} else if (ret == -1) {
#ifdef CGICDEBUG
	CGICDEBUGSTART
			fprintf(dout, "file:%s is null! \n", debug_log);
	CGICDEBUGEND
#endif
	}

	fprintf(cgiOut, "cur_offset**=**%ld**/**", g_cur_offset);
	fprintf(cgiOut, "cur_log_content**=**%s**/**", text);
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
			return -1;
		}
		UserInput[data_len] = '\0';
		//解析get请求参数
		phase_get_request_params(UserInput);
		get_requist_log_content();
		return 0;
	}
	return 0;
}

