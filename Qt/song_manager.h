#ifndef _SONG_MANAGER_H_
#define _SONG_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>
#ifndef _STUFF_H_
#define _STUFF_H_
#include "../stuff.h"
#endif


void readsong(int pos, int ch, u8 *dest);
void readtrack(int num, int pos, struct trackline *tl);
void readinstr(int num, int pos, u8 *il);

void loadfile(char *fname) ;

struct songline {
	u8			track[4];
	u8			transp[4];
};

void get_songlines(void ** pp_songlines, int *p_number_of_songlines);


void get_tracks(void ** pp_track, int *p_track_number, int *p_track_length);

bool is_song_playing(int *p_processing_song_index);
bool is_track_playing(int *p_playing_track_index, int *p_playing_line_index);
#ifdef __cplusplus
}
#endif

#endif // _SONG_MANAGER_H_