/*--------------------------------------------------------------------------------------------------------------------------------------
														STEGANOGRAPHY PROJECT
---------------------------------------------------------------------------------------------------------------------------------------*/
/* 
	Name : Vivek Bharat Patil
	Date : 18/09/2024
	Description : STEGANOGRAPHY PROJECT 
				  It is practice of concealing a file, message, image,or video within another file, message, image, or video.
*/
#include<stdio.h>
#include<string.h>
#include"encode.h"                         // It is header file which contains all the function prototypes which are used in encoding 
#include"decode.h"                        // It is header file which contains all the function prototypes which are used in decoding

int main(int argc,char *argv[])                          // CLA arguments
{
	FILE * file;
	if(argc>5)
	{
		fprintf(stderr,RED"ERROR: Too many Arguments\n"RESET);          // showing error if CLA are not proper
		return e_success;
	}
	if(argc == 1)
	{
		fprintf(stderr,RED"ERROR: %s [ Usage -e or -d ]\n"RESET,argv[0]); 
		return e_success;
	}
	
	if(check_operation_type(argv) == e_encode)
	{
		EncodeInfo encInfo;
		if(argc<3)
		{
			fprintf(stderr,RED"ERROR: %s %s <.bmp file> <.txt file> [output file]\n"RESET,argv[0],argv[1]);  
			return e_success;                         // checking the files are .bmp and .txt/.c/.sh or not
		}

		file=fopen(argv[2],"r");
		if(file==NULL)
		{
			printf(RED"ERROR: no such file in the directory\n"RESET);
			return e_success;
		}
		char *check=strchr(argv[2],'.');
		if(check==NULL || (strcmp(check,".bmp")!=0))
		{
			fprintf(stderr,RED"ERROR: %s <.bmp file> Required\n"RESET,argv[2]);
			return e_success;
		}
		else if(argc==4 || argc>4)
		{
			file=fopen(argv[3],"r");
			if(file==NULL)
			{
				printf(RED"ERROR: no such file in the directory\n"RESET);
				return e_success;
			}
			if(!((strstr(argv[3],".txt")) || (strstr(argv[3],".c")) || (strstr(argv[3],".sh")) || (strstr(argv[3],".jpeg"))))
			{
				fprintf(stderr,RED"ERROR: <.Extension (.txt/.c/.sh/.jpeg) file> Required\n"RESET);
				return e_success;
			}
		}
		else
		{
			fprintf(stderr,RED"ERROR: %s < Secret file > Required\n"RESET,argv[2]);
			return e_success;
		}

		if(argc==5)
		{
			char *ch=strstr(argv[4],".bmp");
			if(ch == NULL)
			{
				strcpy(encInfo.stego_image_fname,argv[4]);
				strcat(encInfo.stego_image_fname,".bmp");
			}
			else if(ch[4] == '\0')
			{
				strcpy(encInfo.stego_image_fname,argv[4]);
			}
			else
			{
				fprintf(stderr,RED"ERROR: %s <.bmp file> Required\n"RESET,argv[4]);
				return e_success;
			}
		}
		else
			strcpy(encInfo.stego_image_fname,"Output_image.bmp");
		
		encInfo.src_image_fname=argv[2];
		encInfo.secret_fname=argv[3];
		
		printf(GREEN"INFO : ## Encoding Started ##\n"RESET);
		if(do_encoding(&encInfo) == e_success)                                        // Encoding function is called
			printf(GREEN"INFO: ## Encoding Done Successfully ##\n"RESET);             
		else
			printf(RED"ERROR: ## Encoding Fail ##\n"RESET);
	}
	else if(check_operation_type(argv) == e_decode)                                        // checking the operations wheather they are done or not
	{
		if(argc>4)
		{
			fprintf(stderr,RED"ERROR: Too many Arguments\n"RESET);
			return e_success;
		}

		if(argc<=2)
		{
			fprintf(stderr,RED"ERROR: %s %s <.bmp file> [output file]\n"RESET,argv[0],argv[1]);
			return e_success;            // checking the file is .bmp
		}
		file=fopen(argv[2],"r");
		if(file==NULL)
		{
			printf(RED"ERROR: no such file in the directory\n"RESET);
			return e_success;
		}
		DecodeInfo decInfo;
		char *ch=strchr(argv[2],'.');
		if( ch==NULL || strcmp(ch,".bmp")!=0)
		{
			fprintf(stderr,RED"ERROR: <.bmp file> Required"RESET);
			return e_success;
		}
		if(argc==4)
		{
			if(strchr(argv[3],'.'))
			{
				fprintf(stderr,RED"ERROR: %s Just Provide the File Name < NO EXTNESION >\n"RESET,argv[3]);
				return e_success;
			}
			strcpy(decInfo.decode_secret_fname,argv[3]);	
		}
		else	
			strcpy(decInfo.decode_secret_fname,"Secret_Message");
			decInfo.decode_src_image_fname=argv[2];
			
			printf(GREEN"INFO : ## Decoding Started ##\n"RESET);
			if(do_decoding(&decInfo) == e_success)                                  // Decoding function is called
				printf(GREEN"INFO: ## Decoding Done Successfully ##\n"RESET);
    	    else
        	    printf(RED"ERROR: ## Decoding Failed ##\n"RESET);

	}
	else if(check_operation_type(argv) == e_unsupported)
	{
		fprintf(stderr,RED"ERROR: Usage -e or -d\n"RESET);                // If not given -e or -d then it will show error             
		return e_success;
	}
}

OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e")==0 || strcmp(argv[1],"-E")==0)
		return e_encode;
	else if(strcmp(argv[1],"-d")==0 || strcmp(argv[1],"-D")==0)      // checking the user has given correct argument or not
		return e_decode;
	else
		return e_unsupported;
}
