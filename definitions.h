#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <regex.h>

//important includes
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>

#define Max_Packet 12288
#define INDEXGET 1
#define FILEHASH 2
#define FILEDOWNLOAD 3
#define FILEUPLOAD 4
#define QUIT 5
#define ERROR -1
#define DOWHILE(x,a) for(i = x; i < a; i++)
#define INFINITELOOP() for(;;) 
#define GETSOCKET socket
#define BINDTOPORT bind
#define watse for(k=0;k<1000;k++)

//GLOBAL VARIABLES
struct info_for_printing 	{
	char filename[100]; 	//filename
	 off_t size; 			//size
	time_t modified_time; 			//last modified
	  char type;			//filetype
};

struct inform  {
	char siz;
		char t;
	char mt;
};

struct info_for_md5hash
{
	char *filename; 							//filename
		unsigned char md5[MD5_DIGEST_LENGTH]; 	//md5
	  time_t modified_time; 							//last modified
};

struct connection {
	int portc;
   int portl;
      char *ip;
};

//Printing Purposes
struct info_for_printing printdata[1024];
struct info_for_md5hash  hashdata[1024];

//Storing History
char his[1024][1024] = {0};


char fileDownloadName[1024];
 char Response_for_query[Max_Packet];
  char variable[5];

int i;
	int hist_count_variable_index = 0;
  int Error_code = -1;

//Some boolean variables
int RegexFlag = 0;
 int is_tcp_client = 0;


//Function Declarations
//Servers and Client
//TCP
int tcp_server(char *portno_listen);
int tcp_client(char *ip_address, char *portno_connect);

//UDP
int udp_server(char *portno_listen);
int udp_client(char *ip_address, char *portno_connect);

//parser
int parse_request(char *request);

//IndexGet Functions
void handle_Index_Get(char *request);
int handle_IndexGet_Long_List();
int handle_IndexGet_Short_List(time_t start_time,time_t end_time);

//FileHash Functions
void handle_FileHash(char *request);
int handle_CheckAll();
int handle_Verify(char *file);

//FileDownload Functions
void handle_FileDownload(char *request);
int handleFileDownload(char *filename);

//utility
void Print(char *string);
