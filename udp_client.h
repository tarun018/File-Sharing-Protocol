int udp_client(char *ip_address,char *portno_connect)
{
	int sockfd = 0, n = 0;
	char Buffer_Read[1024];
	char Buffer_Write[1024];
	struct sockaddr_in serv_addr;
	int portno = atoi(portno_connect);
	char Download_File_Name[1024];
	char Upload_File_Name[1024];

	memset(Buffer_Read, 0,sizeof(Buffer_Read));
	memset(Buffer_Write, 0,sizeof(Buffer_Write));
	if((sockfd = GETSOCKET(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	} 

	memset(&serv_addr, 0, sizeof(serv_addr)); 

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno); 

	if(inet_pton(AF_INET, ip_address, &serv_addr.sin_addr)<=0)
	{
		printf("\n inet_pton Error_code occured\n");
		return 1;
	} 

	int a , count = 0 , filedownload = 0 , fileupload = 0;

	INFINITELOOP()
	{
		filedownload = 0;
		fileupload = 0;
		FILE *fp = NULL;
		int i;
		char *cresponse = malloc(Max_Packet);
		fgets(Buffer_Write,sizeof(Buffer_Write),stdin);

		char *filename;
		char copy[1024];
		strcpy(copy,Buffer_Write);
		filename = malloc(1024);
		filename = strtok(copy," \n");
		if(strcmp(filename,"FileDownload") == 0)
		{
			filedownload = 1;
			filename = strtok(NULL," \n");
			strcpy(Download_File_Name,filename);
			fp = fopen(Download_File_Name,"wb");
		}
		if(strcmp(filename,"FileUpload") == 0)
		{
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
			fclose(f);
		}
		write(sockfd, Buffer_Write , strlen(Buffer_Write));

		n = read(sockfd, Buffer_Read, sizeof(Buffer_Read)-1);
		size_t bytes_read;
		if(strcmp(Buffer_Read,"FileUpload Accept\n") == 0)
		{
			int b,c;
			printf("Response 1\nUpload Accepted\n");
			handle_Verify(Upload_File_Name);
			for (b = 0 ; b < 1 ; b++)
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
			}
				write(sockfd , Buffer_Write , bytes_read);
				printf("%s\n",Buffer_Write);
				memset(Buffer_Write, 0, sizeof(Buffer_Write));
			fp = fopen(Upload_File_Name,"rb");
			while(!feof(fp))
			{
				bytes_read = fread(cresponse, 1, 1024, fp);
				cresponse[1024] = 0;
				memcpy(Buffer_Write,cresponse,bytes_read);
				write(sockfd , Buffer_Write , bytes_read);
				memset(Buffer_Write, 0, sizeof(Buffer_Write));
				memset(cresponse, 0, sizeof(cresponse));
			}
			memcpy(Buffer_Write,"~@~",3);
			write(sockfd , Buffer_Write , 3);
			memset(Buffer_Write, 0, sizeof(Buffer_Write));
			memset(Buffer_Read, 0, strlen(Buffer_Read));
			fclose(fp);
		}
		else if(strcmp(Buffer_Read,"FileUpload Deny\n") == 0)
		{
			printf("Response 1\nUpload Denied\n");
			memset(Buffer_Read, 0,sizeof(Buffer_Read));
			continue;
		}
		else
		{
			while(1)
			{
				Buffer_Read[n] = 0;
				if(Buffer_Read[n-1] == '~' && Buffer_Read[n-3] == '~' && Buffer_Read[n-2] == '@')
				{
					Buffer_Read[n-3] = 0;
					if(filedownload == 1)
					{
						fwrite(Buffer_Read,1,n-3,fp);
						fclose(fp);
					}
					else
						strcat(cresponse,Buffer_Read);
					memset(Buffer_Read, 0,strlen(Buffer_Read));
					break;
				}
				else
				{
					if(filedownload == 1)
						fwrite(Buffer_Read,1,n,fp);
					else
						strcat(cresponse,Buffer_Read);
					memset(Buffer_Read, 0,strlen(Buffer_Read));
				}
				n = read(sockfd, Buffer_Read, sizeof(Buffer_Read)-1);
				if(n < 0)
					break;
			}
		}

		if(filedownload == 0)
			printf("%s\n",cresponse);
		else 
			printf("Response 1\nFile Downloaded\n");

		if(n < 0)
			printf("\n Read Error_code \n");
		memset(Buffer_Read, 0,sizeof(Buffer_Read));
		memset(Buffer_Write, 0,sizeof(Buffer_Write));
	}
	return 0;
}

