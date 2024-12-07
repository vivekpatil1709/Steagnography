#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include"common.h"                           // In this header file our password is there which used while decoding
#include "encode.h"


/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)                             // this function will give the height and width of the image
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status do_encoding(EncodeInfo *encInfo)
{	
	if(open_files(encInfo) == e_success)                                 // opening the files
	{
		printf("INFO: Open File Successfully\n");
		printf("INFO: Done\n"RESET);
	}
	else
	{
		printf(RED"INFO: Open File Failure\n"RESET);
		return e_failure;
	}
    
	if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)   // coping the bmp header files
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Copied BMP Header File Failure\n"RESET);
		return e_failure;
	}

	if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)              // encoding magic string
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Encoding Magic String Failure\n"RESET);
		return e_failure;
	}

	char *ptr = strchr(encInfo -> secret_fname,'.');                      // encoding sceret file size and data
	if(encode_secret_file_extn( ptr, encInfo) == e_success)
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Encoding Secret Data Failure\n"RESET);
		return e_failure;
	}

	if(copy_remaining_img_data(encInfo -> fptr_src_image , encInfo -> fptr_stego_image) == e_success) //copying remaining data
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Copying Left Over Data Failure\n"RESET);
		return e_failure;
	}

	printf(BLUE"INFO: Output FIle is %s\n"RESET,encInfo -> stego_image_fname);       // showing the output file name
	fclose(encInfo -> fptr_src_image);
	fclose(encInfo -> fptr_secret);                                  // Closing of all the files 
	fclose(encInfo -> fptr_stego_image);
	return e_success;
	
}
Status open_files(EncodeInfo *encInfo)
{ 
	
	encInfo -> fptr_src_image= fopen(encInfo -> src_image_fname,"r");
	if(encInfo -> fptr_src_image == NULL)
	{	
		printf(RED"INFO: The File is not Opened\n"RESET);            
		return e_failure;
	}
	
	encInfo -> fptr_secret= fopen(encInfo -> secret_fname,"r");
	if(fgetc(encInfo -> fptr_secret) == EOF)
	{
		printf(RED"%s is Empty\n"RESET,encInfo -> secret_fname);
		return e_failure;
	}
	else
	{
		uint length=get_image_size_for_bmp(encInfo -> fptr_src_image);
		fseek(encInfo -> fptr_secret,0,SEEK_END);
		int len=ftell(encInfo -> fptr_secret);           // checking the main file space if its is small than secret file the error occur 
		if(length<((4+strlen(MAGIC_STRING)+4+4+4+len)*8))
		{
			printf(RED"INFO: The Secret File Size is greater than %s \n"RESET,encInfo -> src_image_fname);
			return e_failure;
		}
	}

	if(encInfo -> fptr_secret == NULL)
	{	
		printf(RED"INFO: The File is not Opened\n"RESET);
		return e_failure;
	}

	rewind(encInfo -> fptr_src_image);
	rewind(encInfo -> fptr_secret);                        // It is  used  to move the cursor to the first position of the file 

	encInfo -> fptr_stego_image= fopen(encInfo -> stego_image_fname,"w");
	if(encInfo -> fptr_stego_image == NULL)
	{	
		printf(RED"INFO: The File is not Opened\n"RED);
		return e_failure;
	}
	return e_success;
}

Status copy_bmp_header(FILE *fptr_src, FILE *fptr_dest)
{
	printf("INFO: Copying Image Header\n");
	char *buff = malloc(54);
	fread(buff,54,1,fptr_src);                      // copying all the header files as it is
	fwrite(buff,54,1,fptr_dest);
	free(buff);
	return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	printf("INFO: Encoding Magic String Signature\n");
	int l = strlen(magic_string);
	char *buff = malloc(32);        
	fread(buff,32,1,encInfo -> fptr_src_image);
	encode_int_to_lsb(l,buff);                                        // encoding the magic string length in the RGB data
	fwrite(buff,32,1,encInfo -> fptr_stego_image);
	buff = realloc(buff , 8);
	for(int i=0 ; i<l ; i++)
	{
		fread(buff,8,1,encInfo -> fptr_src_image);
		encode_byte_to_lsb(magic_string[i],buff);                      // encoding the magic string data in the RGB data
		fwrite(buff,8,1,encInfo -> fptr_stego_image);
	}
	free(buff);
	return e_success;
}

Status encode_int_to_lsb(int size,char buff[])
{
	int index=0;
	for(int i=31;i>=0; i--)
	{
		if(size & (1<< i))
			buff[index] = (buff[index] | 1);                          // Encoding the interger (4 byte) data in the RBG Data
		else
			buff[index] = (buff[index] & ~(1));
		index++;
	}
}

Status encode_byte_to_lsb(char ch, char buff[])
{	
	int index=0;
	for(int i=7;i>=0; i--)
	{
		if(ch & (1<< i))
			buff[index] = (buff[index] | 1);             // Encoding the char (1 byte) data in the RBG Data
		else
			buff[index] = (buff[index] & ~(1));
		index++;
	}
}

Status encode_secret_file_extn( const char *str, EncodeInfo *encInfo)
{
	printf("INFO: Encoding secret.txt File Extension\n");
	int len = strlen(str);
	char *buff = malloc(32*sizeof(char));
	fread(buff,32,1,encInfo -> fptr_src_image);
	encode_int_to_lsb(len,buff);                                    // encoding the Extension length in the RGB data
	fwrite(buff,32,1,encInfo -> fptr_stego_image);
	buff = realloc(buff , 8);
	for(int i=0 ; i<len ; i++)
	{
		fread(buff,8,1,encInfo -> fptr_src_image);
		encode_byte_to_lsb(str[i],buff);                         // encoding the Extension in the RGB data
		fwrite(buff,8,1,encInfo -> fptr_stego_image);
	}
	printf("INFO: Done\n");
	free(buff);
	
	fseek(encInfo -> fptr_secret,0,SEEK_END);
	
	if(encode_secret_file_size(ftell(encInfo -> fptr_secret), encInfo) == e_success)  
		printf("INFO: Done\n");
	else
	{
		printf(RED"INFO: Encoding Secret File Size Failure\n"RESET);
		return e_failure;
	}

	if(!(encode_secret_file_data(encInfo) == e_success))            
	{
		printf(RED"INFO: Encoding Secret File Data Failure\n"RESET);
		return e_failure;
	}
	return e_success;
}

Status encode_secret_file_size(int size,EncodeInfo *encInfo)
{	
	printf("INFO: Encoding secret.txt File Size\n");
	rewind(encInfo ->fptr_secret);
	char *buff = malloc(32);
	fread(buff,32,1,encInfo -> fptr_src_image);
	encode_int_to_lsb(size,buff);                             // encoding the Secret File Size in the RGB data
	fwrite(buff,32,1,encInfo -> fptr_stego_image);
	free(buff);
	return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	printf("INFO: Encoding secret.txt File Data\n");
	rewind(encInfo ->fptr_secret);
	char ch;
	char *buff=malloc(8);
	while((ch = fgetc(encInfo -> fptr_secret)) != EOF)
	{
		fread(buff,8,1,encInfo -> fptr_src_image);
		encode_byte_to_lsb(ch,buff);                       // encoding the Secret File Data in the RGB data
		fwrite(buff,8,1,encInfo -> fptr_stego_image);
	}
	free(buff);
	return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
	printf("INFO: Copying Left Over Data\n");
	char *buff;
	while(fread(&buff,1,1,fptr_src_image) > 0)                    // copying all the remaining data in the main file to the Output file  
	{
		fwrite(&buff,1,1,fptr_stego_image);
	}
	return e_success;
}
