/*************************************************************************
	> File Name: http-service.c
	> Author: xyx
	> Mail:719660015@qq.com 
	> Created Time: Sat 17 Oct 2020 03:03:15 AM PDT
 ************************************************************************/

#include"lunp_s.h"
#include<sys/stat.h>
#include<dirent.h>
#include<signal.h>
int http_request(int cfd, int epfd);

int main(int argc, char *argv)
{
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, NULL);
    //char path[255] = {0};
    //sprintf(path, "/home/xyx/Documents/SourceCode/cpptest/network/http");
    //chdir(path);

    int lfd = tcp4Listen();

    int epfd = epoll_create(1024);

    if(epfd < 0)
    {
        perror("epoll_create error");
        close(lfd);
        return -1;
    }

    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    int nready;
    struct epoll_event events[1024];
    char cIP[16];
    struct sockaddr_in cli;

    while(1)
    {
        nready = epoll_wait(epfd, events, 1024, -1);
        if(nready < 0)
        {
            if(errno == EINTR)
                continue;
            break;
        }

        for(int i = 0; i < nready; ++i)
        {
            int sockfd = events[i].data.fd;
            //connect
            if(sockfd == lfd)
            {
                bzero(&cli, sizeof(cli));
                socklen_t len = sizeof(cli);
                int cfd = Accept(lfd, (struct sockaddr *)&cli, &len);

                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                memset(cIP, 0x00, sizeof(cIP));
                printf("%s:%d connect\n", inet_ntop(AF_INET, &cli.sin_addr.s_addr, cIP, sizeof(cIP)), ntohs(cli.sin_port));

                ev.data.fd = cfd;
                ev.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
            }
            else
            {
                http_request(sockfd, epfd);
            }
            //data
        }
    }
	return 0;
}

char *get_mime_type(char *name)
{
    char* dot;

    dot = strrchr(name, '.');
    if (dot == (char*)0)
        return "text/plain; charset=utf-8";
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp( dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";

    return "text/plain; charset=utf-8";
}


int hexit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return 0;
}

void strdecode(char *to, char *from)
{
    for ( ; *from != '\0'; ++to, ++from) {

        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2])) {

            *to = hexit(from[1])*16 + hexit(from[2]);
            from += 2;
        } else
            *to = *from;
    }
    *to = '\0';
}

int send_header(int cfd, char *code, char *msg,char *fileType, int length) {
    char buf[BUFFSIZE] = {0};
    sprintf(buf, "HTTP/1.1  %s %s \r\n", code, msg);
    sprintf(buf + strlen(buf), "Content-Type:%s\r\n", fileType);
    if (length > 0)
        sprintf(buf + strlen(buf), "Content-Length:%d\r\n", length);
    strcat(buf, "\r\n");
    Writen(cfd, buf, strlen(buf));
    return 0;
}

int send_file(int cfd, char *fileName)
{
    int fd = open(fileName, O_RDONLY);
    if(fd < 0)
    {
        perror("open error");
        return -1;
    }

    char buf[BUFFSIZE];
    int n;
    while(1)
    {
        memset(buf, 0x00, sizeof(buf));
        n = Read(fd, buf, sizeof(buf));
        if(n <= 0)
        {
            break;
        }else
        {
            Write(cfd, buf,n);
        }
    }
}

int http_request(int cfd, int epfd)
{
    int n;
    char buf[BUFFSIZE];

    memset(buf, 0x00, sizeof(buf));
    n = ReadLine(cfd, buf, sizeof(buf));
    if(n <= 0)
    {
        close(cfd);
        epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
        return -1;
    }

    char reqType[16]   = {0};
    char fileName[255] = {0};
    char protocal[16]  = {0};

    sscanf(buf, "%[^ ] %[^ ] %[^ \r\n]", reqType, fileName, protocal);
    printf("reqType:[%s], fileName:[%s], protocal:[%s]\n", reqType, fileName, protocal);

    char *pFile = fileName;
    if(strlen(fileName) <= 1)
        strcpy(pFile, "./");
    else
        pFile = fileName + 1;
    strdecode(pFile, pFile);
    while( (n = ReadLine(cfd, buf, sizeof(buf)) ) > 0);

    struct stat st;
    if(stat(pFile, &st) < 0)
    {
        printf("file not exist \n");

        send_header(cfd, "404", "NOT FOUND", get_mime_type(".html"),  0);

        send_file(cfd, "error.html");
    }
    else
    {
        if(S_ISREG(st.st_mode))
        {
            send_header(cfd, "200", "OK", get_mime_type(pFile), st.st_size);
            send_file(cfd, pFile);
        }
        else if(S_ISDIR(st.st_mode))
        {
            //FTP
            send_header(cfd, "200", "OK", get_mime_type(".html"), 0);
            send_file(cfd, "dir_header.html");

            char filebuf[BUFFSIZE];

            struct dirent **namelist;
            int file_num;
            file_num = scandir(pFile, &namelist, NULL, alphasort);
            if(file_num < 0)
            {
                perror("scandir error");
                close(cfd);
                epoll_ctl(epfd, EPOLL_CTL_DEL, cfd, NULL);
                return -1;
            }
            else
            {
                int index = 0;
                while(index < file_num)
                {
                    //printf("%s\n", namelist[index]->d_name);
                    memset(filebuf, 0x00, sizeof(filebuf));
                    if(namelist[index]->d_type == DT_DIR)
                        sprintf(filebuf,"<li><a href=%s/> %s </a></li>", namelist[index]->d_name, namelist[index]->d_name);
                    else
                        sprintf(filebuf,"<li><a href=%s> %s </a></li>", namelist[index]->d_name, namelist[index]->d_name);
                    Write(cfd, filebuf, strlen(filebuf));
                    free(namelist[index]);
                    index++;
                }
                free(namelist);
            }

            send_file(cfd, "dir_tail.html");

        }

    }

}
