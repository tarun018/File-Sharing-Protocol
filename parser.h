int parse_request(char *request)
{
		const char delim[] = " \n";

	char copying_request[50];
	strcpy(copying_request, request);

	  char *request_data = NULL;
	request_data = strtok(copying_request, delim);

	if(request_data != NULL) {   
	   if(strcmp(request_data,"IndexGet") == 0) 				return INDEXGET;
		 else if(strcmp(request_data, "FileHash") == 0) {		return FILEHASH;}
	   else if(strcmp(request_data, "FileDownload") == 0)		return FILEDOWNLOAD;
		 else if(strcmp(request_data, "FileUpload") == 0)		{return FILEUPLOAD;}
	   else if(strcmp(request_data, "Quit") == 0 || strcmp(request_data, "quit") == 0)
		{
		  return QUIT;
		}
		else if(strcmp(request_data, "Q") == 0  || strcmp(request_data, "q") == 0)	{
		  return QUIT;
		}
		else return ERROR;
	}
}