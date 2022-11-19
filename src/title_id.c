#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sd_config.h"


static int find_index_pos(const char* title_id)
{
	char first_id[5] = "";
	first_id[0] = title_id[0];
	first_id[1] = title_id[1];
	first_id[2] = title_id[2];
	first_id[3] = title_id[3];
	int second_id = atoi(&(title_id[5]));
	
	FIL fil;
	FRESULT fr = f_open(&fil, "titleid_name.index.txt", FA_READ);
	if(FR_OK != fr)
		return 0;
	
	char buffer_first_id[64]="";
	int buffer_second_id = 0;

	int buffer_pos;
	int before_pos = 0;


	int pos = 0;
	int same_first_id = 0;
	int bigger_than = 0;
	while (f_gets(buffer_first_id, 64, &fil) != NULL)
	{
		buffer_first_id[4] = '\0';
		buffer_first_id[10] = '\0';
		buffer_second_id = atoi(&(buffer_first_id[5]));
		buffer_pos = atoi(&(buffer_first_id[11]));

		if (strcmp(first_id, buffer_first_id) == 0)
			same_first_id = 1;
		else
		{
			if (same_first_id == 1)
			{
				f_close(&fil);
				return before_pos;
			}

			same_first_id = 0;
			before_pos = buffer_pos;
			bigger_than = 0;
			continue;
		}

		if (bigger_than == 1 && second_id < buffer_second_id)
		{
			f_close(&fil);
			return before_pos;
		}

		if (second_id >= buffer_second_id)
			bigger_than = 1;
		else
			bigger_than = 0;
		before_pos = buffer_pos;
	}
	if (same_first_id == 1 && bigger_than == 1){
		f_close(&fil);
		return before_pos;
	}
	f_close(&fil);
	return 0;
}


struct title_info {
	char* id;
	char* name;
};

#define MAX_TITLE_INFO_CACHE_SIZE 100
static struct title_info title_info_cache[MAX_TITLE_INFO_CACHE_SIZE] = {{0,0},};
static int cache_first = 0;
static int cache_last = 0;

static const char* cache_find(const char* title_id)
{
	int current_index = cache_first;
	while(current_index != cache_last)
	{
		if (strcmp(title_info_cache[current_index].id, title_id) == 0)
			return title_info_cache[current_index].name;
		current_index++;
		current_index = current_index % MAX_TITLE_INFO_CACHE_SIZE;
	}
	return NULL;
}
static void cache_insert(const char* title_id, const char* title_name){
	if (title_info_cache[cache_last].id){
		free(title_info_cache[cache_last].id);
		title_info_cache[cache_last].id = 0;
	}
	if (title_info_cache[cache_last].name){
		free(title_info_cache[cache_last].name);
		title_info_cache[cache_last].name = 0;
	}
	title_info_cache[cache_last].id = strdup(title_id);
	title_info_cache[cache_last].name = strdup(title_name);
	cache_last++;
	cache_last = cache_last % MAX_TITLE_INFO_CACHE_SIZE;
	if (cache_first == cache_last)
	{
		cache_first++;
		cache_first = cache_first % MAX_TITLE_INFO_CACHE_SIZE;
	}
}



void title_id_make_index()
{
	FIL fil;
	FIL out_fil;
	FRESULT fr = f_open(&out_fil, "titleid_name.index.txt", FA_CREATE_NEW | FA_WRITE);
	if(FR_OK != fr)
		return;

	fr = f_open(&fil, "titleid_name.txt", FA_READ);
	if(FR_OK != fr){
		f_close(&out_fil);
		return;
	}

	

	char buffer[256];

	f_lseek(&fil, 0);

	char first_id[5] = "";
	char second_id[6] = "";

	char b_first_id[5] = "";
	char titleid_and_filepos[64] = "";

	long cur_pos = f_tell(&fil);

	int i=0;
	int id_count=0;
	while (f_gets(buffer, 256, &fil) != NULL)
	{

		first_id[0] = buffer[0];
		first_id[1] = buffer[1];
		first_id[2] = buffer[2];
		first_id[3] = buffer[3];
		first_id[4] = '\0';

		second_id[0] = buffer[5];
		second_id[1] = buffer[6];
		second_id[2] = buffer[7];
		second_id[3] = buffer[8];
		second_id[4] = buffer[9];
		second_id[5] = '\0';

		if (strcmp(b_first_id, first_id) != 0 || id_count == 500){

			sprintf(titleid_and_filepos, "%s-%s %d\n",first_id, second_id, cur_pos);
			UINT n;
			f_write(&out_fil, titleid_and_filepos, strlen(titleid_and_filepos), &n);
			
			if (strcmp(b_first_id, first_id) != 0)
				strcpy(b_first_id, first_id);
			
			id_count = 0;
			cur_pos = f_tell(&fil);
			
		}else
		{
			id_count++;
		}

	}

	f_close(&fil);
	f_close(&out_fil);
}


static char out_titlename[256] = "";
const char* title_id_find_name(const char* title_id)
{
	const char* title_name = cache_find(title_id);
	if (title_name)
		return title_name;

	char buffer[256];
	int pos = find_index_pos(title_id);
	out_titlename[0] ='\0';

	FIL fil;
	FRESULT fr = f_open(&fil, "titleid_name.txt", FA_READ);
	if(FR_OK != fr)
		return out_titlename;

	f_lseek(&fil, pos);
	
	while (f_gets(buffer, 256, &fil) != NULL)
	{
		buffer[10] = '\0';
		if (strcmp(title_id, buffer) == 0)
		{
			strcpy(out_titlename, &(buffer[11]));
			break;
		}
	}
	f_close(&fil);
	cache_insert(title_id, out_titlename);
	return out_titlename;
}