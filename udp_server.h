int udp_server(char *portno_listen) {
	int listen_file_descriptor = 0;
	int connect_file_descriptor = 0;

	struct sockaddr_in  serv_addr; 
	int portno = atoi(portno_listen);


	char Buffer_Write[1024];
	char Buffer_Read[1024];

	time_t ticks; 

	listen_file_descriptor = GETSOCKET(AF_INET, SOCK_DGRAM, 0);
	memset(Buffer_Write, 0, sizeof(Buffer_Write)); 
	memset(Buffer_Read, 0, sizeof(Buffer_Read)); 
	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_port = htons(portno); 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_family = AF_INET;

	BINDTOPORT(listen_file_descriptor, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 


	int a , n , b , c;
	n = read(listen_file_descriptor,Buffer_Read,sizeof(Buffer_Read));


	for( ;n > 0;)		{

		size_t size = strlen(Buffer_Read) + 1;
		char *request = malloc(size);
		strcpy(request,Buffer_Read);

		printf("%s\n",request);
		fflush(stdout);

		strcpy(his[hist_count_variable_index],Buffer_Read);
		hist_count_variable_index++;

		int request_type = parse_request(request);
		if(request_type == ERROR)      {	Error_code = 1;	sprintf(Response_for_query,"ERROR: No request of this type.\n");	}

		else if(request_type == INDEXGET)      //Indexget
			handle_Index_Get(request);
		else if(request_type == FILEHASH)      //FileHash
			handle_FileHash(request);
		else if(request_type == FILEDOWNLOAD)      //FileDownload
			handle_FileDownload(request);

		if(Error_code == 1)		  {
			strcat(Buffer_Write,Response_for_query);
			strcat(Buffer_Write,"~@~");
			write(listen_file_descriptor , Buffer_Write , strlen(Buffer_Write));
			Error_code = -1;
			memset(Buffer_Write, 0, sizeof(Buffer_Write));
			memset(Buffer_Read, 0, sizeof(Buffer_Read)); 
			while((n = read(listen_file_descriptor,Buffer_Read,sizeof(Buffer_Read)))<=0);
			continue;}

		if(request_type == INDEXGET && RegexFlag == 1)	{
			sprintf(Response_for_query, "Response %d\n" , i);
			strcat(Buffer_Write,Response_for_query);

			a = 0;
			while(a < i)
			{
				sprintf(Response_for_query, "%s, %d, %c\n" , printdata[a].filename , printdata[a].size , printdata[a].type );
				strcat(Buffer_Write,Response_for_query);
			}
			
			strcat(Buffer_Write,"~@~");

			write(listen_file_descriptor , Buffer_Write , strlen(Buffer_Write));	}
		else if(request_type == INDEXGET)
		{
			sprintf(Response_for_query, "Response %d\n" , i);
			strcat(Buffer_Write,Response_for_query);

			for(a = 0 ; a < i ; a++)
			{
				sprintf(Response_for_query, "%s, %d, %c, %s" , printdata[a].filename , printdata[a].size , printdata[a].type , ctime(&printdata[a].modified_time));
				strcat(Buffer_Write,Response_for_query);
			}
			strcat(Buffer_Write,"~@~");

			write(listen_file_descriptor , Buffer_Write , strlen(Buffer_Write));
		}
		else if(request_type == FILEHASH)
		{
			sprintf(Response_for_query, "Response %d\n" , i);
			strcat(Buffer_Write,Response_for_query);

			for (b = 0 ; b < i ; b++)
			{
				sprintf(Response_for_query, "%s, ",hashdata[b].filename);
				strcat(Buffer_Write,Response_for_query);
				for (c = 0 ; c < MD5_DIGEST_LENGTH ; c++)
				{
					sprintf(Response_for_query, "%02x",hashdata[b].md5[c]);
					strcat(Buffer_Write,Response_for_query);
				}
				sprintf(Response_for_query, ", %s",ctime(&hashdata[b].modified_time));
				strcat(Buffer_Write,Response_for_query);
			}
			strcat(Buffer_Write,"~@~");

			write(listen_file_descriptor , Buffer_Write , strlen(Buffer_Write));
		}
		else if(request_type == FILEDOWNLOAD)
		{
			FILE* fp;
			fp = fopen(fileDownloadName,"rb");
			size_t bytes_read;
			while(!feof(fp))
			{
				bytes_read = fread(Response_for_query, 1, 1024, fp);
				memcpy(Buffer_Write,Response_for_query,bytes_read);
				write(listen_file_descriptor , Buffer_Write , bytes_read);
				memset(Buffer_Write, 0, sizeof(Buffer_Write));
				memset(Response_for_query, 0, sizeof(Response_for_query));
			}
			memcpy(Buffer_Write,"~@~",3);
			write(listen_file_descriptor , Buffer_Write , 3);
			memset(Buffer_Write, 0, sizeof(Buffer_Write));
			fclose(fp);
		}
		else if(request_type == FILEUPLOAD)
		{
			printf("FileUpload Accepted\n");
			memcpy(Buffer_Write,"FileUpload Accept\n",18);
			write(listen_file_descriptor , Buffer_Write , 18);
			memset(Buffer_Write, 0,18);
			char copying_request[1024];
			memset(copying_request, 0,1024);
			memcpy(copying_request,request,1024);
			char *size = strtok(copying_request,"\n");
			size = strtok(NULL,"\n");
			long fsize = atol(size);
			char *request_data = NULL;
			const char delim[] = " \n";
			request_data = strtok(request,delim);
			request_data = strtok(NULL,delim);
			int f;
			int result;
			f = open(request_data, O_WRONLY | O_CREAT | O_EXCL, (mode_t)0600);
			if (f == -1) {
				perror("Error opening file for writing:");
				return 1;
			}
			result = lseek(f,fsize-1, SEEK_SET);
			result = write(f, "", 1);
			if (result < 0) {
				close(f);
				perror("Error opening file for writing:");
				return 1;
			}
			close(f);
			FILE *fp;
			fp = fopen(request_data,"wb");
			n = read(listen_file_descriptor, Buffer_Read, sizeof(Buffer_Read)-1);
			while(1)
			{
				Buffer_Read[n] = 0;
				if(Buffer_Read[n-1] == '~' && Buffer_Read[n-3] == '~' && Buffer_Read[n-2] == '@')
				{
					Buffer_Read[n-3] = 0;
					fwrite(Buffer_Read,1,n-3,fp);
					fclose(fp);
					memset(Buffer_Read, 0,n-3);
					break;
				}
				else
				{
					fwrite(Buffer_Read,1,n,fp);
					memset(Buffer_Read, 0,n);
				}
				n = read(listen_file_descriptor, Buffer_Read, sizeof(Buffer_Read)-1);
				if(n < 0)
					break;
			}
			memset(Buffer_Write, 0,1024);

		}

		RegexFlag = 0;
		memset(Buffer_Read, 0, sizeof(Buffer_Read)); 
		memset(Buffer_Write, 0, sizeof(Buffer_Write));
		while((n = read(listen_file_descriptor,Buffer_Read,sizeof(Buffer_Read)))<=0);
	}
	close(listen_file_descriptor);
	wait(NULL);
}