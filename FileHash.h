int handle_CheckAll()
{
	unsigned char c[MD5_DIGEST_LENGTH];
	DIR *dp;
	i = 0;
	int a;
	struct dirent *ep;
	dp = opendir ("./");
	struct stat fileStat;

	if (dp != NULL)
	{
		while (ep = readdir (dp))
		{
			if(stat(ep->d_name,&fileStat) < 0)
				return 1;
			else
			{
				char *filename=ep->d_name;
				hashdata[i].filename = ep->d_name;
				hashdata[i].modified_time = fileStat.st_mtime;
				FILE *inFile = fopen (filename, "r");
				MD5_CTX mdContext;
				int bytes;
				unsigned char data[1024];

				if (inFile == NULL) {
					Error_code = 1;
					sprintf (Response_for_query,"%s can't be opened.\n", filename);
					return 0;
				}

				MD5_Init (&mdContext);
				while ((bytes = fread (data, 1, 1024, inFile)) != 0)
					MD5_Update (&mdContext, data, bytes);
				MD5_Final (c,&mdContext);
				for(a = 0; a < MD5_DIGEST_LENGTH; a++)
					hashdata[i].md5[a] = c[a];
				fclose (inFile);
				i++;
			}
		}
	}
	else
	{
		Error_code = 1;
		sprintf(Response_for_query,"Couldn't open the directory");
	}
}

int handle_Verify(char *file)
{
	unsigned char c[MD5_DIGEST_LENGTH];
	DIR *dp;
	i = 0;
	int a;
	struct dirent *ep;
	dp = opendir ("./");
	struct stat fileStat;

	if (dp != NULL)
	{
		while (ep = readdir (dp))
		{
			if(stat(ep->d_name,&fileStat) < 0)
				return 1;
			else if(strcmp(file,ep->d_name) == 0)
			{
				char *filename=ep->d_name;
				hashdata[i].filename = ep->d_name;
				hashdata[i].modified_time = fileStat.st_mtime;
				FILE *inFile = fopen (filename, "r");
				MD5_CTX mdContext;
				int bytes;
				unsigned char data[1024];

				if (inFile == NULL) {
					Error_code = 1;
					sprintf(Response_for_query,"%s can't be opened.\n", filename);
					return 0;
				}

				MD5_Init (&mdContext);
				while ((bytes = fread (data, 1, 1024, inFile)) != 0)
					MD5_Update (&mdContext, data, bytes);
				MD5_Final (c,&mdContext);
				for(a = 0; a < MD5_DIGEST_LENGTH; a++)
					hashdata[i].md5[a] = c[a];
				fclose (inFile);
				i++;
			}
			else
				continue;
		}
	}
	else
	{
		Error_code = 1;
		sprintf(Response_for_query,"Couldn't open the directory");
	}
}

void handle_FileHash(char *request)
{
	char *request_data = NULL;
	char delim[] = " \n";
	request_data = strtok(request,delim);
	request_data = strtok(NULL,delim);
	if(request_data == NULL)
	{
		Error_code = 1;
		sprintf(Response_for_query,"ERROR: Wrong Format\n");
	}
	while(request_data != NULL)
	{
		if(strcmp(request_data,"--checkall") == 0)
		{
			request_data = strtok(NULL,delim);

			if(request_data != NULL)
			{
				Error_code = 1;
				sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nFileHash CheckAll\n");
				return;
			}
			else
			{
				handle_CheckAll();
			}

		}
		else if(strcmp(request_data,"--verify") == 0)
		{
			request_data = strtok(NULL,delim);
			if(request_data == NULL)
			{
				Error_code = 1;
				sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nFileHash Verify <filename>\n");
				return;
			}
			char *filename = request_data;
			request_data = strtok(NULL,delim);
			if(request_data != NULL)
			{
				Error_code = 1;
				sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nFileHash Verify <filename>\n");
				return;
			}
			else
				handle_Verify(filename);
		}
	}
}
