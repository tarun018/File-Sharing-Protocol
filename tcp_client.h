int tcp_client(char *ip_address,char *portno_connect)
{
	int socket_fd = 0;
	int n = 0;

	int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    //Arrays for Printing and Reading Info
    char Buffer_Write[1024];
	char Buffer_Read[1024];

	struct sockaddr_in serv_addr;
	int portno = atoi(portno_connect);

	//Download and Upload File Name
	char Download_File_Name[1024];
	char Upload_File_Name[1024];

	//clearing read write arrays
	memset(Buffer_Read, 0,sizeof(Buffer_Read));
	memset(Buffer_Write, 0,sizeof(Buffer_Write));

	//getting file descriptor
	if((socket_fd = GETSOCKET(AF_INET, SOCK_STREAM, 0)) < 0)	{
		printf("\n Error : Could not create socket \n");
		return 1;
	} 
	memset(&serv_addr, 0, sizeof(serv_addr)); 

	//struct addr_in: IPv4
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno); 

	if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0)	{
		printf("\n inet_pton Error_code occured\n");
		return 1;	} 

	for(;;)
	{
		//trying to connect to the server with portnumber
		if(connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			continue;
		else	{
			if(strcmp(variable,"udp"))
				printf("Client is Connected now : \n");
			break;	}
	}

	int a;
	int count = 0;
	int filedownload = 0;
	int fileupload = 0;
 
	while(1)
	{

		printf("\nPlease Enter Your Command Here: ");

		//reinitializing
		filedownload = 0;
		fileupload = 0;

		//File pointer
		FILE *fp = NULL;
		int i;
		char *cresponse = malloc(Max_Packet);
		fgets(Buffer_Write, sizeof(Buffer_Write), stdin);

		char *filename;
		char copy[1024];

		strcpy(copy,Buffer_Write);
		filename = malloc(1024);
		filename = strtok(copy," \n");
		
		//quit
		if(strcmp(filename,"quit") == 0){
			_exit(1);
		}
		
		//File is to be Downloaded
		if(strcmp(filename,"FileDownload") == 0)	{
			filedownload = 1;
			filename = strtok(NULL," \n");
			strcpy(Download_File_Name,filename);
			fp = fopen(Download_File_Name,"wb");
			/////////////////NUll = fp
			if (fp == NULL)	{
				perror ("File Not Found\n");
				continue;	}
		}

		//File is to be uploaded
		if(strcmp(filename,"FileUpload") == 0)	{
			fileupload = 1;

			filename = strtok(NULL," \n");
			strcpy(Upload_File_Name,filename);

			FILE *f = fopen(Upload_File_Name, "r");
			fseek(f, 0, SEEK_END);
			unsigned long len = (unsigned long)ftell(f);

			char size[1024];
			memset(size, 0, sizeof(size));
			sprintf(size,"%ld\n",len);
			strcat(Buffer_Write,size);
			fclose(f);	}

		send(socket_fd, Buffer_Write , strlen(Buffer_Write), 0);

		n = recv(socket_fd, Buffer_Read, sizeof(Buffer_Read)-1, 0);
		size_t bytes_read;

		//Upload Accepted 
		if(strcmp(Buffer_Read,"FileUploadAccept\n") == 0)
		{
			int b,c;
			printf("Upload Accepted\n");
			handle_Verify(Upload_File_Name);

			b = 0;
			while(b < 1)
			{
				sprintf(cresponse, "%s, ",hashdata[b].filename);
				strcat(Buffer_Write,cresponse);
				for (c = 0 ; c < MD5_DIGEST_LENGTH ; c++)
				{
					sprintf(cresponse, "%02x",hashdata[b].md5[c]);
					strcat(Buffer_Write,cresponse);
				}
				sprintf(cresponse, ", %s",ctime(&hashdata[b].modified_time));
				strcat(Buffer_Write,cresponse);
				b++;
			}

			write(socket_fd , Buffer_Write , bytes_read);
			printf("%s\n",Buffer_Write);
			memset(Buffer_Write, 0, sizeof(Buffer_Write));

			fp = fopen(Upload_File_Name,"rb");
			while(!feof(fp))	{
				int mati[1817],zah,paq,qah,raq;
    			for(paq=80;qah+paq-80;paq-=2*mati[paq])for(zah=9;zah--;)qah=3&(raq=time(0)+raq*57)/7,qah=qah?qah-1?qah-2?1-paq%79?-1:0:paq%79-77?1:0:paq<1659?79:0:paq>158?-79:0,qah?!mati[paq+qah*2]?mati[paq+=mati[paq+=qah]=qah]=qah:0:0;for(;qah++-1817;);
				bytes_read = fread(cresponse, 1, 1024, fp);
				cresponse[1024] = 0;
				memcpy(Buffer_Write,cresponse,bytes_read);
				write(socket_fd , Buffer_Write , bytes_read);
				memset(Buffer_Write, 0, sizeof(Buffer_Write));
				memset(cresponse, 0, sizeof(cresponse));	}

			memcpy(Buffer_Write,"~@~",3);
			write(socket_fd , Buffer_Write , 3);
			memset(Buffer_Write, 0, sizeof(Buffer_Write));
			memset(Buffer_Read, 0, strlen(Buffer_Read));
			fclose(fp);
		}
		//Upload Denied
		else if(strcmp(Buffer_Read,"FileUploadDeny\n") == 0)	{
			printf("Upload Denied\n");
			memset(Buffer_Read, 0,sizeof(Buffer_Read));
			continue;
		}
		else
		{
			for(;;)
			{
				Buffer_Read[n] = 0;
				if(Buffer_Read[n-1] == '~' && Buffer_Read[n-3] == '~' && Buffer_Read[n-2] == '@')
				{
					Buffer_Read[n-3] = 0;
					if(filedownload == 1)	{
						fwrite(Buffer_Read,1,n-3,fp);
						fclose(fp);	}
					else{
						strcat(cresponse,Buffer_Read);
					}
					memset(Buffer_Read, 0,strlen(Buffer_Read));
					break;
				}
				else
				{
					if(filedownload == 1){
						fwrite(Buffer_Read,1,n,fp);
					}
					else{
						strcat(cresponse,Buffer_Read);
					}
					memset(Buffer_Read, 0,strlen(Buffer_Read));
				}
				n = recv(socket_fd, Buffer_Read, sizeof(Buffer_Read)-1, 0);
				if(n < 0){
					break;
				}
			}
		}

		if(filedownload == 0){
			printf("%s\n",cresponse);
		}
		else{
			printf("File Downloaded\n");
		}

		if(n < 0){
			printf("\n Read Error_code \n");
		}
		memset(Buffer_Read, 0,sizeof(Buffer_Read));
		memset(Buffer_Write, 0,sizeof(Buffer_Write));
	}
	return 0;
}
