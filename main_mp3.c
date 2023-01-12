/*
Name :Harshitha H.R.
Date :
Description :
Sample i/p :
Sample o/p :
 */

#include <stdio.h>
#include <string.h>
#include "types.h"
#include "view.h"
#include "edit.h"

int main(int argc, char *argv[])
{
	MP3Info mp3Info;
	if(argc < 2)
	{
		goto label1;
	}

	if(check_Operation_type(argv) ==  e_help)
	{
		printf("\n---------------------------------HELP MENU-----------------------------------\n\n");
		printf("1. -v -> to view mp3 file contents\n2. -e -> to edit mp3 file contents\n");
		printf("%10d.1. -t -> to edit song title\n",2);
		printf("%10d.2. -a -> to edit artist name\n",2);
		printf("%10d.3. -A -> to edit album name\n",2);
		printf("%10d.4. -y -> to edit year\n",2);
		printf("%10d.5. -m -> to edit content\n",2);
		printf("%10d.6. -c -> to edit comment\n\n",2);
		printf("-----------------------------------------------------------------------------\n");
	}
	else if(check_Operation_type(argv) == e_view)
	{
		//fun call
		printf("------inside e_view-----\n");
		MP3Info mp3Info;

		if(argc < 3)
		{
			goto label1;
		}

		if(read_and_validate_args(argv, &mp3Info) == e_success)
		{
			//fun call
			printf("--------read & validate success--------\n");
			if(view_mp3_file_info(&mp3Info) == e_success)
			{
				printf("-----------------------------SELECTED VIEW DETAILS------------------------------\n\n\n");
				printf("--------------------------------------------------------------------------------\n");
				printf("                       MP3 TAG READER AND EDITOR FOR ID3V2                      \n");
				printf("--------------------------------------------------------------------------------\n");
				printf("%-16s : %-10s %s\n","TITLE"," ",mp3Info.title_name);
				printf("%-16s : %-10s %s\n","ARTIST"," ",mp3Info.artist_name);
				printf("%-16s : %-10s %s\n","ALBUM"," ",mp3Info.album_name);
				printf("%-16s : %-10s %s\n","YEAR"," ",mp3Info.year);
				printf("%-16s : %-10s %s\n","MUSIC"," ",mp3Info.content_type);
				printf("%-16s : %-10s %s\n","COMMENT"," ",mp3Info.comment);
				/*	printf("TITLE            :           %s\n",mp3Info.title_name);
					printf("ARTIST           :           %s\n",mp3Info.artist_name);
					printf("ALBUM            :           %s\n",mp3Info.album_name);
					printf("YEAR             :           %s\n",mp3Info.year);
					printf("MUSIC            :           %s\n",mp3Info.content_type);
					printf("COMMENT          :           %s\n",mp3Info.comment);*/
				printf("--------------------------------------------------------------------------------\n\n");
				printf("--------------------------DETAILS DISPLAYED SUCCESSFULLY------------------------\n");


			}
			else
				printf("ERROR : viewing file failed !\n");
		}
		else
		{
			printf("ERROR: read and validate failed !\n");
			goto label1;
		}
	}
	else if(check_Operation_type(argv) == e_edit)
	{
		Edit_Info editInfo;
		//func call
		printf("------------------------------SELECTED EDIT DETAILS----------------------------\n");

		if(argc < 5)
		{
			goto label1;
		}

		if(read_and_validate_edit_args(argv ,&editInfo) == e_success)
		{
			printf("read & validate edit success\n");
			//fun call to perform edit operation
			if(edit_mp3_file_info(&editInfo ,argv) == e_success)
			{
				printf("edit completed\n");
			}
			else
			{
				printf("edit failed !");
				return e_failure;
			}
		}
		else
		{
			printf("ERROR: read and validate failed !\n");
			goto label1;
		}
	}
	else if(check_Operation_type(argv) == e_unsupported)
	{
label1:		printf("\n-----------------------------------------------------------------------------\n\n");
			printf("ERROR: ./a.out : INVALID ARGUMENTS\nUSAGE :\n");
			printf("To view please pass like : ./a.out -v mp3filename\n");
			printf("To edit please pass like : ./a.out -e -t/-a/-A/-m/-y/-c changing_text mp3filename\n");
			printf("To get help pass like : ./a.out --help\n");
			printf("\n-----------------------------------------------------------------------------\n");

			return e_failure;
	}
	return 0;
}

OperationType check_Operation_type(char *argv[])
{
	if(strcmp(argv[1], "--help") == 0)
	{
		return e_help;
	}
	if(strcmp(argv[1], "-v") == 0)
	{
		return e_view;
	}
	else if(strcmp(argv[1], "-e") == 0)
	{
		return e_edit;
	}
	else
		return e_unsupported;
}
