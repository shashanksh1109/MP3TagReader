#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "edit.h"
#include "types.h"

//read and validate function
Status read_and_validate_edit_args(char *argv[] ,Edit_Info *editInfo)
{
	if(strstr(argv[4] ,".mp3"))
	{
		editInfo->edit_mp3_fname = argv[4];
		editInfo->duplicate_mp3_fname = "temp.mp3";
		strcpy(editInfo->new_data , argv[3]);
		editInfo->new_size = strlen(argv[3]);

		if(open_src_edit_file(editInfo) == e_success)
		{
			printf("file opened successfully\n");
		}
	}
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
	int old_size;
	fread(buf,3,1,editInfo->fptr_edit_mp3);
	if(strcmp(buf,"ID3") != 0)
	{
		printf("tag is not ID3\n");
		return e_failure;
	}

	rewind(editInfo->fptr_edit_mp3);
	//skip 10 bytes header
	fread(buf,10,1,editInfo->fptr_edit_mp3);
	fwrite(buf,10,1,editInfo->fptr_duplicate_file);
	printf("header copied successfully %s at %d in temp\n",buf,ftell(editInfo->fptr_duplicate_file));


	if(strcmp(argv[2] ,"-t") == 0)
	{
		//read size
		fread(buf,4,1,editInfo->fptr_edit_mp3);
		printf("inside %s\t ftell = %ld\n",buf,ftell(editInfo->fptr_edit_mp3));

		if(strcmp(buf ,"TIT2") == 0)
		{
			printf("Inside strcmp\n");
			fwrite(buf,4,1,editInfo->fptr_duplicate_file);
		}
		//call read size fun
		old_size = get_old_size(editInfo);
		printf("get old size success %d\n",old_size);

		if(store_new_size(editInfo) == e_success)
		{
			puts("data stored success\n");
		}
		//skip old size-1 times in src file
		fseek(editInfo->fptr_edit_mp3,old_size-1,SEEK_CUR);
		printf("after fseek = %d\n",ftell(editInfo->fptr_edit_mp3));
		if(copy_remaining_bytes(editInfo) == e_success)
		{
			printf("remaining bytes copied\n");
			if(copy_duplicate_to_src(editInfo) == e_success)
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

	}

	//skipping size from fptr_edit_mp3
#if 0
	else if(strcmp(argv[2] ,"-a") == 0)
	{
		printf("-a selected \n");
		fread(buf,4,1,editInfo->fptr_edit_mp3);
		while(strcmp(buf ,"TPE1") != 0)
		{
			printf("buf = %s not TPE1\n",buf);
			fread(buf,4,1,editInfo->fptr_edit_mp3);
			fwrite(buf,4,1,editInfo->fptr_edit_mp3);
			old_size = get_old_size(editInfo);

			fread(buf,3,1,editInfo->fptr_edit_mp3);
			fwrite(buf,3,1,editInfo->fptr_edit_mp3);
			fwrite(buf,old_size-1,1,editInfo->fptr_duplicate_file);
		}
		if(strcmp(buf,"TPE1") == 0)
		{
			fwrite(buf,4,1,editInfo->fptr_duplicate_file);
			old_size=get_old_size(editInfo);

			store_new_size(editInfo);
			fseek(editInfo->fptr_edit_mp3,old_size-1,SEEK_CUR);

			if(copy_remaining_bytes(editInfo) == e_success)
			{
				printf("remaining bytes copied\n");
				if(copy_duplicate_to_src(editInfo) == e_success)
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


		}
	}
#endif
	/*   else if(argv[2] == "-A")
		 {

		 }
		 else
		 {
		 printf("Error : invalid option ! %s\n",argv[2]);
		 return e_failure;
		 }*/
	return e_success;
}

uint get_old_size(Edit_Info *editInfo)
{
	uint old_size=0;
	char buf[4];
	fread(buf,4,1,editInfo->fptr_edit_mp3);
	for(int i=0;i<4;i++)
	{
		printf("buf[%d] = %x\n",i,buf[i]);
		old_size = old_size | (buf[i] << (8*(4-i-1)));
	}
	putchar('\n');
	printf("old size = %u\t %x\n",old_size,old_size);


	return old_size;
}

Status store_new_size(Edit_Info *editInfo)
{
	int size = editInfo->new_size;
	char buf[4];
	printf("newsize = %d\n",size);

	//store new size into temp file
	for(int i=0;i<4;i++)
	{
		buf[i] = (size<<(8*(3-i)));
		printf("buf[%d] = %d\n",i,buf[i]);
	}
	fwrite(buf,4,1,editInfo->fptr_duplicate_file);

	printf("newsize = %d\n",size);

	//read flag from src file
	fread(buf,3,1,editInfo->fptr_edit_mp3);
	fwrite(buf,3,1,editInfo->fptr_duplicate_file);
	printf("3bytes flag is skipped\n");

	fwrite(editInfo->new_data,size,1,editInfo->fptr_duplicate_file);
	printf("new_data = %s\n",editInfo->new_data);
	return e_success;
}

Status copy_remaining_bytes(Edit_Info *editInfo)
{
	char buf;
	printf("after data copy offset is in %d pos\n",ftell(editInfo->fptr_edit_mp3));
	//copy remaining bytes
	while(fread(&buf,1,1,editInfo->fptr_edit_mp3))
		fwrite(&buf,1,1,editInfo->fptr_duplicate_file);
}

Status  copy_duplicate_to_src(Edit_Info *editInfo)
{
	char buf;
	editInfo->fptr_duplicate_file = fopen(editInfo->duplicate_mp3_fname ,"r");
	editInfo->fptr_edit_mp3 = fopen(editInfo->edit_mp3_fname , "w");
	rewind(editInfo->fptr_duplicate_file);
	while(fread(&buf,1,1,editInfo->fptr_duplicate_file))
		fwrite(&buf,1,1,editInfo->fptr_edit_mp3);

	return e_success;
}
