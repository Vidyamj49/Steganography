#include<stdio.h>
#include<string.h>
#include "decode.h"
#include "common.h"
#include "types.h"

/*Function definition */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
     if(!strcmp((strstr(argv[2],".")),".bmp"))
    {
        decInfo->src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
   
    if(argv[3])
    {
	strncpy(decInfo->extn_secret_file, strstr(argv[3], "."), 4);

     
         if ((strncmp(decInfo->extn_secret_file, ".txt", 4) == 0) || (strncmp(decInfo->extn_secret_file, ".c", 2) == 0) || (strncmp(decInfo->extn_secret_file, ".sh", 3) == 0))
        {
				decInfo->secret_fname = argv[3];
        }
        else
        {
      		fprintf(stderr,"Error: Output file %s format should be .txt or .c or .sh\n", argv[3]);
            return e_failure;
        }
    }
    else
    {
		decInfo->secret_fname = "decode.txt";
	       strcpy(decInfo->extn_secret_file,".txt");
    }
	
   
    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_f(decInfo)==e_success)
    {
        printf("INFO: Files are opened successfully\n");
    }
    else
    {
        printf("ERROR:Open files id failed\n");
        return e_failure;
    }
    if(decode_magic_string(MAGIC_STRING,decInfo)==e_success)
    {
	printf("INFO: Magic string is decoded\n");
    }
    else
    {
	printf("ERROR: Magic string is not decoded\n");
	return e_failure;
    }
    if(decode_extn_size(decInfo)== e_success)
    {
	printf("INFO:Extension size decoded successfully.\n");
    }
    else
    {
	printf("ERROR:Extension size decoding failed.\n");
	return e_failure;
    }
    if(decode_secret_file_extn(decInfo) == e_success)
    {

	{
	    strcat(decInfo->secret_fname,decInfo->extn_secret_file);
	}
	printf("INFO: Extension decoded successfully.\n");
	// Secret file
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
	// Do Error handling
	if (decInfo->fptr_secret == NULL)
	{
	    perror("fopen");
	    fprintf(stderr, "ERROR: Unable to open Output file %s\n", decInfo->secret_fname);

  
	    return e_failure;
	}
	printf("INFO : Output file opened successfully.\n");
    }
    else
    {
	return e_failure;
    }

    if(decode_secret_file_size(decInfo)== e_success)
    {	    
        printf("INFO: File size decoded successfully.\n");	
    }
    else
    {
	printf("ERROR: File size decoding is failed.\n");
	return e_failure;
    }
    if(decode_secret_file_data(decInfo) == e_success)
    {
	printf("INFO: Secrete file data decoded successfully.\n");
    }
    else
    {
	printf("ERROR: Secrete file data decoding is failed.\n");
        return e_failure;
    }

    return e_success;

}

Status open_f(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image  = fopen(decInfo->src_image_fname, "r");
    if(decInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->src_image_fname );
        return e_failure;
    }

    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname,"w");

    /*if(decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",decInfo->stego_image_fname );
        return e_failure;
    }*/

    return e_success;
}

Status decode_magic_string(char *magic_string,DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_src_image,54, 0);
    char buffer[2]={0};
    for(int i=0;i<2;i++)
    {
	decode_data_from_image(buffer,8,i,decInfo->fptr_src_image);
    }
    if(!strcmp(magic_string,buffer))
    {
	return e_success;
    }
    else
    {
	return e_failure;
    }
}
//---------------------------------------------------------
Status decode_data_from_image(char *data,int size,int j,FILE *fptr_src_image)
{
    char ch;
    for(int i=0;i<size;i++)
    {
	fread(&ch,1,1,fptr_src_image);
	ch= ch & 1;
	data[j]=( data[j] | (ch << (i) ) );
 
    }
    return e_success;
}

//-------------------------------------------------------
Status decode_extn_size(DecodeInfo *decInfo)
{
    char size_buffer[32],ch;
    int size=0;
    fread(size_buffer,1,32,decInfo->fptr_src_image);
    for(int i=0;i<32;i++)
    {
	size = ( size   ) | (( size_buffer[i] & 1 )<<i);
    }
    decInfo->extn_size=size;
    return e_success;
}
//---------------------------------------------------------------------------------------------------------------------------------------------
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    int i;

    for(int i =0; i< decInfo -> extn_size; i++)
    {
	decInfo -> extn_secret_file[i] = 0;
    }
    for( i=0;i<decInfo->extn_size;i++)
    {
	decode_data_from_image(decInfo->extn_secret_file,8,i,decInfo->fptr_src_image);
    }
    decInfo->extn_secret_file[i]='\0';
    return e_success;
}
//-------------------------------------------------------------------------------------------------------------------------------

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char filesize_buffer[32];
    int size=0;
    fread(filesize_buffer,1,32,decInfo->fptr_src_image);
    for(int i=0; i<32;i++)
    {
	size= (size) | ((filesize_buffer[i] &1)<<i);

    }
    decInfo->size_secret_file=size;
    return e_success;
}
//-------------------------------------------------------------------------------------------------------------------------------

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char  data_buffer[8],ch=0;
    fseek(decInfo->fptr_secret,0,SEEK_SET);
    for(int i=0;i<decInfo->size_secret_file;i++)
    {
	fread(data_buffer,8, 1,decInfo->fptr_src_image);
	ch =0;
	for(int j=0;j<8;j++)
	{
	    ch= ch | ((data_buffer[j] & 1 )<<j);
	}
	fwrite(&ch,1,1,decInfo->fptr_secret);
    }
    return e_success;
}
