#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include"common.h"
#include "decode.h"

Status do_decoding(DecodeInfo *decodeInfo)
{	
	if(decode_open_files(decodeInfo) == e_success)         // opening the files
		printf("INFO: Open File Successfully\n");
	else
	{
		printf(RED"INFO: Open File Failure\n"RESET);
		return e_failure;
	}

    fseek(decodeInfo -> decode_fptr_src_image,54,SEEK_SET);   // Skipping the bmp Header files

	if(decode_magic_string(decodeInfo) == e_success)               // decoding magic string
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Decoding Magic String Failure\n"RESET);
		return e_failure;
	}

	if(decode_secret_file_extn(decodeInfo) == e_success)            // decoding sceret file extn size and data
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Decoding Secret File Extn Failure\n"RESET);
		return e_failure;
	}
 
	if(!(decode_secret_file_size(decodeInfo) == e_success))         // decoding sceret file size
    {
		printf(RED"INFO: Decoding Secret File Size Failure\n"RESET);
		return e_failure;
	}

	printf(BLUE"INFO: Output FIle is %s\n"RESET,decodeInfo -> decode_secret_fname);
	
	fclose(decodeInfo -> decode_fptr_src_image);                     //Closing the files
	fclose(decodeInfo -> decode_fptr_secret);
	return e_success;
	
}
Status decode_open_files(DecodeInfo *decodeInfo)
{ 
	decodeInfo -> decode_fptr_src_image= fopen(decodeInfo -> decode_src_image_fname,"r");
	if(decodeInfo -> decode_fptr_src_image == NULL)
	{	
		printf(RED"INFO: The File is not Opened\n"RESET);
		return e_failure;
	}
	return e_success;
}

Status decode_magic_string(DecodeInfo *decodeInfo)
{
	printf("INFO: Decoding Magic String Signature\n");
	char *buff = malloc(32);
	fread(buff,32,1,decodeInfo -> decode_fptr_src_image);
    int len= decode_int_to_msb(buff);                              // decoding the magic string length from the Output File
	buff = realloc(buff , 8);
	char *str=malloc(len);
	for(int i=0 ; i<len; i++)
	{
		fread(buff,8,1,decodeInfo -> decode_fptr_src_image);
		str[i]=decode_byte_to_msb(buff);                            // decoding the magic string from the Output File
	}
    char pass[20];
    printf(BLUE"Enter the Magic String Passkey : "RESET);         // checkin the magic string encoded and the enter magic string is equal or not 
    scanf("%[^\n]",pass);
    if(strcmp(str,pass)==0)
    {
        free(buff);
		printf("INFO: Magic String Matched....!\n");
        return e_success;
    }
    else
    {
		free(buff);
		printf("INFO: Magic String Not Matched....!\n");
	    return e_failure;
    }
}

Status decode_int_to_msb(char buff[])
{
    int data=0;
	for(int i=0;i<32; i++)
	{
        if(buff[i] & 1)
            data=(data<<1) | 1;                         // Decoding the interger (4 byte) data in the Output File
        else
            data=data<<1;
	}
    return data;
}

Status decode_byte_to_msb(char buff[])
{	
	char data;
	for(int i=0;i<8; i++)
	{
	    if(buff[i] & 1)
            data=(data<<1) | 1;                    // Decoding the char (1 byte) data in the Output File
        else
            data=data<<1;
	}
	return data;
}

Status decode_secret_file_extn(DecodeInfo *decodeInfo)
{
	printf("INFO: Decoding Output File Extension\n");
	char *buff = malloc(32);
	fread(buff,32,1,decodeInfo -> decode_fptr_src_image);
    int len= decode_int_to_msb(buff);                          // decoding the Secret File Extension length from the Output File
	buff = realloc(buff , 8);
	char *str=malloc(len);
	for(int i=0 ; i<len; i++)
	{
		fread(buff,8,1,decodeInfo -> decode_fptr_src_image);
		str[i]=decode_byte_to_msb(buff);                    // decoding the Secret File Extension length from the Output File
	}

	free(buff);
    strcat(decodeInfo -> decode_secret_fname,str);	       // merging the extension with the output secret file
	return e_success;
}

Status decode_secret_file_size(DecodeInfo *decodeInfo)
{	
	printf("INFO: Decoding File Size\n");
	char *buff = malloc(32);
	fread(buff,32,1,decodeInfo -> decode_fptr_src_image);
    int len= decode_int_to_msb(buff);                              // decoding the Secret File Size from the Output File

	printf("INFO: Done\n");
    
	if(decode_secret_file_data(len,decodeInfo,buff) == e_success)               // decoding sceret file data 
    	printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Decoding Secret File Data Failure\n"RESET);
		return e_failure;
	}
	free(buff);
    return e_success;
}

Status decode_secret_file_data(int len,DecodeInfo *decodeInfo,char buff[])
{
	printf("INFO: Decoding File Data\n");
	char *str=malloc(len);
    buff = realloc(buff , 8);
	for(int i=0 ; i<len; i++)
	{
		fread(buff,8,1,decodeInfo -> decode_fptr_src_image);
		str[i]=decode_byte_to_msb(buff);                            // decoding the Secret File Data from the Output File
	}

    decodeInfo -> decode_fptr_secret = fopen(decodeInfo -> decode_secret_fname,"w");
    int i=0;
    char data; 
    while(i<len)
        fputc(str[i++],decodeInfo -> decode_fptr_secret);
    return e_success;
}

