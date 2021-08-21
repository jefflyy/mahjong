#include"network.h"
int main(){
	SOCKET server=createserver(1919,4);
	SOCKET client=getclient(server);
	printf("client connected\n");
	char buf[1024];
	int len=recv(client,buf,1024,0);
	fwrite(buf,len,1,stdout);
	printf("\n");
	close(client);
	close(server);
}