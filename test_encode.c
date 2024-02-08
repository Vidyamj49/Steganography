#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    //1.call check_operation_type() 
          OperationType check= check_operation_type(argv); 
          if(check==e_encode)
	  {
              printf("you have selected encoding\n");
	      if (argc <= 5)
	      {
	         if (argc >= 4)
		 {
		    int valid=(read_and_validate_encode_args(argv, &encInfo)== e_success);
	            if(valid)
		    {
	                printf("read and validation is done\n");
			int encode=(do_encoding(&encInfo) == e_success);
			if(encode)
			{
			    printf("Encoding Completed\n");
			}
			else
			{
			    printf("Encoding Failed\n");
			}
		     }
		    
	            else
	                printf("read and validation is failed\n");
		 }
		 else
		     printf("Pass correct arg\n");
	      }
	      else
	         printf("Pass correct arg\n");
	  }
        
         else if (check == e_decode)
	 {
             printf("you have selected decoding\n");
	     int valid=(read_and_validate_decode_args(argv, &decInfo)== e_success);
             if(valid)
	     {
                 printf("INFO: Read and Validation is done.\n");
                 int decode=(do_decoding(&decInfo) == e_success);
		 if(decode)
		 {
		     printf("INFO:Decoding is completed\n");
		 }
		 else
		     printf("INFO:Decoding is failed\n");
	     }
             else
		 printf("ERROR: Read and Validation is failed.\n");


	 }
         else 
             printf("pass the correct tag.usage : encoding and decoding\n");            
}

OperationType check_operation_type(char * argv[])
{
      if(argv[1] != NULL)
      {
           if(strcmp(argv[1],"-e") == 0)//strcmp for comparison
                return e_encode;
           else if(strcmp(argv[1],"-d") == 0) // use strcmp
                return e_decode;
           else 
                return e_unsupported;
      }
      else 
           return e_unsupported;

}


