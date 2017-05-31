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
#include <io.h> 
#include <stdint.h>


int HandleConnect(int fd);
int ParseReq(FILE *f,char *r);  
int PrintHeader(FILE *f,char *r);
int DoDir(FILE *f,char *name);
int DoHTML(FILE *f,char *name);
int DoText(FILE *f,char *name);
int DoJpeg(FILE *f,char *name);
int DoGif(FILE *f,char *name); 



char Refferer[256]; 
/**
*�ͻ����Ӵ���
*��������int HandleConnect(int fd)
*�������ͻ������ļ�������
*/
int HandleConnect(int fd){
	int client = fd;
	FILE *file;
	
	char request_type[255];
	int numchars;
	char buf[1024];
	char method[255];
	char *query_string = NULL;
		
	
	//1����ȡRefferer��������ֵ��ȫ�ֱ���
	//2����ȡcontent_length�ֶ���Ϣ
	//1,2��ʱ��������Ϊ�ƺ�û���� ,��ȡcontent_type�������� 
	numchars = get_line(client, buf, sizeof(buf));
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
	  
	file = fdopen(client,r); //�õ��ļ�����������ֻ����ʽ�� 
	
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
	pathname = strcat(path,r);
	char tmp[255];
	char html[10];
	char jpg_or_gif[10]
	
	strncpy(html, r+(strlen(r)-5), 5); 
    string[5] = '\0';
    strncpy(jpg_or_gif,r+(strlen(r)-4),4);
    string[4] = '\0';
    
    FILE destFile = fopen(pathname,)
	if(r[strlen(r)-1] == '/') {  			//���Ŀ¼ 
		if(access(strcat(pathname,"index.html"),0){ 	//�ж��Ƿ����index.html 
			DoHtml(f,strcat(pathname,"index.html"));
			return 1;
		}    
		DoDir(f,pathname);
	}  else if(strcasecmp(jpg_or_gif,".jpg")==0){
		DoJpeg(f,pathname);
	} else if (strcasecmp(jpg_or_gif,".gif")==0){
		DoGif(f,pathname);
	}else if (strcasecmp(html,".html")==0)  {  //���html�ļ� 
		DoHTML(f,pathname);
	} else{
		DoText(f,pathname);
	} 
	return 1;
}

/*
���� HTTP Э������ͷ��
��������int PrintHeader(FILE *f, int content_type)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д�� HTTP
Э������ͷ��Ϣ��
���� 2����Ϣ���ͣ�����ȷ�����͵� HTTP Э������ͷ��Ϣ��

*/
int PrintHeader(FILE *f,char *r){
    fprintf(f,"HTTP/1.0 200 OK\n")
    switch (*r)
	{
	case 't':
	fprintf(f,"Content-type: text/plain\n");
	break;
	case 'g':
	fprintf(f,"Content-type: image/gif\n");
	BREAK;
	case 'j':
	fprintf(f,"Content-type: image/jpeg\n");
	break;
	case 'h':
	fprintf(f,"Content-type: text/html\n");
	break;
	}
	//���ͷ�������Ϣ��
	fprintf(f,"Server: AMRLinux-httpd 0.2.4\n");
	//�����ļ�����Ϊ�������ڣ�
	fprintf(f,"Expires: 0\n");
}

/*
���͵�ǰĿ¼�ļ��б���Ϣ��
��������int DoDir(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д��Ŀ¼�ļ���Ϣ
���ݡ�
���� 2��Ŀ¼������ʾ�ͻ������Ŀ¼��Ϣ��
*/
int DoDir(FILE *f,char *name){
	
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

	int client = fileno(f);    //���ļ�ָ��ת��Ϊ�׽��֣���������httpd.c�ĺ��� 
    resource = fopen(name, "r");
    if (resource == NULL)
        //not_found(client);
    else
    {
        PrintHeader(f, 'h');
        fgets(buf, sizeof(buf), resource);
	    while (!feof(resource))
	    {
	    	fprintf(f,buf);
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
	
}

/*
���� Jpeg ͼ���ļ����ݣ�
��������int DoJpeg(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ���
*/

int DoJpeg(FILE *f,char *name){
	
}

/*
���� GIF ͼ���ļ����ݣ�
��������int DoGif(FILE *f, char *name)
���� 1���ļ��� FILE �ṹָ�룬���ڱ�ʾ�ͻ����ӵ��ļ���ָ�롣����д���ļ���Ϣ���ݡ�
���� 2���ͻ�������ļ�����
*/
int DoGif(FILE *f,char *name){
	
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

int main(void)
{
    int server_sock = -1;
    u_short port = 4000;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    server_sock = startup(&port);
    printf("httpd running on port %d\n", port);

    while (1)
    {
        client_sock = accept(server_sock,
                (struct sockaddr *)&client_name,
                &client_name_len);
//        if (client_sock == -1)
//            error_die("accept");
            HandleConnect(client_sock); 
        //accept_request(&client_sock); 
       
    }

    close(server_sock);

    return(0);
}
