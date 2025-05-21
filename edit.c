#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "edit.h"
#include "types.h"

//read and validate function
Status read_and_validate_edit_args(char *argv[] ,Edit_Info *editInfo)
{
	//check for source file extn is of ".mp3"
	if((strstr(argv[4] ,".mp3") != NULL) && (strcmp(strstr(argv[4] ,".mp3"), ".mp3") == 0))                       
	{
		editInfo->edit_mp3_fname = argv[4];          //store source file name    
		editInfo->duplicate_mp3_fname = "temp.mp3";  //store temp.mp3 string to an array
		strcpy(editInfo->new_data , argv[3]);        //copy new data which has to be edited
		editInfo->new_size = strlen(argv[3])+1;      //store new data size including null character

		if(open_src_edit_file(editInfo) == e_success)    //open files function call
		{
			printf("file opened successfully\n");
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	return e_success;
}

/* open source file */
Status open_src_edit_file(Edit_Info *editInfo)
{
	editInfo->fptr_edit_mp3 = fopen(editInfo->edit_mp3_fname ,"r");
	editInfo->fptr_duplicate_file = fopen(editInfo->duplicate_mp3_fname ,"w");

	if(editInfo->fptr_edit_mp3 == NULL && editInfo->fptr_duplicate_file == NULL )
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n",editInfo->edit_mp3_fname);

		return e_failure;
	}
	return e_success;
}

/*edit mp3 file info*/
Status edit_mp3_file_info(Edit_Info *editInfo ,char *argv[])
{
	char *buf;
	uint old_size;

	fread(buf,3,1,editInfo->fptr_edit_mp3); 
	if(strcmp(buf,"ID3") != 0)               //check for the TAG
	{
		printf("tag is not ID3\n");
		return e_failure;
	}

	rewind(editInfo->fptr_edit_mp3);       
	//skip 10 bytes header
	fread(buf,10,1,editInfo->fptr_edit_mp3);
	fwrite(buf,10,1,editInfo->fptr_duplicate_file);
	printf("header copied successfully %s at %ld in temp\n",buf,ftell(editInfo->fptr_duplicate_file));

	//checking for edit TAG options
	if(strcmp(argv[2] ,"-t") == 0)
	{
		strcpy(editInfo->TAG ,"TIT2");

		printf("tag is checked successfull\n");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}

	else if(strcmp(argv[2] ,"-a") == 0)
	{
		strcpy(editInfo->TAG ,"TPE1");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}
	else if(strcmp(argv[2] ,"-A") == 0)
	{
		strcpy(editInfo->TAG ,"TALB");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}
	else if(strcmp(argv[2] ,"-y") == 0)
	{
		strcpy(editInfo->TAG ,"TYER");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}
	else if(strcmp(argv[2] ,"-m") == 0)
	{
		strcpy(editInfo->TAG ,"TCON");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}
	else if(strcmp(argv[2] ,"-c") == 0)
	{
		strcpy(editInfo->TAG ,"COMM");
		printf("TAG = %s\t %s is selected\n",editInfo->TAG,argv[2]);
	}
	else
	{
		printf("ERROR : select a valid tag edit option\n");
		return e_failure;
	}
	
	if(copy_data_till_Tag_matched(editInfo ) == e_success)   //fun call
		printf("tag is checked successfull\n");
	else
		puts("copy failed\n");

	old_size=get_old_size(editInfo);                         //fun call to get old size

	store_new_size(editInfo);                                //fun call to store new size
	fseek(editInfo->fptr_edit_mp3,old_size-1,SEEK_CUR);      //skip old content in source file

	if(copy_remaining_bytes(editInfo) == e_success)          //fun call to copy remaining bytes from src to temp file
	{
		printf("remaining bytes copied\n");
		if(copy_duplicate_to_src(editInfo) == e_success)     //fun call to copy all contents from temp to source file
		{
			printf("copy from dup to src success\n");
			return e_success;
		}
		else
		{
			puts("copy to src failed\n");
			return e_failure;
		}
	}
	else
		puts("copy remaining bytes failed !\n");

	return e_success;
}

/* copy data till tag matches fun definition */
Status copy_data_till_Tag_matched(Edit_Info *editInfo )
{
	char buf[1000]={};
	uint old_size;

	fread(buf,4,1,editInfo->fptr_edit_mp3);          //read 4 bytes TAG
	buf[4] = 0;                                      //store null character at buf[4]
	while(strcmp(buf ,editInfo->TAG) != 0)           //run until buffer TAG & source file TAG become same
	{
		
		fwrite(buf,4,1,editInfo->fptr_duplicate_file);    //write TAG to duplicate file if TAG not matched
		old_size = get_old_size(editInfo);                //fun call

		fseek(editInfo -> fptr_edit_mp3,-4,SEEK_CUR);     //moving OFFSET 4bytes backward using fseek
		
		fread(buf,4,1,editInfo->fptr_edit_mp3);           //copy the old size to duplicate file using fread & fwrite
		fwrite(buf,4,1,editInfo->fptr_duplicate_file);
		
		fread(buf,3,1,editInfo->fptr_edit_mp3);           //skip 3bytes flag
		fwrite(buf,3,1,editInfo->fptr_duplicate_file);
		
		fread(buf,old_size-1,1,editInfo->fptr_edit_mp3);  //copy the data from source to duplicate file
		fwrite(buf,old_size-1,1,editInfo->fptr_duplicate_file);
		
		fread(buf,4,1,editInfo->fptr_edit_mp3);           //read the TAG
		buf[4] = 0;                                       //store null to perform string compare operation 
	}
	fwrite(buf,4,1,editInfo->fptr_duplicate_file);        //write the TAG to duplicate file if TAG is same 

	return e_success;
}

/* getting old size fun definition */
uint get_old_size(Edit_Info *editInfo)
{
	uint old_size=0;
	char buf[4];
	
	fread(buf,4,1,editInfo->fptr_edit_mp3);
	for(int i=0;i<4;i++)
	{
		old_size = old_size | (buf[i] << (8*(4-i-1)));    //converting size from hexa value into decimal
	}
//	printf("old size = %u\t %#x\n",old_size,old_size);

	return old_size;
}

/* store new size & new content to duplicate file fun definition */
Status store_new_size(Edit_Info *editInfo)
{
	int size = editInfo->new_size;
	char buf[4];
	printf("newsize = %d\n",size);

	//store new size into temp file
	for(int i=0;i<4;i++)
	{
		buf[i] = (size<<(8*(3-i)));
//		printf("buf[%d] = %d\n",i,buf[i]);
	}
	fwrite(buf,4,1,editInfo->fptr_duplicate_file);    //write new size stored in buf to temp file

	fread(buf,3,1,editInfo->fptr_edit_mp3);           //read flag from src file
	fwrite(buf,3,1,editInfo->fptr_duplicate_file);    //write flag to temp file
	printf("3bytes flag is skipped\n");

	//write new data into the temp file
	fwrite(editInfo->new_data,size-1,1,editInfo->fptr_duplicate_file);
	printf("new_data = %s\n",editInfo->new_data);

	return e_success;
}

/* copying remaining bytes from source file to duplicate file fun definition */
Status copy_remaining_bytes(Edit_Info *editInfo)
{
	char buf;
	
	//copy remaining bytes
	while(fread(&buf,1,1,editInfo->fptr_edit_mp3))
		fwrite(&buf,1,1,editInfo->fptr_duplicate_file);
}

/* copying all contents of duplicate file into original source file */
Status  copy_duplicate_to_src(Edit_Info *editInfo)
{
	char buf;

	editInfo->fptr_duplicate_file = fopen(editInfo->duplicate_mp3_fname ,"r");   //open duplicate file in read mode
	editInfo->fptr_edit_mp3 = fopen(editInfo->edit_mp3_fname , "w");             //open source file in write mode 
	
	rewind(editInfo->fptr_duplicate_file);     //offset pointing to starting position of file

	//copying content until EOF
	while(fread(&buf,1,1,editInfo->fptr_duplicate_file))
		fwrite(&buf,1,1,editInfo->fptr_edit_mp3);

	return e_success;
}
