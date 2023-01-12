#ifndef VIEW_H 
#define VIEW_H

#include "types.h"

typedef struct _MP3Info
{
	//view mp3_info
	char *view_mp3_fname;
	FILE *fptr_view_mp3;
	char *title_name;
	char *artist_name;
	char *album_name;
	char *year;
	char *content_type;
	char *comment;
	uint tag_size;
	uint flag;

} MP3Info;

/* function prototypes for view option */

//check OperationType function
OperationType check_Operation_type(char *argv[]);

//read and validate args function
Status read_and_validate_args(char *argv[] ,MP3Info *mp3Info);

//open source file function
Status open_src_file(MP3Info *mp3Info);

//view mp3 file info
Status view_mp3_file_info(MP3Info *mp3Info);

//check mp3 tags
Status check_mp3_tags(MP3Info *mp3Info);

//get tag size
uint get_tag_size(FILE *fptr_view_mp3);

//get meta data
Status get_meta_data(FILE *fptr_view_mp3 ,char *buff ,uint size);

#endif
