#include <avr/io.h>
#include <avr/signal.h>
#include <avr/interrupt.h>

#define TRACKLEN 32

typedef unsigned char u8;
typedef unsigned short u16;
typedef char s8;
typedef short s16;
typedef unsigned long u32;

volatile u8 callbackwait;
volatile u8 lastsample;

volatile u8 timetoplay;

volatile u8 test;
volatile u8 testwait;

u8 trackwait;
u8 trackpos;
u8 playsong;
u8 songpos;

u32 noiseseed = 1;

u8 light[2];

#include "../tracker/exported.h"

/*const u16 freqtable[] = {
	0x010b, 0x011b, 0x012c, 0x013e, 0x0151, 0x0165, 0x017a, 0x0191, 0x01a9,
	0x01c2, 0x01dd, 0x01f9, 0x0217, 0x0237, 0x0259, 0x027d, 0x02a3, 0x02cb,
	0x02f5, 0x0322, 0x0352, 0x0385, 0x03ba, 0x03f3, 0x042f, 0x046f, 0x04b2,
	0x04fa, 0x0546, 0x0596, 0x05eb, 0x0645, 0x06a5, 0x070a, 0x0775, 0x07e6,
	0x085f, 0x08de, 0x0965, 0x09f4, 0x0a8c, 0x0b2c, 0x0bd6, 0x0c8b, 0x0d4a,
	0x0e14, 0x0eea, 0x0fcd, 0x10be, 0x11bd, 0x12cb, 0x13e9, 0x1518, 0x1659,
	0x17ad, 0x1916, 0x1a94, 0x1c28, 0x1dd5, 0x1f9b, 0x217c, 0x237a, 0x2596,
	0x27d3, 0x2a31, 0x2cb3, 0x2f5b, 0x322c, 0x3528, 0x3851, 0x3bab, 0x3f37,
	0x42f9, 0x46f5, 0x4b2d, 0x4fa6, 0x5462, 0x5967, 0x5eb7, 0x6459, 0x6a51,
	0x70a3, 0x7756, 0x7e6f
};*/

const u16 freqtable[] = {
	0x0085, 0x008d, 0x0096, 0x009f, 0x00a8, 0x00b2, 0x00bd, 0x00c8, 0x00d4,
	0x00e1, 0x00ee, 0x00fc, 0x010b, 0x011b, 0x012c, 0x013e, 0x0151, 0x0165,
	0x017a, 0x0191, 0x01a9, 0x01c2, 0x01dd, 0x01f9, 0x0217, 0x0237, 0x0259,
	0x027d, 0x02a3, 0x02cb, 0x02f5, 0x0322, 0x0352, 0x0385, 0x03ba, 0x03f3,
	0x042f, 0x046f, 0x04b2, 0x04fa, 0x0546, 0x0596, 0x05eb, 0x0645, 0x06a5,
	0x070a, 0x0775, 0x07e6, 0x085f, 0x08de, 0x0965, 0x09f4, 0x0a8c, 0x0b2c,
	0x0bd6, 0x0c8b, 0x0d4a, 0x0e14, 0x0eea, 0x0fcd, 0x10be, 0x11bd, 0x12cb,
	0x13e9, 0x1518, 0x1659, 0x17ad, 0x1916, 0x1a94, 0x1c28, 0x1dd5, 0x1f9b,
	0x217c, 0x237a, 0x2596, 0x27d3, 0x2a31, 0x2cb3, 0x2f5b, 0x322c, 0x3528,
	0x3851, 0x3bab, 0x3f37
};

const s8 sinetable[] = {
	0, 12, 25, 37, 49, 60, 71, 81, 90, 98, 106, 112, 117, 122, 125, 126,
	127, 126, 125, 122, 117, 112, 106, 98, 90, 81, 71, 60, 49, 37, 25, 12,
	0, -12, -25, -37, -49, -60, -71, -81, -90, -98, -106, -112, -117, -122,
	-125, -126, -127, -126, -125, -122, -117, -112, -106, -98, -90, -81,
	-71, -60, -49, -37, -25, -12
};

const u8 validcmds[] = "0dfijlmtvw~+=";

enum {
	WF_TRI,
	WF_SAW,
	WF_PUL,
	WF_NOI
};

volatile struct oscillator {
	u16	freq;
	u16	phase;
	u16	duty;
	u8	waveform;
	u8	volume;	// 0-255
} osc[4];

struct trackline {
	u8	note;
	u8	instr;
	u8	cmd[2];
	u8	param[2];
	};

struct track {
	struct trackline	line[TRACKLEN];
};

struct unpacker {
	u16	nextbyte;
	u8	buffer;
	u8	bits;
};

struct channel {
	struct unpacker		trackup;
	u8			tnum;
	s8			transp;
	u8			tnote;
	u8			lastinstr;
	u8			inum;
	u16			iptr;
	u8			iwait;
	u8			inote;
	s8			bendd;
	s16			bend;
	s8			volumed;
	s16			dutyd;
	u8			vdepth;
	u8			vrate;
	u8			vpos;
	s16			inertia;
	u16			slur;
} channel[4];

u16 resources[16 + MAXTRACK];

struct unpacker songup;

u8 readsongbyte(u16 offset);
void watchdogoff();

void initup(struct unpacker *up, u16 offset) {
	up->nextbyte = offset;
	up->bits = 0;
}

u8 readbit(struct unpacker *up) {
	u8 val;

	if(!up->bits) {
		up->buffer = readsongbyte(up->nextbyte++);
		up->bits = 8;
	}

	up->bits--;
	val = up->buffer & 1;
	up->buffer >>= 1;

	return val;
}

u16 readchunk(struct unpacker *up, u8 n) {
	u16 val = 0;
	u8 i;

	for(i = 0; i < n; i++) {
		if(readbit(up)) {
			val |= (1 << i);
		}
	}

	return val;
}

void readinstr(u8 num, u8 pos, u8 *dest) {
	dest[0] = readsongbyte(resources[num] + 2 * pos + 0);
	dest[1] = readsongbyte(resources[num] + 2 * pos + 1);
}

void runcmd(u8 ch, u8 cmd, u8 param) {
	switch(validcmds[cmd]) {
		case '0':
			channel[ch].inum = 0;
			break;
		case 'd':
			osc[ch].duty = param << 8;
			break;
		case 'f':
			channel[ch].volumed = param;
			break;
		case 'i':
			channel[ch].inertia = param << 1;
			break;
		case 'j':
			channel[ch].iptr = param;
			break;
		case 'l':
			channel[ch].bendd = param;
			break;
		case 'm':
			channel[ch].dutyd = param << 6;
			break;
		case 't':
			channel[ch].iwait = param;
			break;
		case 'v':
			osc[ch].volume = param;
			break;
		case 'w':
			osc[ch].waveform = param;
			break;
		case '+':
			channel[ch].inote = param + channel[ch].tnote - 12 * 4;
			break;
		case '=':
			channel[ch].inote = param;
			break;
		case '~':
			if(channel[ch].vdepth != (param >> 4)) {
				channel[ch].vpos = 0;
			}
			channel[ch].vdepth = param >> 4;
			channel[ch].vrate = param & 15;
			break;
	}
}

void playroutine() {			// called at 50 Hz
	u8 ch;
	u8 lights;

	if(playsong) {
		if(trackwait) {
			trackwait--;
		} else {
			trackwait = 4;

			if(!trackpos) {
				if(playsong) {
					if(songpos >= SONGLEN) {
						playsong = 0;
					} else {
						for(ch = 0; ch < 4; ch++) {
							u8 gottransp;
							u8 transp;

							gottransp = readchunk(&songup, 1);
							channel[ch].tnum = readchunk(&songup, 6);
							if(gottransp) {
								transp = readchunk(&songup, 4);
								if(transp & 0x8) transp |= 0xf0;
							} else {
								transp = 0;
							}
							channel[ch].transp = (s8) transp;
							if(channel[ch].tnum) {
								initup(&channel[ch].trackup, resources[16 + channel[ch].tnum - 1]);
							}
						}
						songpos++;
					}
				}
			}

			if(playsong) {
				for(ch = 0; ch < 4; ch++) {
					if(channel[ch].tnum) {
						u8 note, instr, cmd, param;
						u8 fields;

						fields = readchunk(&channel[ch].trackup, 3);
						note = 0;
						instr = 0;
						cmd = 0;
						param = 0;
						if(fields & 1) note = readchunk(&channel[ch].trackup, 7);
						if(fields & 2) instr = readchunk(&channel[ch].trackup, 4);
						if(fields & 4) {
							cmd = readchunk(&channel[ch].trackup, 4);
							param = readchunk(&channel[ch].trackup, 8);
						}
						if(note) {
							channel[ch].tnote = note + channel[ch].transp;
							if(!instr) instr = channel[ch].lastinstr;
						}
						if(instr) {
							if(instr == 2) light[1] = 5;
							if(instr == 1) {
								light[0] = 5;
								if(channel[ch].tnum == 4) {
									light[0] = light[1] = 3;
								}
							}
							if(instr == 7) {
								light[0] = light[1] = 30;
							}
							channel[ch].lastinstr = instr;
							channel[ch].inum = instr;
							channel[ch].iptr = 0;
							channel[ch].iwait = 0;
							channel[ch].bend = 0;
							channel[ch].bendd = 0;
							channel[ch].volumed = 0;
							channel[ch].dutyd = 0;
							channel[ch].vdepth = 0;
						}
						if(cmd) runcmd(ch, cmd, param);
					}
				}

				trackpos++;
				trackpos &= 31;
			}
		}
	}

	for(ch = 0; ch < 4; ch++) {
		s16 vol;
		u16 duty;
		u16 slur;

		while(channel[ch].inum && !channel[ch].iwait) {
			u8 il[2];

			readinstr(channel[ch].inum, channel[ch].iptr, il);
			channel[ch].iptr++;

			runcmd(ch, il[0], il[1]);
		}
		if(channel[ch].iwait) channel[ch].iwait--;

		if(channel[ch].inertia) {
			s16 diff;

			slur = channel[ch].slur;
			diff = freqtable[channel[ch].inote] - slur;
			//diff >>= channel[ch].inertia;
			if(diff > 0) {
				if(diff > channel[ch].inertia) diff = channel[ch].inertia;
			} else if(diff < 0) {
				if(diff < -channel[ch].inertia) diff = -channel[ch].inertia;
			}
			slur += diff;
			channel[ch].slur = slur;
		} else {
			slur = freqtable[channel[ch].inote];
		}
		osc[ch].freq =
			slur +
			channel[ch].bend +
			((channel[ch].vdepth * sinetable[channel[ch].vpos & 63]) >> 2);
		channel[ch].bend += channel[ch].bendd;
		vol = osc[ch].volume + channel[ch].volumed;
		if(vol < 0) vol = 0;
		if(vol > 255) vol = 255;
		osc[ch].volume = vol;

		duty = osc[ch].duty + channel[ch].dutyd;
		if(duty > 0xe000) duty = 0x2000;
		if(duty < 0x2000) duty = 0xe000;
		osc[ch].duty = duty;

		channel[ch].vpos += channel[ch].vrate;
	}

	lights = 0;
	if(light[0]) {
		light[0]--;
		lights |= 0x02;
	}
	if(light[1]) {
		light[1]--;
		lights |= 0x10;
	}
	PORTC = lights;
}

void initresources() {
	u8 i;
	struct unpacker up;

	initup(&up, 0);
	for(i = 0; i < 16 + MAXTRACK; i++) {
		resources[i] = readchunk(&up, 13);
	}

	initup(&songup, resources[0]);
}

int main() {
	asm("cli");
	watchdogoff();
	CLKPR = 0x80;
	CLKPR = 0x80;

	DDRC = 0x12;
	DDRD = 0xff;

	PORTC = 0;

	timetoplay = 0;
	trackwait = 0;
	trackpos = 0;
	playsong = 1;
	songpos = 0;

	osc[0].volume = 0;
	channel[0].inum = 0;
	osc[1].volume = 0;
	channel[1].inum = 0;
	osc[2].volume = 0;
	channel[2].inum = 0;
	osc[3].volume = 0;
	channel[3].inum = 0;

	initresources();

	TCCR0A = 0x02;
	TCCR0B = 0x02;	// clkIO/8, so 1/8 MHz
	OCR0A = 62;//125; // 8 KHz

	TIMSK0 = 0x02;

	asm("sei");
	for(;;) {
		while(!timetoplay);

		timetoplay--;
		playroutine();
	}
}

/*
INTERRUPT(_VECTOR(14))		// called at 8 KHz
{
	u8 i;
	s16 acc;
	u8 newbit;

	PORTD = lastsample;

	newbit = 0;
	if(noiseseed & 0x80000000L) newbit ^= 1;
	if(noiseseed & 0x01000000L) newbit ^= 1;
	if(noiseseed & 0x00000040L) newbit ^= 1;
	if(noiseseed & 0x00000200L) newbit ^= 1;
	noiseseed = (noiseseed << 1) | newbit;

	if(callbackwait) {
		callbackwait--;
	} else {
		timetoplay++;
		callbackwait = 90 - 1;
	}

	acc = 0;
	for(i = 0; i < 4; i++) {
		s8 value; // [-32,31]

		switch(osc[i].waveform) {
			case WF_TRI:
				if(osc[i].phase < 0x8000) {
					value = -32 + (osc[i].phase >> 9);
				} else {
					value = 31 - ((osc[i].phase - 0x8000) >> 9);
				}
				break;
			case WF_SAW:
				value = -32 + (osc[i].phase >> 10);
				break;
			case WF_PUL:
				value = (osc[i].phase > osc[i].duty)? -32 : 31;
				break;
			case WF_NOI:
				value = (noiseseed & 63) - 32;
				break;
			default:
				value = 0;
				break;
		}
		osc[i].phase += osc[i].freq;

		acc += value * osc[i].volume; // rhs = [-8160,7905]
	}
	// acc [-32640,31620]
	lastsample = 128 + (acc >> 8);	// [1,251]
}
*/
