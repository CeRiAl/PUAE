#include "SDL.h"
#include "SDL_image.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "menu.h"
#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "uae.h"

#include "gui.h"
#include "zfile.h"

#ifdef TOUCHUI
#include <SDL_TouchUI.h>
#endif

#define GUI_DEBUG
#ifdef GUI_DEBUG
	#define DEBUG_LOG write_log( "%s: ", __func__); write_log
#else
	#define DEBUG_LOG(...) do ; while(0)
#endif

#define VIDEO_FLAGS SDL_HWSURFACE
SDL_Surface* tmpSDLScreen = NULL;
extern SDL_Surface* display;

bool sdlGuiInitialized = false;

int menuSelection = 0;
char msg[50];
char msg_status[50];
char launchDir[MAX_PATH];

extern int diskz (void);
extern int romz (void);
extern int tweakz (void);
extern int prefz (void);
int soundVolume = 100;
extern int flashLED;
static int gui_available = 0;


void write_text (int x, int y, char* txt) {
	if (strlen(txt) > 0) {
		SDL_Surface* pText_Surface = TTF_RenderText_Solid(amiga_font, txt, text_color);

		rect.x = x;
		rect.y = y;
		rect.w = pText_Surface->w;
		rect.h = pText_Surface->h;

		SDL_BlitSurface(pText_Surface, NULL, tmpSDLScreen, &rect);
		SDL_FreeSurface(pText_Surface);
	}
}

void blit_image (SDL_Surface* img, int x, int y) {
	SDL_Rect dest;
   	dest.x = x;
   	dest.y = y;
   	SDL_BlitSurface(img, 0, tmpSDLScreen, &dest);
}

void drawMenuIcon (int ix, int iy, int mx, int my, SDL_Surface* img, int hangi) {
	int selection = 0;
	if (mx >= ix && mx <= ix + iconsizex) {
		if (my >= iy && my <= iy + iconsizey) {
			selection = 1;
		}
	}
	if (selection == 1) {
		SDL_SetAlpha(img, SDL_SRCALPHA, 100);
		menuSelection = hangi;
	} else {
		SDL_SetAlpha(img, SDL_SRCALPHA, 255);
	}
}

int gui_init (void) {
	if (display) {
		SDL_JoystickEventState(SDL_ENABLE);
		SDL_JoystickOpen(0);
		SDL_ShowCursor(SDL_DISABLE);
	  	TTF_Init();

		amiga_font = TTF_OpenFont("fonts/amiga4ever_pro2.ttf", 8);
		text_color.r = 0;
		text_color.g = 0;
		text_color.b = 0;

	    pMenu_Surface = SDL_LoadBMP("images/menu.bmp");
	    pMouse_Pointer = SDL_LoadBMP("images/mousep.bmp");
		SDL_SetColorKey(pMouse_Pointer, SDL_SRCCOLORKEY, SDL_MapRGB(pMouse_Pointer->format, 85, 170, 153));

		icon_expansion = SDL_LoadBMP("images/icon-expansion.bmp");
		icon_preferences = SDL_LoadBMP("images/icon-preferences.bmp");
		icon_keymaps = SDL_LoadBMP("images/icon-keymaps.bmp");
		icon_floppy	= SDL_LoadBMP("images/icon-floppy.bmp");
		icon_reset = SDL_LoadBMP("images/icon-reset.bmp");
		icon_storage = SDL_LoadBMP("images/icon-storage.bmp");
		icon_run = SDL_LoadBMP("images/icon-run.bmp");
		icon_exit = SDL_LoadBMP("images/icon-exit.bmp");
		icon_tweaks = SDL_LoadBMP("images/icon-tweaks.bmp");

		tmpSDLScreen = SDL_CreateRGBSurface(display->flags, display->w, display->h, display->format->BitsPerPixel, display->format->Rmask, display->format->Gmask, display->format->Bmask, display->format->Amask);
#ifdef TOUCHUI
		SDL_TUI_Init("sdl_touchui.xml", "keyboard-off");
#endif
		sdlGuiInitialized = true;
		return 1;
	}
	return 0;
}

void gui_exit (void) {
	if (0 != 1) {
    	SDL_FreeSurface(tmpSDLScreen);
    	SDL_FreeSurface(pMenu_Surface);
    	SDL_FreeSurface(pMouse_Pointer);
		SDL_FreeSurface(icon_expansion);
		SDL_FreeSurface(icon_preferences);
		SDL_FreeSurface(icon_keymaps);
		SDL_FreeSurface(icon_floppy);
		SDL_FreeSurface(icon_reset);
		SDL_FreeSurface(icon_storage);
		SDL_FreeSurface(icon_run);
		SDL_FreeSurface(icon_exit);
		SDL_FreeSurface(icon_tweaks);
	}
	SDL_Quit;
}

void gui_display (int shortcut) {
	SDL_Event event;

	int menu_exitcode = -1;
	int mainloopdone = 0;
	int mouse_x = 30;
	int mouse_y = 40;
	int kup = 0;
	int kdown = 0;
	int kleft = 0;
	int kright = 0;
	int ksel = 0;
	int iconpos_x = 0;
	int iconpos_y = 0;

	if (!sdlGuiInitialized) {
		gui_init();
	}

	getcwd(launchDir, MAX_PATH);

	while (!mainloopdone) {
		while (SDL_PollEvent(&event)) {
#ifdef TOUCHUI
			SDL_TUI_HandleEvent(&event);
#endif
			if (event.type == SDL_QUIT) {
				mainloopdone = 1;
			}
#if GP2X
			if (event.type == SDL_JOYBUTTONDOWN) {
           		switch (event.jbutton.button) {
                 	case GP2X_BUTTON_R: break;
					case GP2X_BUTTON_L: break;
					case GP2X_BUTTON_UP: kup = 1; break;
					case GP2X_BUTTON_DOWN: kdown = 1; break;
					case GP2X_BUTTON_LEFT: kleft = 1; break;
					case GP2X_BUTTON_RIGHT: kright = 1; break;
					case GP2X_BUTTON_CLICK: ksel = 1; break;
					case GP2X_BUTTON_B: ksel = 1; break;
					case GP2X_BUTTON_Y: break;
					case GP2X_BUTTON_START: mainloopdone = 1; break;
				}
			}
#endif
   			if (event.type == SDL_KEYDOWN) {
   				switch (event.key.keysym.sym) {
					case SDLK_ESCAPE: mainloopdone = 1; break;
				 	case SDLK_UP: kup = 1; break;
					case SDLK_DOWN: kdown = 1; break;
					case SDLK_LEFT: kleft = 1; break;
					case SDLK_RIGHT: kright = 1; break;
					case SDLK_RETURN: ksel = 1; break;
					default: break;
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				mouse_x += event.motion.xrel;
				mouse_y += event.motion.yrel;
			}
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (menuSelection == 0) {
					if (mouse_x >= 0 && mouse_x <= 20) {
						if (mouse_y >= 0 && mouse_y <= 20) {
							mainloopdone = 1;
						}
					}
				} else {
					ksel = 1; break;
				}
			}
		}
		if (ksel == 1) {
			if (menuSelection == menu_sel_expansion) {
				// select kickstart rom
				romz();
			}
			if (menuSelection == menu_sel_floppy) {
				// select diskimages
				diskz();
			}
			if (menuSelection == menu_sel_prefs) {
				// preferences
				prefz();
			}
			if (menuSelection == menu_sel_reset) {
				// reset amiga
				menu_exitcode = 2;
				mainloopdone = 1;
			}
			if (menuSelection == menu_sel_keymaps) {
				// keymaps
			}
			if (menuSelection == menu_sel_tweaks) {
				// tweaks
				tweakz();
			}
			if (menuSelection == menu_sel_storage) {
				// storage
			}
			if (menuSelection == menu_sel_run) {
				menu_exitcode = 1;
				mainloopdone = 1;
			}
			if (menuSelection == menu_sel_exit) {
				SDL_Quit();
#ifdef GP2X
				//remove mmuhack module
				system("/sbin/rmmod mmuhack");

				//menu
				chdir("/usr/gp2x");
				execl("/usr/gp2x/gp2xmenu", "/usr/gp2x/gp2xmenu", NULL);
#endif
				exit(0);
			}
			ksel = 0;
		}

		// background
		SDL_BlitSurface(pMenu_Surface,NULL,tmpSDLScreen,NULL);

		// icons
		iconpos_x = 10;
		iconpos_y = 23;

		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y,icon_floppy, menu_sel_floppy);
		blit_image(icon_floppy, iconpos_x, iconpos_y);

		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_preferences, menu_sel_prefs);
		blit_image(icon_preferences, iconpos_x, iconpos_y);

#ifdef GP2X
		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_tweaks, menu_sel_tweaks);
		blit_image(icon_tweaks, iconpos_x, iconpos_y);
#endif

		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_keymaps, menu_sel_keymaps);
		blit_image(icon_keymaps, iconpos_x, iconpos_y);

		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_expansion, menu_sel_expansion);
		blit_image(icon_expansion, iconpos_x, iconpos_y);

#ifdef GP2X
		iconpos_x = 10;
		iconpos_y = 93;
#else
		iconpos_x += iconsizex + bosluk;
#endif
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y,icon_storage, menu_sel_storage);
		blit_image(icon_storage, iconpos_x, iconpos_y);

#ifndef GP2X
		iconpos_x = 10;
		iconpos_y = 93;
#else
		iconpos_x += iconsizex + bosluk;
#endif
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_reset, menu_sel_reset);
		blit_image(icon_reset, iconpos_x, iconpos_y);

		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_run, menu_sel_run);
		blit_image(icon_run, iconpos_x, iconpos_y);

		iconpos_x += iconsizex + bosluk;
		drawMenuIcon(iconpos_x,iconpos_y,mouse_x,mouse_y, icon_exit, menu_sel_exit);
		blit_image(icon_exit, iconpos_x, iconpos_y);
		// texts

		char tmpMsg[50];
		sprintf(tmpMsg, "PUAE %d.%d.%d", UAEMAJOR, UAEMINOR, UAESUBREV);
		write_text(25, 5, tmpMsg);

		// mouse pointer ------------------------------
		if (kleft == 1) {
			mouse_x -= (iconsizex + bosluk);
			kleft = 0;
		}
		if (kright == 1) {
			mouse_x += (iconsizex + bosluk);
			kright = 0;
		}
		if (kup == 1) {
			mouse_y -= (iconsizey + bosluk);
			kup = 0;
		}
		if (kdown == 1) {
			kdown = 0;
			mouse_y += (iconsizey + bosluk);
		}

		if (mouse_x < 1) { mouse_x = 1; }
		if (mouse_y < 1) { mouse_y = 1; }
		if (mouse_x > 320) { mouse_x = 320; }
		if (mouse_y > 240) { mouse_y = 240; }
		rect.x = mouse_x;
		rect.y = mouse_y;
		//rect.w = pMouse_Pointer->w;
		//rect.h = pMouse_Pointer->h;
		SDL_BlitSurface(pMouse_Pointer,NULL,tmpSDLScreen,&rect);
		// mouse pointer-end

		SDL_BlitSurface(tmpSDLScreen,NULL,display,NULL);
#ifdef TOUCHUI
		SDL_TUI_UpdateAll();
#endif
		redraw_frame();
	} //while done

}

void gui_handle_events (void) {
    if (!gui_available)
		return;

#if GP2X
    while (comm_pipe_has_data (&from_gui_pipe)) {
		int cmd = read_comm_pipe_int_blocking (&from_gui_pipe);

	switch (cmd) {
	    case UAECMD_EJECTDISK: {
			int n = read_comm_pipe_int_blocking(&from_gui_pipe);
			uae_sem_wait(&gui_sem);
			changed_prefs.floppyslots[n].df[0] = '\0';
			uae_sem_post(&gui_sem);
			if (pause_uae) {
				/* When UAE is running it will notify the GUI when a disk has been inserted
				* or removed itself. When UAE is paused, however, we need to do this ourselves
				* or the change won't be realized in the GUI until UAE is resumed */
//				write_comm_pipe_int(&to_gui_pipe, n, 1);
			}
			break;
		   }
	    case UAECMD_INSERTDISK: {
			int n = read_comm_pipe_int_blocking(&from_gui_pipe);
			uae_sem_wait(&gui_sem);
			strncpy(changed_prefs.floppyslots[n].df, new_disk_string[n], 255);
			free(new_disk_string[n]);
			new_disk_string[n] = 0;
			changed_prefs.floppyslots[n].df[255] = '\0';
			uae_sem_post(&gui_sem);
			if (pause_uae) {
				/* When UAE is running it will notify the GUI when a disk has been inserted
				* or removed itself. When UAE is paused, however, we need to do this ourselves
				* or the change won't be realized in the GUI until UAE is resumed */
//				write_comm_pipe_int(&to_gui_pipe, GUICMD_DISKCHANGE, 0);
//				write_comm_pipe_int(&to_gui_pipe, n, 1);
			}
			break;
		    }
	    case UAECMD_RESET:
			uae_reset(0);
			break;
#ifdef DEBUGGER
	    case UAECMD_DEBUG:
			activate_debugger();
			break;
#endif
	    case UAECMD_QUIT:
			uae_quit();
			break;
	    case UAECMD_PAUSE:
			pause_uae = 1;
			uae_pause();
			break;
	    case UAECMD_RESUME:
			pause_uae = 0;
			uae_resume();
			break;
	    case UAECMD_SAVE_CONFIG:
			uae_sem_wait(&gui_sem);
			//uae_save_config ();
			uae_sem_post(&gui_sem);
			break;
	    case UAECMD_SELECT_ROM:
			uae_sem_wait(&gui_sem);
			strncpy(changed_prefs.romfile, gui_romname, 255);
			changed_prefs.romfile[255] = '\0';
			free(gui_romname);
			uae_sem_post(&gui_sem);
			break;
	    case UAECMD_SELECT_KEY:
			uae_sem_wait(&gui_sem);
			strncpy(changed_prefs.keyfile, gui_keyname, 255);
			changed_prefs.keyfile[255] = '\0';
			free(gui_keyname);
			uae_sem_post(&gui_sem);
			break;
	    case UAECMD_SAVESTATE_LOAD:
			uae_sem_wait(&gui_sem);
			savestate_initsave(gui_sstate_name, 0, 0);
			savestate_state = STATE_DORESTORE;
            write_log("Restoring state from '%s'...\n", gui_sstate_name);
			uae_sem_post(&gui_sem);
			break;
	    case UAECMD_SAVESTATE_SAVE:
			uae_sem_wait(&gui_sem);
			savestate_initsave(gui_sstate_name, 0, 0);
			save_state(gui_sstate_name, "puae");
            write_log("Saved state to '%s'...\n", gui_sstate_name);
			uae_sem_post(&gui_sem);
			break;
/*	    case UAECMD_START:
			uae_start();
			break;
		case UAECMD_STOP:
			uae_stop();
			break;*/
	}
    }
#endif
}

void gui_message (const char *format, ...) {
   char msg[2048];
   va_list parms;

   va_start(parms, format);
   vsprintf(msg, format, parms);
   va_end(parms);

   write_log(msg);
}

void gui_fps (int fps, int idle) {
    gui_data.fps  = fps;
    gui_data.idle = idle;
}

char **dirstack = NULL;
int dirstackcount = 0;
int pushdir (char *directory) {
	void **_tmp = (char**)realloc(dirstack, (dirstackcount+1) * sizeof(char *));
	if (!_tmp) { 
		write_log("ERROR: Couldn't realloc more memory for dirstack!\n");
		return(-1); 
	}
	dirstack = (char**)_tmp;
	dirstack[dirstackcount] = (char*)malloc(MAX_PATH);
	if (!dirstack[dirstackcount]) {
		write_log("ERROR: Couldn't malloc memory for dirstack entry!\n");
		return(-1); 
	}
	strncpy(dirstack[dirstackcount], directory, MAX_PATH);
	dirstackcount++;
	
//	write_log( "added '%s', now at %d\n", dirstack[dirstackcount-1], dirstackcount);
	return dirstackcount;
}

char *popdir (void) {
	if ((dirstackcount > 0) && (dirstack != NULL)) {
		char* _tmp = (char*)malloc(MAX_PATH);
		dirstackcount--;
		strncpy(_tmp, dirstack[dirstackcount], MAX_PATH);
		free(dirstack[dirstackcount]);
		
//		write_log( "returning '%s', now at %d\n", _tmp, dirstackcount);
		return _tmp;
	} else {
		return NULL;
	}
}

int getdirstackcount (void) {
	return dirstackcount;
}

void freedirstack (void) {
	if (dirstack != NULL) {
		int i = 0;
		for (i = dirstackcount; i > 0; i--) {
			free(dirstack[i]);
			dirstackcount--;
		}
		free(dirstack);
	}
}

#define MAX_FILES 1024
int getdirectory (char **filez, char *directory, bool hide_parent) {
	int i;
	char launchDir[MAX_PATH];

	int num_of_files = 0;
	DIR *d = opendir(directory);
	struct dirent *ep;
	getcwd(launchDir, MAX_PATH);
	
	if (d != NULL) {
		for (i = 0; i < MAX_FILES; i++) {
			ep = readdir(d);
			if (ep == NULL) {
				break;
			} else {
				struct stat sstat;
				char *tmp = (char *)calloc(1, MAX_PATH);
				strcpy(tmp, directory);
				strcat(tmp, "/");
				strcat(tmp, ep->d_name);
				
				if ( (strcasecmp(ep->d_name, ".") != 0) ) {
					if ( (!hide_parent) || (strcasecmp(ep->d_name, "..") != 0) ) {
						filez[num_of_files] = (char*)malloc(64);
						strncpy(filez[num_of_files], ep->d_name, 64);
						num_of_files++;
					}
				}
				free(tmp);
			}
		}
		closedir(d);
	}
	
	return num_of_files;
}

int gui_update (void) {	return 0; }
void gui_flicker_led (int led, int unitnum, int status) {}
void gui_led (int led, int on) {}
void gui_filename (int num, const char *name) {}
void gui_disk_image_change (int unitnum, const TCHAR *name, bool writeprotected) {}
void gui_lock (void) {}
void gui_unlock (void) {}
void gui_gameport_button_change (int port, int button, int onoff) {}
void gui_gameport_axis_change (int port, int axis, int state, int max) {}
