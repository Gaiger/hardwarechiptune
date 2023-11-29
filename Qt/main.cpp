
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <QApplication>
#include "WaveGenerator.h"
#include "AudioPlayer.h"

#include "HardwareChiptunePanelWidget.h"

#ifndef _STUFF_H_
#define _STUFF_H_
extern "C"{
	#include "../stuff.h"
}
#endif
#if(0)
const u16 instrptr[] = {
	0, 1
};

const u8 instrdata[] = {
	0, 0,
	'v', 255, 'm', 0x11, '+', 0, 'w', WF_PUL, 'f', 0xef, 't', 8, 'f', 0xfc, 0, 0, // lead
	'v', 255, 'w', WF_NOI, 't', 2, '+', 0, 'w', WF_SAW, 'f', 0xef, 't', 8, 'f', 0, 't', 12, '~', 0x37, 'f', 0xfc, 0, 0, // lead+vibr
	'v', 255, 'w', WF_NOI, 't', 2, 'm', 0x11, '+', 12, 'w', WF_PUL, 'f', 0xfc, 't', 2, '+', 7, 't', 2, '+', 3, 't', 2, '+', 0, 't', 2, 'j', 5, 0, 0, // arp
	'v', 255, 'w', WF_NOI, 't', 2, 'd', 0x80, '+', 0, 'w', WF_PUL, 'l', 0x80, 't', 12, 'v', 0, 0, 0	// bass drum
};

const struct track trackdata = {
	{
		{12*3+0,	1,	{'i',	0},	{0xff,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+0,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*3+2,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+7,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+2,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*2+10,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+0,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+0,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*3+7,	1,	{0,	0},	{0,	0}},
		{12*3+8,	1,	{0,	0},	{0,	0}},
		{12*3+5,	1,	{0,	0},	{0,	0}},
		{12*3+7,	1,	{0,	0},	{0,	0}},
		{12*3+3,	1,	{0,	0},	{0,	0}},
		{12*3+2,	1,	{0,	0},	{0,	0}},
		{12*2+0,	1,	{0,	0},	{0,	0}},
		{12*2+10,	1,	{0,	0},	{0,	0}},
		{12*2+7,	1,	{0,	0},	{0,	0}},
		{12*3+2,	1,	{0,	0},	{0,	0}},
	}
};

#endif
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	WaveGenerator wave_generator("../test2.song");
	AudioPlayer player(&wave_generator, &a);

	HardwareChiptunePanelWidget hardware_chiptune_panel_widget;
	hardware_chiptune_panel_widget.show();
	hardware_chiptune_panel_widget.setFocus();
	//player.PlaySong(1);
	player.PlayTrack(1);
	return a.exec();
}
