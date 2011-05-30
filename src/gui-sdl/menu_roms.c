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

#ifdef TOUCHUI
#include <SDL_TouchUI.h>
#endif

extern void write_text(int x, int y, char* txt);
extern void blit_image(SDL_Surface* img, int x, int y);
extern int pushdir (char *directory);
extern char *popdir (void);
extern int getdirstackcount (void);
extern void freedirstack (void);
extern int getdirectory (char **filez, char *directory, bool hide_parent);
extern SDL_Surface* display;
extern SDL_Surface* tmpSDLScreen;
extern SDL_Surface* pMenu_Surface;
extern SDL_Color text_color;

#define MAX_FILES 1024
extern char msg[];
extern char msg_status[];

int romz (void) {
	char subDir[MAX_PATH];
	char launchDir[MAX_PATH];
	SDL_Event event;
	int getdir = 1;
    pMenu_Surface = SDL_LoadBMP("images/menu_load.bmp");
	int loadloopdone = 0;
	int num_of_files = 0;
	int menuSelection = 0;
	int currLine;
	int bas = 0;
	int kb = 0;
	char **filez = (char **)malloc(MAX_FILES * sizeof(char *));

	if (display == NULL) {
		gui_init();
	}
	
	sprintf(msg, "%s", "Select Kickstart ROM");
	sprintf(msg_status, "%s", " ");

	getcwd(launchDir, MAX_PATH);

	int paging = 18;
	strcpy(subDir, "roms");
	
	num_of_files = getdirectory(filez, subDir, true);
	if (num_of_files < 18) {
		paging = num_of_files;
	}

	while (!loadloopdone) {
		while (SDL_PollEvent(&event)) {
#ifdef TOUCHUI
			SDL_TUI_HandleEvent(&event);
#endif
			if (event.type == SDL_QUIT) {
				loadloopdone = 1;
			}
#if GP2X
			if (event.type == SDL_JOYBUTTONDOWN) {
             	switch (event.jbutton.button) {
					case GP2X_BUTTON_UP: menuSelection -= 1; break;
					case GP2X_BUTTON_DOWN: menuSelection += 1; break;
					case GP2X_BUTTON_B: kb = 1; break;
					case GP2X_BUTTON_SELECT: loadloopdone = 1; break;
				}
			}
#endif
   			if (event.type == SDL_KEYDOWN) {
   				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: loadloopdone = 1; break;
				 	case SDLK_UP: menuSelection -= 1; break;
					case SDLK_DOWN: menuSelection += 1; break;
					case SDLK_RETURN: kb = 1; break;
					default: break;
				}
			}
		}
		if (kb == 1) {
			// Check if a directory was selected
			struct stat sstat;
			char *tmp = (char *)calloc(1, MAX_PATH);
			strcpy(tmp, subDir);
			strcat(tmp, "/");
			strcat(tmp, filez[menuSelection]);
			if (stat(tmp, &sstat) != -1) {
				if (sstat.st_mode & S_IFDIR) {
					// Select subdirectory
					if (strcasecmp(filez[menuSelection], "..") == 0) {
						// Back to parent
						char *tmp2 = NULL;
						tmp2 = popdir();
						if (tmp2) {
							menuSelection = 0;
							paging = 18;
							strcpy(subDir, tmp2);
							int i;
							bool hide_parent = false;
							if (getdirstackcount() == 0) {
								hide_parent = true;
							}
							for (i=0; i < num_of_files; i++) {
								free(filez[i]);
							}
							num_of_files = getdirectory(filez, subDir, hide_parent);
							if (num_of_files < 18) {
								paging = num_of_files;
							}
						}
						free(tmp2);
					} else {
						// go into subdirectory
						pushdir(subDir);
						menuSelection = 0;
						paging = 18;
						strcpy(subDir, tmp);
						int i;
						for (i=0; i < num_of_files; i++) {
							free(filez[i]);
						}
						num_of_files = getdirectory(filez, subDir, false);
						if (num_of_files < 18) {
							paging = num_of_files;
						}
					}
				} else {
					// Select kickstart rom
					write_log ("Old KickStart ROM: %s\n", changed_prefs.romfile);
					strncpy(changed_prefs.romfile, tmp, 255);
					write_log ("Selected KickStart ROM: %s\n", changed_prefs.romfile);
					loadloopdone = 1;
				}
			}
			free(tmp);
			kb = 0;
		}

		if (menuSelection < 0) { menuSelection = 0; }
		if (menuSelection >= num_of_files) { menuSelection = num_of_files-1; }
		if (menuSelection > (bas + paging -1)) { bas += 1; }
		if (menuSelection < bas) { bas -= 1; }
		if ((bas+paging) > num_of_files) { bas = (num_of_files - paging); }

		// background
		SDL_BlitSurface (pMenu_Surface, NULL, tmpSDLScreen, NULL);

		// texts
		int lineOffset = 0;
		for (currLine = bas; currLine < (bas + paging); currLine++) {
			if (menuSelection == currLine) {
				text_color.r = 255; text_color.g = 100; text_color.b = 100;
			}
			write_text (10, (lineOffset * 10) + 25, filez[currLine]);
			if (menuSelection == currLine) {
				text_color.r = 0; text_color.g = 0; text_color.b = 0;
			}
			lineOffset++;
		}

		write_text (25, 5, msg);
		write_text (15, 238, msg_status);

		SDL_BlitSurface (tmpSDLScreen,NULL,display,NULL);
#ifdef TOUCHUI
		SDL_TUI_UpdateAll();
#endif
		redraw_frame();
	} //while done

	free(filez);
	freedirstack();
	pMenu_Surface = SDL_LoadBMP("images/menu.bmp");

	return 0;
}
