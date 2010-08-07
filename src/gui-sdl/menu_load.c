#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include <SDL_TouchUI.h>

extern void write_text(int x, int y, char* txt);
extern void blit_image(SDL_Surface* img, int x, int y);
extern SDL_Surface* display;
extern SDL_Surface* tmpSDLScreen;
extern SDL_Surface* pMenu_Surface;
extern SDL_Color text_color;

#define MAX_FILES 1024
extern char yol[];
extern char msg[];
extern char msg_status[];

int dirz (int parametre) {
	char launchDir[256];
	SDL_Event event;
	int getdir = 1;
    pMenu_Surface = SDL_LoadBMP("images/menu_load.bmp");
	int loadloopdone = 0;
	int num_of_files = 0;
	int menuSelection = 0;
	int q;
	int bas = 0;
	int ka = 0;
	int kb = 0;
	char **filez	= (char **)malloc(MAX_FILES*sizeof(char *));

	if (display == NULL) {
		gui_init();
	}

	int i;
	int paging = 18;
	DIR *d=opendir(yol);
	struct dirent *ep;

	getcwd(launchDir,256);

	if (d != NULL) {
		for(i=0;i<MAX_FILES;i++) {
			ep = readdir(d);
			if (ep == NULL) {
				break;
			} else {
				struct stat sstat;
				char *tmp=(char *)calloc(1,256);
				strcpy(tmp,launchDir);
				strcat(tmp,"/");
				strcat(tmp,ep->d_name);

				filez[i]=(char*)malloc(64);
				strncpy(filez[i],ep->d_name,64);
				num_of_files++;
				free(tmp);
			}
		}
		closedir(d);
	}
	if (num_of_files<18) {
		paging = num_of_files;
	}

	while (!loadloopdone) {
		while (SDL_PollEvent(&event)) {
			SDL_TUI_HandleEvent(&event);
			if (event.type == SDL_QUIT) {
				loadloopdone = 1;
			}
			if (event.type == SDL_JOYBUTTONDOWN) {
             			switch (event.jbutton.button) {
#if 0
					case GP2X_BUTTON_UP: menuSelection -= 1; break;
					case GP2X_BUTTON_DOWN: menuSelection += 1; break;
					case GP2X_BUTTON_A: ka = 1; break;
					case GP2X_BUTTON_B: kb = 1; break;
					case GP2X_BUTTON_SELECT: loadloopdone = 1; break;
#endif
				}
			}
      			if (event.type == SDL_KEYDOWN) {
    				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE:	loadloopdone = 1; break;
				 	case SDLK_UP:		menuSelection -= 1; break;
					case SDLK_DOWN:		menuSelection += 1; break;
					case SDLK_a:		ka = 1; break;
					case SDLK_b:		kb = 1; break;
					default: break;
				}
			}
		}
		if (parametre == 0) {
			if (ka == 1) {	//df1
				// Select Disk Image
				char *tmp=(char *)calloc(1,256);
				//strcpy(tmp,launchDir);
				strcat(tmp,"disks/");
				strcat(tmp,filez[menuSelection]);
				//strcpy(changed_prefs.floppyslots[1].df,tmp);
				write_log ("Old Disk Image: %s\n", changed_prefs.floppyslots[1].df);
				strncpy(changed_prefs.floppyslots[1].df, tmp, 255);
				write_log ("Selected Disk Image: %s\n", changed_prefs.floppyslots[1].df);
				free(tmp);

				loadloopdone = 1;
				ka = 0;
			}
			if (kb == 1) {  //df0;
				// Select Disk Image
				char *tmp=(char *)calloc(1,256);
				//strcpy(tmp,launchDir);
				strcat(tmp,"disks/");
				strcat(tmp,filez[menuSelection]);
				//strcpy(changed_prefs.floppyslots[0].df,tmp);
				write_log ("Old Disk Image: %s\n", changed_prefs.floppyslots[0].df);
				strncpy(changed_prefs.floppyslots[0].df, tmp, 255);
				write_log ("Selected Disk Image: %s\n", changed_prefs.floppyslots[0].df);
				free(tmp);

				loadloopdone = 1;
				kb = 0;
			}
		} else {
			if (kb == 1) {
				// Select KickStart ROM
				char *tmp=(char *)calloc(1,256);
				//strcpy(tmp,launchDir);
				strcat(tmp,"roms/");
				strcat(tmp,filez[menuSelection]);
				//strcpy(changed_prefs.romfile,tmp);
				write_log ("Old KickStart ROM: %s\n", changed_prefs.romfile);
				strncpy(changed_prefs.romfile, tmp, 255);
				write_log ("Selected KickStart ROM: %s\n", changed_prefs.romfile);
				free(tmp);

				loadloopdone = 1; 
			}
			ka = 0;
			kb = 0;
		}

		if (menuSelection < 0) { menuSelection = 0; }
		if (menuSelection >= num_of_files) { menuSelection = num_of_files-1; }
		if (menuSelection > (bas + paging -1)) { bas += 1; }
		if (menuSelection < bas) { bas -= 1; }
		if ((bas+paging) > num_of_files) { bas = (num_of_files - paging); }

	// background
		SDL_BlitSurface (pMenu_Surface,NULL,tmpSDLScreen,NULL);

	// texts
		int sira = 0;
		for (q=bas; q<(bas+paging); q++) {
			if (menuSelection == q) {
				text_color.r = 255; text_color.g = 100; text_color.b = 100;
			}
			write_text (10,25+(sira*10),filez[q]); //
			if (menuSelection == q) {
				text_color.r = 0; text_color.g = 0; text_color.b = 0;
			}
			sira++;
		}

		write_text (25,3,msg);
		write_text (15,228,msg_status);

		SDL_BlitSurface (tmpSDLScreen,NULL,display,NULL);
		SDL_TUI_UpdateAll();
		SDL_Flip(display);
	} //while done

	free(filez);
	pMenu_Surface = SDL_LoadBMP("images/menu.bmp");

	return 0;
}
