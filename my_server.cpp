#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
//#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
//#include <io.h> 
#include <stdint.h>


int HandleConnect(int fd);
void not_found(FILE *f);
int startup(u_short *port);
int get_line(int sock,char *buf,int size);
void error_die(const char *sc);
int ParseReq(FILE *f,char *r);  
int PrintHeader(FILE *f,char *r);
int DoDir(FILE *f,char *name);
int DoHTML(FILE *f,char *name);
int DoText(FILE *f,char *name);
int DoJpeg(FILE *f,char *name);
int DoGif(FILE *f,char *name); 

#define ISspace(x) isspace((int)(x))
#define STDIN   0
#define STDOUT  1
#define STDERR  2

char Refferer[256]; 
int aclient;
/**
*�ͻ����Ӵ���
*��������int HandleConnect(int fd)
*�������ͻ������ļ�������
*/
int HandleConnect(int fd){
        aclient = fd;
	int i,j;
	FILE *file;
	
	char request_type[255];
	int numchars;
	char buf[1024];
	char url[255];
	char method[255];
	char *query_string = NULL;
		
	
	//1����ȡRefferer��������ֵ��ȫ�ֱ���
	//2����ȡcontent_length�ֶ���Ϣ
	//1,2��ʱ��������Ϊ�ƺ�û���� ,��ȡcontent_type�������� 
	numchars = get_line(aclient, buf, sizeof(buf));
    i = 0; j = 0;
    while (!ISspace(buf[i]) && (i < sizeof(method) - 1))
    {
        method[i] = buf[i];
        i++;
    }
    j=i;
    method[i] = '\0';
    i = 0;
    while (ISspace(buf[j]) && (j < numchars))
        j++;
    while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars))
    {
        url[i] = buf[j];
        i++; j++;
    }
    url[i] = '\0';
	  
	file = fdopen(aclient,"rt+"); //�õ��ļ�����������ֻ����ʽ�� 
	
	if(file == NULL )
	{
	printf("file is null\n");
	} else printf("");

	ParseReq(file,url); 
	fclose(file);
	
	
}

/**
*�����ͻ�����
��������int ParseReq(FILE *f, char *r)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣
���� 2���ַ���ָ�룬���������ַ�����
*/
int ParseReq(FILE *f,char *r){
	char pathname[521] = ".";
	strcat(pathname,r);
	char tmp[521];
	char html[10];
	char jpg_or_gif[10];
	char buf[1024];
	int numchars =1;
	buf[0] = 'A';buf[1] = '\0';
	while ((numchars > 0) && strcmp("\n", buf))  /* read & discard headers */
	         numchars = get_line(aclient, buf, sizeof(buf));
	//printf("finish pathname :%s\n",pathname);
	strcpy(tmp,pathname);

	strncpy(html, r+(strlen(r)-5), 5); 
    html[5] = '\0';
    strncpy(jpg_or_gif,r+(strlen(r)-4),4);
    jpg_or_gif[4] = '\0';
    
    if(access(pathname,F_OK)!=0) { //judget the file if exist
	printf("not exist!\n");
	not_found(f);
    }else if(r[strlen(r)-1] == '/') {  			//���Ŀ¼ 
		if(access(strcat(tmp,"index.html"),F_OK) == 0){ 	//�ж��Ƿ����index.html 
			DoHTML(f,tmp);
			return 0;
		}else {
	
		DoDir(f,pathname);
		}
	}  else if(strcasecmp(jpg_or_gif,".jpg")==0){
		DoJpeg(f,pathname);
	} else if (strcasecmp(jpg_or_gif,".gif")==0){
		DoGif(f,pathname);
	}else if (strcasecmp(html,".html")==0)  {  //���html�ļ� 
		
		//printf("enter html\n");
		DoHTML(f,pathname);
	} else{
		DoText(f,pathname);
	} 
	return 0;
}

/*
���� HTTP Э������ͷ��
��������int PrintHeader(FILE *f, int content_type)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д�� HTTP
Э������ͷ��Ϣ��
���� 2����Ϣ���ͣ�����ȷ�����͵� HTTP Э������ͷ��Ϣ��

*/
int PrintHeader(FILE *f,char *r){
	char buf[1024];
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(aclient, buf, strlen(buf), 0);
    switch (*r)
	{
	case 't':
	strcpy(buf,"Content-type: text/plain\r\n");
	send(aclient, buf, strlen(buf), 0);
	break;
	case 'g':
	strcpy(buf,"Content-type: image/gif\r\n"); 
	send(aclient, buf, strlen(buf), 0);
	break;
	case 'j':
		strcpy(buf, "Content-type: image/jpeg\r\n");
	    	send(aclient, buf, strlen(buf), 0);
	break;
	case 'h':
	//printf("case 'h'\n");
	strcpy(buf, "Content-type: text/html\r\n");
	    send(aclient, buf, strlen(buf), 0);
	break;
	}
	//���ͷ�������Ϣ��
	strcpy(buf, "Server: AMRLinux-httpd 0.2.4\r\n");
	send(aclient, buf, strlen(buf), 0);
	//�����ļ�����Ϊ�������ڣ�
	strcpy(buf, "Expires: 0\r\n");
	send(aclient, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(aclient, buf, strlen(buf), 0);
}

/*
���͵�ǰĿ¼�ļ��б���Ϣ��
��������int DoDir(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д��Ŀ¼�ļ���Ϣ
���ݡ�
���� 2��Ŀ¼������ʾ�ͻ������Ŀ¼��Ϣ��
*/
int DoDir(FILE *f,char *name){
	char buf[1024];
	char buf2[1024];
	system("ls -lh --full-time | cut -c 23-  |awk '{print $1\"           \t\"$2\"     \t\"$5}'> ./tmpfile");
	PrintHeader(f,"h");
	FILE *resource = fopen("./tmpfile","r");
	sprintf(buf2,"<h3>Server file directory</h3><i>size(byte) &nbsp  date &nbsp&nbsp&nbsp &nbsp  &nbsp  &nbsp  filename<i><br>");
	//send(aclienti,buf2,strlen(buf2),0);

	fgets(buf, sizeof(buf), resource);
	strcat(buf,buf2);
	while (!feof(resource))
	{
		strcat(buf,"<br>");
		send(aclient,buf,strlen(buf),0);	
		fgets(buf, sizeof(buf), resource);
	}
	system("rm -f ./tmpfile");
}

/*���� HTML �ļ����ݣ�
��������int DoHTML(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ�����
*/

int DoHTML(FILE *f,char *name){
	
	FILE *resource = NULL;
    char buf[1024];
    buf[0] = 'A'; buf[1] = '\0';
    resource = fopen(name, "r");
    if (resource == NULL){
	printf("DoHTML():ther is no file exist");
    }
        //not_found(client);
    else
    {
        PrintHeader(f,"h");
        fgets(buf, sizeof(buf), resource);
	    while (!feof(resource))
	    {
	    send(aclient, buf, strlen(buf), 0);
	        fgets(buf, sizeof(buf), resource);
	    }

    }
    fclose(resource);
}

/*
���ʹ��ı���TXT���ļ����ݣ�
��������int DoText(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ�����
*/
int DoText(FILE *f,char *name){
	DoHTML(f,name);	
}

/*
���� Jpeg ͼ���ļ����ݣ�
��������int DoJpeg(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ���
*/

int DoJpeg(FILE *f,char *name){
	int nCount;
	char buf[1024];
	FILE * source = fopen(name,"rb");
	PrintHeader(f,"j");
	while((nCount =fread(buf,1,1024,source))>0){

		send(aclient,buf,nCount,0);
	}
	return 0;

}

/*
���� GIF ͼ���ļ����ݣ�
��������int DoGif(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ�����
*/
int DoGif(FILE *f,char *name){
	int nCount;
	char buf[1024];
	FILE *source = fopen(name,"rb");
	PrintHeader(f,"g");
	
	while((nCount =fread(buf,1,1024,source))>0){
	send(aclient,buf,nCount,0);
	}
	return 0;
}

int startup(u_short *port)
{
    int httpd = 0;
    int on = 1;
    struct sockaddr_in name;

    httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)  
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)  
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0)  
    {  
        error_die("setsockopt failed");
    }
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0)
        error_die("bind");
    if (*port == 0)  /* if dynamically allocating a port */
    {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, 5) < 0)
        error_die("listen");
    return(httpd);
}

int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';

    return(i);
}

/**********************************************************************/
void error_die(const char *sc)
{
    perror(sc);
        exit(1);
}

void not_found(FILE *f){
	char buf[1024];
	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "Server: AMRLinux-httpd 0.2.4\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(aclient, buf, strlen(buf), 0);
        sprintf(buf, "\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
 	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "is unavailable or nonexistent.\r\n");
	send(aclient, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(aclient, buf, strlen(buf), 0);
}
void no2t_found(FILE *f)
{
	char buf[1024];

	int client = fileno(f);

	sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Server: AMRLinux-httpd 0.2.4\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/html\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "your request because the resource specified\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "  sssis unavailable or nonexistent.\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "</BODY></HTML>\r\n");
	send(client, buf, strlen(buf), 0);

	
}


int main(void)
{
    int server_sock = -1;
    u_short port = 4002;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    server_sock = startup(&port);
    printf("starting httpd...\n");

    while (1)
    {
    	printf("wait for connection");
        client_sock = accept(server_sock,
                (struct sockaddr *)&client_name,
                &client_name_len);
        if (client_sock == -1)
            error_die("accept");
	    //printf("i got it ,client_sock is %d\n",client_sock);
            HandleConnect(client_sock); 
       
    }

    close(server_sock);

    return 0;
    }
