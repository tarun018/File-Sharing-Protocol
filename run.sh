#!/bin/bash

cc -w -o filetransferpro file_sharing_protocol_201403002.c -lssl -lcrypto
echo "Done."