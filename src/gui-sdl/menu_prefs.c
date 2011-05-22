#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "SDL.h"
#include <stdlib.h>

#ifdef TOUCHUI
#include <SDL_TouchUI.h>
#endif

extern void write_text(int x, int y, char* txt);
extern void blit_image(SDL_Surface* img, int x, int y);
extern SDL_Surface* display;
extern SDL_Surface* tmpSDLScreen;
extern SDL_Surface* pMenu_Surface;
extern SDL_Color text_color;
extern char msg[50];
extern char msg_status[50];

int prefz (int parametre) {
	SDL_Event event;

	if (display == NULL) {
		gui_init();
	}

    pMenu_Surface = SDL_LoadBMP("images/menu_tweak.bmp");
	int prefsloopdone = 0;
	int kup = 0;
	int kdown = 0;
	int kleft = 0;
	int kright = 0;
	int menuSelection = 0;
	int deger;
	int q;
	int w;

	char* prefs[]	= {	"CPU",
				"CPU Speed",
				"Chipset",
				"Chip",
				"Fast",
				"Bogo",
				"Sound",
				"Frame Skip",
				"Floppy Speed"	};

	char* p_cpu[]	= {"68000", "68010", "68020", "68020/68881", "68ec020", "68ec020/68881"};	//5
	char* p_speed[]	= {"max","real"};								//20
	char* p_chip[]	= {"OCS", "ECS (Agnus)", "ECS (Denise)", "ECS", "AGA"};				//4
	char* p_sound[]	= {"Off", "Off (emulated)", "On", "On (perfect)"};				//3
	char* p_frame[]	= {"0","1","2","3"};								//3
	char* p_ram[]	= {"0","512","1024"};								//2
	char* p_floppy[]= {"0","100","200","300"};							//3
	int defaults[]	= {0,0,0,0,0,0,0,0};

//	defaults[0] = changed_prefs.cpu_level;
	defaults[0] = changed_prefs.cpu_model;
	if (changed_prefs.address_space_24 != 0) {
//		if (changed_prefs.cpu_level == 2) { defaults[0] = 4; }
//		if (changed_prefs.cpu_level == 3) { defaults[0] = 5; }
		if (changed_prefs.cpu_model == 2) { defaults[0] = 4; }
		if (changed_prefs.cpu_model == 3) { defaults[0] = 5; }
	}
	defaults[1] = changed_prefs.m68k_speed;
	defaults[2] = changed_prefs.chipset_mask;
	defaults[3] = changed_prefs.chipmem_size;
	defaults[4] = changed_prefs.fastmem_size;
	defaults[5] = changed_prefs.bogomem_size;
	defaults[6] = changed_prefs.produce_sound;
	defaults[7] = changed_prefs.gfx_framerate;
	defaults[8] = changed_prefs.floppy_speed;

	char *tmp;
	tmp=(char*)malloc(6);

	while (!prefsloopdone) {
		while (SDL_PollEvent(&event)) {
#ifdef TOUCHUI
			SDL_TUI_HandleEvent(&event);
#endif
			if (event.type == SDL_QUIT) {
				prefsloopdone = 1;
			}
			if (event.type == SDL_JOYBUTTONDOWN) {
             			switch (event.jbutton.button) {
#if 0
					case GP2X_BUTTON_UP: menuSelection--; break;
					case GP2X_BUTTON_DOWN: menuSelection++; break;
					case GP2X_BUTTON_LEFT: kleft = 1; break;
					case GP2X_BUTTON_RIGHT: kright = 1; break;
					case GP2X_BUTTON_SELECT: prefsloopdone = 1; break;
					case GP2X_BUTTON_B: prefsloopdone = 1; break;
#endif
				}
			}
      			if (event.type == SDL_KEYDOWN) {
    				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	prefsloopdone = 1; break;
				 	case SDLK_UP:		menuSelection--; break;
					case SDLK_DOWN:		menuSelection++; break;
					case SDLK_LEFT:		kleft = 1; break;
					case SDLK_RIGHT:	kright = 1; break;
					case SDLK_b:		prefsloopdone = 1; break;
					default: break;
				}
			}
		}
		if (kleft == 1) {
			defaults[menuSelection]--;
			kleft = 0;

			if (menuSelection == 1) { 
				//cpu_speed_change = 1; 
			}
			if (menuSelection == 6) { 
				//snd_change = 1; 
			}
			if (menuSelection == 7) { 
				//gfx_frameskip_change = 1; 
			}
		}
		if (kright == 1) {
			defaults[menuSelection]++;
			kright = 0;

			if (menuSelection == 1) { 
				//cpu_speed_change = 1; 
			}
			if (menuSelection == 6) { 
				//snd_change = 1; 
			}
			if (menuSelection == 7) { 
				//gfx_frameskip_change = 1; 
			}
		}

		if (defaults[0] < 0) defaults[0] = 5;	//cpu
		if (defaults[1] < -1) defaults[1] = 20; //speed
		if (defaults[2] < 0) defaults[2] = 4;	//chipset
		if (defaults[3] < 0) defaults[3] = 2;	//chip
		if (defaults[4] < 0) defaults[4] = 2;	//slow
		if (defaults[5] < 0) defaults[5] = 2;	//fast
		if (defaults[6] < 0) defaults[6] = 3;	//sound
		if (defaults[7] < 0) defaults[7] = 3;	//frameskip
		if (defaults[8] < 0) defaults[8] = 3;	//floppy

		if (defaults[0] > 5) defaults[0] = 0;	//cpu
		if (defaults[1] > 20) defaults[1] = -1;	//speed
		if (defaults[2] > 4) defaults[2] = 0;	//chipset
		if (defaults[3] > 2) defaults[3] = 0;	//chip
		if (defaults[4] > 2) defaults[4] = 0;	//slow
		if (defaults[5] > 2) defaults[5] = 0;	//fast
		if (defaults[6] > 3) defaults[6] = 0;	//sound
		if (defaults[7] > 3) defaults[7] = 0;	//frameskip
		if (defaults[8] > 3) defaults[8] = 0;	//floppy

		if (menuSelection < 0) { menuSelection = 8; }
		if (menuSelection > 8) { menuSelection = 0; }
	// background
		SDL_BlitSurface (pMenu_Surface,NULL,tmpSDLScreen,NULL);

	// texts
		int sira = 0;
		int skipper = 0;
		for (q=0; q<9; q++) {
			if (menuSelection == q) {
				text_color.r = 150; text_color.g = 50; text_color.b = 50;
			}
			write_text (10,skipper+25+(sira*10),prefs[q]); //

			if (q == 0) {	write_text (130, skipper+25+(sira*10), p_cpu[defaults[q]]); }
			if (q == 1) {
				if (deger > 0) {
					sprintf(tmp,"%d",defaults[q]-1);
				} else {
					sprintf(tmp,"%d",p_speed[defaults[q]]);
				}
				write_text (130, skipper+25+(sira*10), tmp); 
			}
			if (q == 2) {	write_text (130, skipper+25+(sira*10), p_chip[defaults[q]]); }
			if (q > 2 && q < 6) {
				if (defaults[q] == 0) { deger = 0; }
				if (defaults[q] == 1) { deger = 512; }
				if (defaults[q] == 2) { deger = 1024; }

				sprintf(tmp,"%d",deger);
				write_text (130, skipper+25+(sira*10), tmp);
			}
			if (q == 6) {	write_text (130, skipper+25+(sira*10), p_sound[defaults[q]]); }
			if (q == 7) {	write_text (130, skipper+25+(sira*10), p_frame[defaults[q]]); }
			text_color.r = 0; text_color.g = 0; text_color.b = 0;
			sira++;
		}

		write_text (25,6,msg); //
		write_text (25,240,msg_status); //

		SDL_BlitSurface (tmpSDLScreen,NULL,display,NULL);
#ifdef TOUCHUI
		SDL_TUI_UpdateAll();
#endif
		redraw_frame();
	} //while done
/*
	if (defaults[0] == 4) { }
	if (defaults[0] == 5) { }
	defaults[1]--;
*/
    	pMenu_Surface = SDL_LoadBMP("images/menu.bmp");
	return 0;
}
