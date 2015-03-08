int tcp_server(char *listenportno)
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr; 
    int portno = atoi(listenportno);

    char readBuff[1024];
    char writeBuff[1024];
    time_t ticks; 

    listenfd = GETSOCKET(AF_INET, SOCK_STREAM, 0);
    if(listenfd == -1)		{
	    perror("Unable to create socket");
	    exit(0);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    memset(readBuff, 0, sizeof(readBuff)); 
    memset(writeBuff, 0, sizeof(writeBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno); 

    if(BINDTOPORT(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)	{
	    perror("Unable to bind");
	    exit(0);
    }

    listen(listenfd, 10); 
    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 

    int a , n , b , c;
    n = recv(connfd,readBuff,sizeof(readBuff), 0);


    while( n > 0)
    {
        size_t size = strlen(readBuff) + 1;
        char *request = malloc(size);
        strcpy(request,readBuff);

        strcpy(his[hist_count_variable_index],readBuff);
        hist_count_variable_index++;

        int request_type = parse_request(request);
		printf("\nCommand Received : %s",request);
		Response_for_query[0] = '\0';
        writeBuff[0] = '\0';
		printf("[Enter Command Here : ] ");
		fflush(stdout);
        if(request_type == ERROR)      //Error_code
        {
            Error_code = 1;
            sprintf(Response_for_query,"ERROR: No request of this type.\n");
        }
        else if(request_type == INDEXGET){    //Indexget
            handle_Index_Get(request);}
        else if(request_type == FILEHASH){      //FileHash
            handle_FileHash(request);}
        else if(request_type == FILEDOWNLOAD){      //FileDownload
            handle_FileDownload(request);}

        if(Error_code == 1)
        {
            strcat(writeBuff,Response_for_query);
            strcat(writeBuff,"~@~");
            send(connfd , writeBuff , strlen(writeBuff), 0);
            Error_code = -1;
            memset(writeBuff, 0, sizeof(writeBuff));
            memset(readBuff, 0, sizeof(readBuff)); 
            while((n = recv(connfd,readBuff,sizeof(readBuff), 0))<=0);
            continue;
        }
        if(request_type == INDEXGET && RegexFlag == 1)	{
            strcat(writeBuff,Response_for_query);

            a = 0;
            while(a < i)
            {
                sprintf(Response_for_query, "%-35s| %-10d| %-3c| %-20s" , printdata[a].filename , printdata[a].size , printdata[a].type , ctime(&printdata[a].modified_time));
                strcat(writeBuff,Response_for_query);
            	a++;
            }
            strcat(writeBuff,"~@~");

            send(connfd , writeBuff , strlen(writeBuff), 0);
        }
        else if(request_type == INDEXGET)
        {
            strcat(writeBuff,Response_for_query);

            a = 0;
            while(a < i)
            {
                sprintf(Response_for_query, "%-35s| %-10d| %-3c| %-20s" , printdata[a].filename , printdata[a].size , printdata[a].type , ctime(&printdata[a].modified_time));
                strcat(writeBuff,Response_for_query);
                a++;
            }
            strcat(writeBuff,"~@~");

            send(connfd , writeBuff , strlen(writeBuff), 0);
        }
        else if(request_type == FILEHASH)
        {
            strcat(writeBuff,Response_for_query);

            b = 0;
            while(b < i)
            {
                sprintf(Response_for_query, "%-35s |   ",hashdata[b].filename);
                strcat(writeBuff,Response_for_query);
                c = 0;
                while(c < MD5_DIGEST_LENGTH)
                {
                    sprintf(Response_for_query, "%x",hashdata[b].md5[c]);
                    strcat(writeBuff,Response_for_query);
                    c++;
                }
                sprintf(Response_for_query, "\t %20s",ctime(&hashdata[b].modified_time));
                strcat(writeBuff,Response_for_query);
                b++;
            }
            strcat(writeBuff,"~@~");

            send(connfd , writeBuff , strlen(writeBuff), 0);
        }
        else if(request_type == FILEDOWNLOAD)
        {
            FILE* fp;
            fp = fopen(fileDownloadName,"rb");
            size_t bytes_read;
            while(!feof(fp))
            {
                bytes_read = fread(Response_for_query, 1, 1024, fp);
                memcpy(writeBuff,Response_for_query,bytes_read);
                send(connfd , writeBuff , bytes_read, 0);
                memset(writeBuff, 0, sizeof(writeBuff));
                memset(Response_for_query, 0, sizeof(Response_for_query));
            }
            memcpy(writeBuff,"~@~",3);
            send(connfd , writeBuff , 3, 0);
            memset(writeBuff, 0, sizeof(writeBuff));
            fclose(fp);
        }
        else if(request_type == FILEUPLOAD)
        {
            printf("FileUpload Accepted\n");

            memcpy(writeBuff,"FileUpload Accept\n",18);
            send(connfd , writeBuff , 18, 0);
            memset(writeBuff, 0,18);
            char copyrequest[1024];
            memset(copyrequest, 0,1024);
            memcpy(copyrequest,request,1024);
            char *size = strtok(copyrequest,"\n");
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
            result = send(f, "", 1, 0);
            if (result < 0) {
                close(f);
                perror("Error opening file for writing:");
                return 1;
            }
            close(f);
            FILE *fp;
            fp = fopen(request_data,"wb");
            n = recv(connfd, readBuff, sizeof(readBuff)-1, 0);
            for(;;)
            {
                readBuff[n] = 0;
                if(readBuff[n-1] == '~' && readBuff[n-3] == '~' && readBuff[n-2] == '@')
                {
                    readBuff[n-3] = 0;
                    fwrite(readBuff,1,n-3,fp);
                    fclose(fp);
                    memset(readBuff, 0,n-3);
                    break;
                }
                else
                {
                    fwrite(readBuff,1,n,fp);
                    memset(readBuff, 0,n);
                }
                n = recv(connfd, readBuff, sizeof(readBuff)-1, 0);
                if(n < 0)
                    break;
            }
            memset(writeBuff, 0,1024);

        }

        RegexFlag = 0;
        memset(readBuff, 0, sizeof(readBuff)); 
        memset(writeBuff, 0, sizeof(writeBuff));
        while((n = recv(connfd,readBuff,sizeof(readBuff), 0))<=0);
    }
    close(connfd);
    wait(NULL);
}