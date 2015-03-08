void handle_FileDownload(char *request)
{
	char *request_data = NULL;
	char delim[] = " \n";
	request_data = strtok(request,delim);
	request_data = strtok(NULL,delim);
	if(request_data == NULL)
	{
		Error_code = 1;
		sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nFileDownload <file_name>\n");
		return;
	}
	strcpy(fileDownloadName,request_data);
	request_data = strtok(NULL,delim);
	if(request_data != NULL)
	{
		Error_code = 1;
		sprintf(Response_for_query,"ERROR: Wrong Format. The correct format is:\nFileDownload <file_name>\n");
		return;
	}
	FILE* fp;
	fp = fopen(fileDownloadName,"rb");
	if(fp == NULL)
	{

	}
}