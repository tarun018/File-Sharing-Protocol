void handle_Index_Get(char *request)
{
	char *request_data = NULL;
	const char delim[] = " \n";
	struct tm tm;
	time_t start_time , end_time;
	int enter = 1;
	char *regexp;

	request_data = strtok(request,delim);
	request_data = strtok(NULL,delim);
	if(request_data == NULL)
	{
		Error_code = 1;
		sprintf(Response_for_query,"ERROR: Wrong Format.\n Correct Formats : (1)IndexGet<space>LongList\n(2)IndexGet<space>ShortList<space>StartTimeStamp<space>EndTimeStamp\n");
		return;
	}
	else
	{
		if(strcmp(request_data,"--longlist") == 0)
		{
			request_data = strtok(NULL,delim);

			if(request_data != NULL)
			{
				printf("Entered in Error_code\n");
				Error_code = 1;
				sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nIndexGet LongList\n");
				return;
			}
			else
			{
				handle_IndexGet_Long_List();
			}
		}
		else if(strcmp(request_data,"--shortlist") == 0)
		{
			request_data = strtok(NULL,delim);
			if(request_data == NULL)
			{
				Error_code = 1;
				sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nIndexGet ShortList <timestamp1> <timestamp2>\n");
				return;
			}
			while(request_data != NULL)
			{
				if(enter >= 3)
				{
					Error_code = 1;
					sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nIndexGet ShortList <timestamp1> <timestamp2>\n");
					return;
				}
				if (strptime(request_data, "%d-%b-%Y-%H:%M:%S", &tm) == NULL)
				{
					Error_code = 1;
					sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nDate-Month-Year-hrs:min:sec\n");
					return;
				}
				if(enter == 1)
					start_time = mktime(&tm);
				else
					end_time = mktime(&tm);
				enter++;
				request_data = strtok(NULL,delim);
			}
			handle_IndexGet_Short_List(start_time,end_time);
		}
		else if(strcmp(request_data,"--regex") == 0)
		{
			request_data = strtok(NULL,delim);
			if(request_data == NULL)
			{
				printf("ERROR: Wrong Format. The correct format is:\nIndexGet RegEx <regular expression>\n");
				_exit(1);
			}
			regexp = request_data;
			request_data = strtok(NULL,delim);
			if(request_data != NULL)
			{
				printf("ERROR: Wrong Format. The correct format is:\nIndexGet RegEx <regular expression>\n");
				_exit(1);
			}
			handleRegEx(regexp);
		}
		else
		{
			Error_code = 1;
			sprintf(Response_for_query,"ERROR: Wrong Format.\n");
			return;
		}
	}
}

int handle_IndexGet_Long_List()
{
	DIR *dp;
	i = 0; 
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
				strcpy(printdata[i].filename, ep->d_name);
				printdata[i].size = fileStat.st_size;
				printdata[i].modified_time = fileStat.st_mtime;
				printdata[i].type = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
				i++;
			}
		}
		closedir (dp);
	}
	else
	{
		printf("Couldn't open the directory");
	}
}

int handle_IndexGet_Short_List(time_t start_time,time_t end_time)
{
	DIR *dp;
	i = 0;
	struct dirent *ep;
	dp = opendir ("./");
	struct stat fileStat;

	if (dp != NULL)
	{
		while (ep = readdir (dp))
		{
			if(stat(ep->d_name,&fileStat) < 0)
				return 1;
			else if(difftime(fileStat.st_mtime,start_time) > 0 && difftime(end_time,fileStat.st_mtime) > 0)
			{
				strcpy(printdata[i].filename , ep->d_name);
				printdata[i].size = fileStat.st_size;
				printdata[i].modified_time = fileStat.st_mtime;
				printdata[i].type = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
				i++;
			}
		}
		closedir (dp);
	}
	else
	{
		Error_code = 1;
		sprintf(Response_for_query,"Couldn't open the directory");
	}
}

int handleRegEx(char *regexp)
{
	FILE *pipein_fp;
	regex_t regex;
	if(regexp != NULL){
        int reti = regcomp(&regex, regexp, 0);
        if(reti){
            perror("Invalid regular expression.\n");
        }
    }

	char string[1024] = "ls ";
	char str[1024];
	memset(str, 0,sizeof(str));
	char line[1024];
	char readbuf[1024];
	i = 0;
	RegexFlag = 1;

	strncpy(str,regexp+1,strlen(regexp)-2);
	strcat(string,str);

	DIR *dp;
	int a;
	struct dirent *ep;
	dp = opendir ("./");
	struct stat fileStat;

	if (dp != NULL)
	{
		while (ep = readdir (dp))
		{

			if(regexec(&regex, ep->d_name, 0, NULL, 0))
				continue;
			if(stat(ep->d_name,&fileStat) < 0)
				return 1;
			else
			{	
				if (( pipein_fp = popen(string, "r")) == NULL)
				{
					perror("popen");
					exit(1);
				}
				while(fgets(readbuf, 1024, pipein_fp))
				{
					strncpy(line,readbuf,strlen(readbuf)-1);
					if(strcmp(line,ep->d_name) == 0)
					{
						strcpy(printdata[i].filename , ep->d_name);
						printdata[i].size = fileStat.st_size;
						printdata[i].type = (S_ISDIR(fileStat.st_mode)) ? 'd' : '-';
						i++;
						break;
					}
					memset(line, 0,sizeof(line));
				}
			}
		}

		pclose(pipein_fp);
	}
	else
	{
		Error_code = 1;
		sprintf(Response_for_query,"Couldn't open the directory");
	}
}
