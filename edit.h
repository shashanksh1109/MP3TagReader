#ifndef EDIT_H
#define EDIT_H

#include "types.h"

typedef struct _EditInfo
{
	//edit mp3_info
	char *edit_mp3_fname;
	char *duplicate_mp3_fname;
	FILE *fptr_edit_mp3;
	FILE *fptr_duplicate_file;
	char new_data[100];
	uint new_size;

}Edit_Info;

/* function prototypes for edit option */

//read and validate args function
Status read_and_validate_edit_args(char *argv[] ,Edit_Info *editInfo);

//edit mp3 file
Status edit_mp3_file_info(Edit_Info *editInfo ,char *argv[]);

//open source file function
Status open_src_edit_file(Edit_Info *editInfo);

//get old size
uint get_old_size(Edit_Info *editInfo);

//store new size
uint store_new_size(Edit_Info *editInfo);

//copy remaining bytes
Status copy_remaining_bytes(Edit_Info *editInfo);

//copy duplicate file contents to source file
Status copy_duplicate_to_src(Edit_Info *editInfo);

#endif
