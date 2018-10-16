cc=arm-fsl-linux-gnueabi-gcc
##cc=gcc

all:update.cgi login.cgi waterConfig.cgi ipConfig.cgi readConfig.cgi ctrConfig.cgi debugLog.cgi

update.cgi:update.c cgic.c cgic.h
	$(cc) -o update.cgi cgic.c update.c

login.cgi:login/login.c
	$(cc) -o login.cgi  login/login.c

waterConfig.cgi:waterConfig.c cgic.c cgic.h
	$(cc) -lsqlite3 -o waterConfig.cgi waterConfig.c cgic.c 

ipConfig.cgi:ipConfig.c cgic.c cgic.h
	$(cc) -o ipConfig.cgi cgic.c ipConfig.c
	
readConfig.cgi:readConfig.c cgic.c cgic.h
	$(cc) -o readConfig.cgi cgic.c readConfig.c

ctrConfig.cgi:ctrConfig.c cgic.c cgic.h
	$(cc) -o ctrConfig.cgi cgic.c ctrConfig.c

debugLog.cgi:debugLog.c cgic.c cgic.h
	$(cc) -o debugLog.cgi cgic.c cgic.h debugLog.c

clean:
	rm -rf *.cgi
