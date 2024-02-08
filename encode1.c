#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "common.h"
#include "types.h"

/* Function Definitions */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(!strcmp((strstr(argv[2],".")),".bmp"))
    {
        encInfo->src_image_fname=argv[2];
    }
    else
    {
        return e_failure;
    }
    if((strstr(argv[3],".txt")!=NULL))
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,".txt");
    }
    else if((strstr(argv[3],".sh")!=NULL))
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,".sh");
    }
    else if((strstr(argv[3],".c")!=NULL))
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,".c");
    }
    else if((strstr(argv[3],".pdf")!=NULL))
    {
        encInfo->secret_fname=argv[3];
        strcpy(encInfo->extn_secret_file,".pdf");
    }
    else
    {
        return e_failure;
    }
    if(argv[4])
    {
        encInfo->stego_image_fname=argv[4];
        printf("INFO :Output  file is mentioned, It is added\n");
    }
    else
    {
        encInfo->stego_image_fname="encode.bmp";
        printf("INFO : Output file not mentioned,Default file name is added\n");
    }
    return e_success;

}



Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)== e_success)
    {
        printf("INFO: Files are opened successfully\n");
    }
    else
    {
        printf("ERROR: Open files Failed\n");
        return e_failure;
    }
    if(check_capacity(encInfo) == e_success)
    {
         printf("INFO: Capacity is done\n");
    }
    else
    {
          printf("ERROR: Capacity is failed\n");
          return e_failure;
    }
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)
    {
           printf("INFO: Header of bmp copied to stego successfully\n");
    }
    else
    {
           printf("ERROR: Header copying is failed\n");

           return e_failure;
    }
    if(encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
            printf("INFO: Magic string encoded successfully\n");
    }
    else
    {
            printf("ERROR: Magic string is not encoded successfully\n");
            return e_failure;
    }
    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo) == e_success)
    {
             printf("INFO: Secrete file size extn encoded successfully\n");
    }
    else
    {
        printf("ERROR: Secrete file size extn is not encoded successfully\n");
        return e_failure;
    }
    if(encode_secret_file_extn(encInfo-> extn_secret_file,encInfo) == e_success)
    {
        printf("INFO: Secrete file extension encoded successfully\n");
    }
    else
    {
       printf("ERROR: Secrete file extension is not encoded successfully\n");
       return e_failure;
    }
    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("INFO : Encoded Successful in secret file size. \n");
    }
    else
    {
        printf("ERROR : Failed in encoding secret file size. \n");
        return e_failure;
    }
    if(encode_secret_file_data(encInfo)==e_success)
    {
        printf("INFO : Encoded Successful in encoding secret data.\n");
    }
    else
    {
        printf("ERROR : Failed in encoding secret data.\n");
        return e_failure;
    }
    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("INFO : Encoded Successful in copying remaining data.\n");
    }
    else
    {
        printf("ERROR : Failed  in  copying remaining data.\n");
        return e_failure;
    }

    return e_success;
}


Status check_capacity(EncodeInfo *encInfo)
{

    encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);
    fseek(encInfo->fptr_secret,0,SEEK_END);
    encInfo->size_secret_file=ftell(encInfo->fptr_secret);
    int calculate = 54+(strlen(MAGIC_STRING)*8) + 32 +32 + 32+ (sizeof(encInfo->size_secret_file)*8);
    if(encInfo->image_capacity>calculate)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    rewind(fptr_src_image);
    char buffer[55];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_stego_image);
    return e_success;

}
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        return e_success;
    }
}

Status encode_data_to_image(char *data,int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(image_buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],image_buffer);
        fwrite(image_buffer,8,1,fptr_stego_image);
    }
    return e_success;
}
Status encode_byte_to_lsb(char ch,char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        char get=ch&(1<<i);
        get=((unsigned)get>>i);
        image_buffer[i]=(image_buffer[i]&(~(1)))|get;

    }
    return e_success;
}

Status encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
    //32 bytes from source image
    char ch;
    char data_buffer[1];
    for(int i=0;i<32;i++)
    {
        ch = ((size & (1 << i)) >> i);
        fread(data_buffer,1,1,encInfo->fptr_src_image);
        data_buffer[0] = (data_buffer[0] & (~(1)) | ch);
        fwrite(data_buffer,1,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_secret_file_extn(char *secret_file_extn,EncodeInfo  *encInfo)
{
    encode_data_to_image(secret_file_extn,strlen(secret_file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}
Status  encode_secret_file_size(long size,EncodeInfo    *encInfo)
{
    encode_secret_file_extn_size(size,encInfo);
    return e_success;
}


///
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char secret_buffer[encInfo->size_secret_file];
    fread(secret_buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    encode_data_to_image(secret_buffer,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char ch;
    while((fread(&ch,1,1,fptr_src_image))>0)
    {
        fwrite(&ch,1,1,fptr_stego_image);
    }
    return e_success;
}

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

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

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
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
