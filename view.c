#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "types.h"
#include "view.h"

/*read and validate function */

Status read_and_validate_args(char *argv[] ,MP3Info *mp3Info)
{
	if(strstr(argv[2] , ".mp3") != NULL && strcmp(strstr(argv[2] ,".mp3") ,".mp3") == 0)
	{
		mp3Info->view_mp3_fname = argv[2];

		if(open_src_file(mp3Info) == e_success)
		{
			printf("file opened successfully\n");
		}

		return e_success;
	}
	else
		return e_failure;
}

/* view mp3 file info function */

Status view_mp3_file_info(MP3Info *mp3Info)
{
	char *buf;

	fread(buf ,3 ,1 ,mp3Info->fptr_view_mp3);       //reading 3bytes from source file & checking its TAG is ID3 or not

	if(strcmp(buf ,"ID3") == 0)
	{
		printf("TAG = %s\n",buf);

		fseek(mp3Info->fptr_view_mp3 ,7 ,SEEK_CUR);                   //skip 7bytes of frame header using fseek 

		if(check_mp3_tags(mp3Info) == e_success)
		{
			printf("check_mp3_tags success\n");
		}
		else
			printf("check tags failed !\n");
	}
	else
		return e_failure;

	return e_success;
}


/* open source file */

Status open_src_file(MP3Info *mp3Info)
{
	//opening a file in read mode using fopen file operation
	mp3Info->fptr_view_mp3 = fopen(mp3Info->view_mp3_fname ,"r");

	//error handling
	if(mp3Info->fptr_view_mp3 == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", mp3Info->view_mp3_fname);

		return e_failure;
	}

	return e_success;
}


//check mp3 TAGS
Status check_mp3_tags(MP3Info *mp3Info)
{
	char buf[4];

	//reading next 4bytes from source file to check TIT2 Tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);            

	if(strcmp(buf ,"TIT2") == 0)
	{

		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;

		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);

		mp3Info->title_name = malloc(mp3Info->tag_size * sizeof(char));

		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->title_name ,mp3Info->tag_size) == e_success)
		{
			printf("title name = %s\n",mp3Info->title_name);
		}
		else
			return e_failure;
	}
	else
		return e_failure;
	//reading next 4bytes from source file to check TPE1 Tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);
	printf("before TPE1\n");
	if(strcmp(buf ,"TPE1") == 0)
	{
	printf("after TPE1\n");

		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;
		
		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);

		mp3Info->artist_name = malloc(mp3Info->tag_size * sizeof(char));

		printf("tag size = %d\n",mp3Info->tag_size);

		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->artist_name ,mp3Info->tag_size) == e_success)
		{
			printf("artist name = %s\n",mp3Info->artist_name);
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	//reading next 4bytes from source file to check TALB Tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);
	if(strcmp(buf ,"TALB") == 0)
	{
		
		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;

		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);
		mp3Info->album_name = malloc(mp3Info->tag_size * sizeof(char));

		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->album_name ,mp3Info->tag_size) == e_success)
		{
			printf("album name = %s\n",mp3Info->album_name);
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	//readin 4bytes from source file to check for TYER tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);
	if(strcmp(buf ,"TYER") == 0)
	{
		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;

		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);

		mp3Info->year = malloc(mp3Info->tag_size * sizeof(char));
		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->year ,mp3Info->tag_size) == e_success)
		{
			printf("year = %s\n",mp3Info->year);
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	//reading 4bytes from source file to check for TCON tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);

	if(strcmp(buf ,"TCON") == 0)
	{
		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;

		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);
		
		mp3Info->content_type = malloc(mp3Info->tag_size * sizeof(char));

		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->content_type ,mp3Info->tag_size) == e_success)
		{
			printf("content type = %s\n",mp3Info->content_type);
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	//read 4bytes from source file to check for COMM tag
	fread(buf ,4 ,1 ,mp3Info->fptr_view_mp3);

	if(strcmp(buf ,"COMM") == 0)
	{
		mp3Info->tag_size = get_tag_size(mp3Info->fptr_view_mp3)-1;
		
		//read 3bytes flag from source file
		fread(&mp3Info->flag ,3 ,1 ,mp3Info->fptr_view_mp3);

		mp3Info->comment = malloc(mp3Info->tag_size * sizeof(char));

		if(get_meta_data(mp3Info->fptr_view_mp3 ,mp3Info->comment ,mp3Info->tag_size) == e_success)
		{
			printf("comment = %s\n",mp3Info->comment);
		}
		else
			return e_failure;
	}
	else
		return e_failure;

	return e_success;
}

//fetching the size of contents of the tag
uint get_tag_size(FILE *fptr_view_mp3)
{
	char buf[4];
	int size = 0;
	fread(buf ,4 ,1 ,fptr_view_mp3);

	for(int i=0;i<4;i++)
	{
		size = size | buf[i] << 8*(4-i-1);
	}

	return size;
}

//get meta data info
Status get_meta_data(FILE *fptr_view_mp3 ,char *buff ,uint size)
{

//	fread(buff ,size-1 ,1 ,fptr_view_mp3);
	fread(buff ,size ,1 ,fptr_view_mp3);
//	buff[size-1] = '\0';

	return e_success;
}
