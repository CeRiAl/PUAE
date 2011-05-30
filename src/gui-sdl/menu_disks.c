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

char disksSubDir[MAX_PATH];

int diskz (void);
void changeDisk (int drive);


int diskz (void) {
	SDL_Event event;
    pMenu_Surface = SDL_LoadBMP("images/menu_load.bmp");
	int drivesloopdone = 0;
	int menuSelection = 0;
	int currLine;
	int kright = 0;
	int kb = 0;
	char* drives[] = {"df0:", "df1:", "df2:", "df3:"};
	char *tmp;
	tmp = (char*)malloc(MAX_PATH);

	if (display == NULL) {
		gui_init();
	}
	
	sprintf(msg, "%s", "Select Disk Drive");
	sprintf(msg_status, "%s", "[RIGHT] to eject disk");

	while (!drivesloopdone) {
		while (SDL_PollEvent(&event)) {
#ifdef TOUCHUI
			SDL_TUI_HandleEvent(&event);
#endif
			if (event.type == SDL_QUIT) { drivesloopdone = 1; }
#if GP2X
			if (event.type == SDL_JOYBUTTONDOWN) {
				switch (event.jbutton.button) {
					case GP2X_BUTTON_UP: menuSelection--; break;
					case GP2X_BUTTON_DOWN: menuSelection++; break;
					case GP2X_BUTTON_RIGHT: kright = 1; break;
					case GP2X_BUTTON_SELECT: drivesloopdone = 1; break;
					case GP2X_BUTTON_B: kb =1; break;
				}
			}
#endif
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: drivesloopdone = 1; break;
				 	case SDLK_UP: menuSelection--; break;
					case SDLK_DOWN: menuSelection++; break;
					case SDLK_RIGHT: kright = 1; break;
					case SDLK_RETURN: kb = 1; break;
					default: break;
				}
			}
		}
		if (kright == 1) {
			//  eject diskimage
			disk_eject(menuSelection);
			kright = 0;
		}
		if (kb == 1) {
			// select disk image
			changeDisk(menuSelection);
			kb = 0;
		}
		
		if (menuSelection < 0) { menuSelection = 3; }
		if (menuSelection > 3) { menuSelection = 0; }

		// background
		SDL_BlitSurface(pMenu_Surface, NULL, tmpSDLScreen, NULL);

		// texts
		for (currLine = 0; currLine < 4; currLine++) {
			if (menuSelection == currLine) {
				text_color.r = 255; text_color.g = 100; text_color.b = 100;
			}
			write_text(10, (currLine*10) + 25, drives[currLine] );
			sprintf(tmp, "%s", changed_prefs.floppyslots[currLine].df);
			write_text(50, (currLine*10) + 25, tmp);
			if (menuSelection == currLine) {
				text_color.r = 0; text_color.g = 0; text_color.b = 0;
			}
		}

		write_text(25, 5, msg);
		write_text(15, 230, msg_status);

		SDL_BlitSurface(tmpSDLScreen, NULL, display, NULL);
#ifdef TOUCHUI
		SDL_TUI_UpdateAll();
#endif
		redraw_frame();
	} //while done

	freedirstack();
	strcpy(disksSubDir, "disks");
    pMenu_Surface = SDL_LoadBMP("images/menu.bmp");
	
	return 0;
}

void changeDisk (int drive) {
	char launchDir[MAX_PATH];
	SDL_Event event;
	int getdir = 1;
	bool hide_parent = false;
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
	
	sprintf(msg, "%s", "Select Disk Image");
	sprintf(msg_status, "%s", " ");

	getcwd(launchDir, MAX_PATH);
	
	int paging = 18;
	
	if (strcmp(disksSubDir, "\0") == 0) {
		strcpy(disksSubDir, "disks");
	}
	if (strcasecmp(disksSubDir, "disks") == 0) {
		hide_parent = true;
	}

	num_of_files = getdirectory(filez, disksSubDir, hide_parent);
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
			strcpy(tmp, disksSubDir);
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
							strcpy(disksSubDir, tmp2);
							int i;
							hide_parent = false;
							if (getdirstackcount() == 0) {
								hide_parent = true;
							}
							for (i=0; i < num_of_files; i++) {
								free(filez[i]);
							}
							num_of_files = getdirectory(filez, disksSubDir, hide_parent);
							if (num_of_files < 18) {
								paging = num_of_files;
							}
						}
						free(tmp2);
					} else {
						// go into subdirectory
						pushdir(disksSubDir);
						menuSelection = 0;
						paging = 18;
						strcpy(disksSubDir, tmp);
						int i;
						for (i=0; i < num_of_files; i++) {
							free(filez[i]);
						}
						num_of_files = getdirectory(filez, disksSubDir, false);
						if (num_of_files < 18) {
							paging = num_of_files;
						}
					}
				} else {
					// Select diskimage
					write_log ("Old Disk Image: %s\n", changed_prefs.floppyslots[drive].df);
					strncpy(changed_prefs.floppyslots[drive].df, tmp, 255);
					write_log ("Selected Disk Image: %s\n", changed_prefs.floppyslots[drive].df);
					loadloopdone = 1;
				}
			}
			free(tmp);
			kb = 0;
		}

		if (menuSelection < 0) { menuSelection = 0; }
		if (menuSelection >= num_of_files) { menuSelection = num_of_files-1; }
		if (menuSelection > (bas + paging - 1)) { bas += 1; }
		if (menuSelection < bas) { bas -= 1; }
		if ((bas + paging) > num_of_files) { bas = (num_of_files - paging); }

		// background
		SDL_BlitSurface (pMenu_Surface, NULL, tmpSDLScreen, NULL);

		// texts
		int lineOffset = 0;
		for (currLine = bas; currLine < (bas + paging); currLine++) {
			if (menuSelection == currLine) {
				text_color.r = 255; text_color.g = 100; text_color.b = 100;
			}
			write_text (10, (lineOffset*10) + 25, filez[currLine]);
			if (menuSelection == currLine) {
				text_color.r = 0; text_color.g = 0; text_color.b = 0;
			}
			lineOffset++;
		}

		write_text (25, 5, msg);
		write_text (15, 238, msg_status);

		SDL_BlitSurface (tmpSDLScreen, NULL, display, NULL);
#ifdef TOUCHUI
		SDL_TUI_UpdateAll();
#endif
		redraw_frame();
	} //while done

	free(filez);
	pMenu_Surface = SDL_LoadBMP("images/menu_load.bmp");

	return;
}
