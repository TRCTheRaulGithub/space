#include "objet_space.h"  // Space Plugin For Winamp

// fonctions 'membres' 
void config(winampVisModule *this_mod);			// fonction du bouton dialogue de configuration 
int init(winampVisModule *this_mod);			// initialisation du (des) module(s)
int render(winampVisModule *this_mod);			// rendu pour le(s) module(s)
void quit(winampVisModule *this_mod);			// deinitialisation du (des) module(s)

// retourne le module selon which
winampVisModule* getModule(int which);

// procédures windows
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// variables globales
static Space*		MyPlug = NULL;  // Juste une instance d'objet Space
char szAppName[] = "Space";			// et le nom de l'application

#ifdef _DEBUG
FILE* debugfile = NULL;
#endif

// En-tête du module , incluant la version, la description, et l'adresse de la fonction de récupération du module choisi
winampVisHeader hdr = { VIS_HDRVER, "Space Plugin version 0.9 -  Pascal", getModule };

// module 
winampVisModule module =
{
	"Space",
	NULL,	// hwndParent  : fenêtre parent : modifié par Winamp
	NULL,	// hDllInstance  : instance du DLL : --------------
	0,		// sRate : fréquence du sample  :  -------------
	1,		// nCH : nombre canaux (1: mono, 2: stéréo) : --------------
	0,		// latente (ms) : chépa tro ske ca change
	0,		// delai (ms) entre 2 appels de la routine de rendu (fps=1000/delai)
	1,		// spectrumNch : combien de canaux de spectrum veut-on ?
	1,		// waveformNch : combien de canaux de waveform veut-on ?
	{ 0, },	// spectrumData : données spectrum : rempli par winamp selon demande par Nch
	{ 0, },	// waveformData : données waveform : rempli par winamp selon demande par Nch
	config, // fonction implémentant le bouton dialogue configuration sous Winamp pour ce module
	init,   // fonction d'initialisation du module
	render, // fonction de rendu visuel du module
	quit	// fonction déinitialisant le module (termine quoi !)
};


// C'est le seul symbole exporté : retour de notre en-tête principal.
// si on compile en C++, l'externe "C" { est nécessaire
extern "C" {
	__declspec( dllexport ) winampVisHeader* winampVisGetHeader() {
	// retourne le pointeur de l'entête du plugin : hdr
		return &hdr;
	}
}

// routine getmodule (obtention de module) de notre en-tête principal. doit retourner NULL si un module invalid est demandé,
// sinon on retourne le module correspond (dependant de 'which').
winampVisModule* getModule(int which) {
	switch (which) {
	case 0: return &module;  // il n'y en a qu'un seul !
	default: return NULL;  
	}
}

// configuration, ici c plutôt un "about" car la config se fait dans le plugin même
void config(winampVisModule *this_mod) {
	DialogBox(module.hDllInstance, MAKEINTRESOURCE(IDD_ABOUT), this_mod->hwndParent,(DLGPROC)AboutDlgProc);
}

// initialisation. Enregistrement de notre classe window, création de notre fenêtre, etc.
// doit retourner 0 si succès, sinon 1.
int init(winampVisModule *this_mod)
{

#ifdef _DEBUG
	debugfile = fopen("c:\\progra~1\\winamp\\plugins\\space\\space.log", "w");
#endif
	
	// Enregistrement de notre classe window
	HRESULT ddrval;
	WNDCLASS wc;
	memset(&wc,0,sizeof(wc));				// je fous tout le contenu de la structure à zéro et je rédéfini l'utile
	wc.lpfnWndProc = WndProc;				// la procédure de gestion de la fenêtre ( et de ses messages)
	wc.hInstance = this_mod->hDllInstance;		// hInstance du DLL  (beuh ?)
	wc.lpszClassName = szAppName;			// le nom de notre classe window
	
	if (!RegisterClass(&wc)) 
	{
		UnregisterClass(szAppName, this_mod->hDllInstance); // et enfin désenregistrement de la classe window
		if (!RegisterClass(&wc))   // si vraiment ça veut pas, on laisse tomber ...
		{ MessageBox(this_mod->hwndParent,"Erreur d'enregistrement de la classe window","dommage ...",MB_OK); return 1;	}
	}
	
	// création du plugin
	MyPlug = new Space();
		
	//initialisation des paramètres avant initialisation directdraw (pour rotoblit)
	ddrval = MyPlug->init(MyPlug->Version);

	// initialisation du mode video
	if(MyPlug->InitViewMode(MyPlug->ViewMode)) {
		MyPlug->KillViewMode(MyPlug->ViewMode); // si ça foire ...
		MyPlug->XWin = 50;	// ...on met les valeurs par défaut et on essai encore
		MyPlug->YWin = 50;
		MyPlug->WidthWin = 320;
		MyPlug->HeightWin = 200;
		MyPlug->ViewMode = WINDOWED;		
		MyPlug->ResMode = AUTORES;
		if(MyPlug->InitViewMode(MyPlug->ViewMode)) { // là c vraiment foutu !
			MyPlug->KillViewMode(MyPlug->ViewMode);
			/*return MyPlug->Abandon(1, "Impossible initialiser le mode d'affichage");*/
			MyPlug->Abandon(1, "Impossible initialiser le mode d'affichage");
			quit(this_mod);
		}
	}

	return ddrval;
}	



// fonction de rendu du module, doit retourner 0 si succès, 1 si on doit arrêter la visualisation 
int render(winampVisModule *this_mod)
{
	static ULONG old_time = 0;
	static ULONG pair = 0;


#ifdef _DEBUG
	RECT RectClWnd;
	RECT RectWnd;
	bool changed = FALSE;
	static int old_winplugwidth = 0, old_winplugheight = 0, old_clwidth = 0, old_clheight = 0, old_winwidth = 0, old_winheight = 0,
	old_plugmodwidth = 0, old_plugmodheight = 0, old_xres = 0, old_yres = 0, old_backwidth = 0, old_backheight = 0;

	GetClientRect(MyPlug->hMainWnd, &RectClWnd);
	GetWindowRect(MyPlug->hMainWnd, &RectWnd);

	if( (old_winplugwidth != MyPlug->WidthWin)||(old_winplugheight != MyPlug->HeightWin) ) {
		if(debugfile) fprintf(debugfile, "-PlugWinRect: %d X %d -", MyPlug->WidthWin, MyPlug->HeightWin);
		old_winplugwidth = MyPlug->WidthWin; old_winplugheight = MyPlug->HeightWin; 
		changed = TRUE;
	}

	if( (old_clwidth != RectClWnd.right)||(old_clheight != RectClWnd.bottom) ) {
		if(debugfile) fprintf(debugfile, "ClRect: %d X %d -", RectClWnd.right, RectClWnd.bottom);
		old_clwidth = RectClWnd.right; old_clheight = RectClWnd.bottom; 
		changed = TRUE;
	}

	if( (old_winwidth != (RectWnd.right - RectWnd.left + 1) )||(old_winheight != (RectWnd.bottom - RectWnd.top + 1) ) ) {

		if(debugfile) fprintf(debugfile, "WinRect: %d X %d -", RectWnd.right - RectWnd.left + 1, RectWnd.bottom - RectWnd.top + 1);
		old_winwidth = RectWnd.right - RectWnd.left + 1; old_winheight = RectWnd.bottom - RectWnd.top + 1; 
		changed = TRUE;
	}
	
	if( (old_plugmodwidth != MyPlug->WidthMode)||(old_plugmodheight != MyPlug->HeightMode) ) {
		if(debugfile) fprintf(debugfile, "PlugMode: %d X %d -", MyPlug->WidthMode, MyPlug->HeightMode);
		old_plugmodwidth = MyPlug->WidthMode; old_plugmodheight = MyPlug->HeightMode; 
		changed = TRUE;
	}

	if( (old_xres != MyPlug->XRes)||(old_yres != MyPlug->YRes) ) {
		if(debugfile) fprintf(debugfile, "XYRes: %d X %d -", MyPlug->XRes, MyPlug->YRes);
		old_xres = MyPlug->XRes; old_yres = MyPlug->YRes; 
		changed = TRUE;
	}

	if( (old_backwidth != MyPlug->WidthBack)||(old_backheight != MyPlug->HeightBack) ) {
		if(debugfile) fprintf(debugfile, "PlugBack: %d X %d -", MyPlug->WidthBack, MyPlug->HeightBack);
		old_backwidth = MyPlug->WidthBack; old_backheight = MyPlug->HeightBack; 
		changed = TRUE;
	}

	if( (debugfile)&&(changed) ) fprintf(debugfile, "\n");

	/**
	if( (MyPlug->ViewMode == WINDOWED)||(MyPlug->ViewMode == DOCKED) ) {
		int width, height;
		if(MyPlug->ViewMode == WINDOWED) {width = MyPlug->WidthWin; height = MyPlug->HeightWin; }
		else {	// peut-pas être toujours bon ...
			RECT r;
			GetClientRect(MyPlug->hMainWnd, &r);
			width = r.right; height = r.bottom;
		}

		// des vérifs ...
		if(MyPlug->ResMode == AUTORES) {
			assert(width == MyPlug->XRes);
			assert(height == MyPlug->YRes);
			if(MyPlug->blitrotozoom) assert(!(width%4));
		} else if(MyPlug->ResMode == AUTODEMIRES) {
			assert(width == (MyPlug->XRes<<1));
			assert(height == (MyPlug->YRes<<1));
			if(MyPlug->blitrotozoom) assert(!(width%8));
			else assert(!(width%2));
			assert(!(height%2));
		} else {
			if(width < MyPlug->DefXRes) {
				assert(width == MyPlug->XRes);
				if(MyPlug->blitrotozoom) assert(!(width%4));
			}
			if(width < MyPlug->DefYRes) assert(height == MyPlug->YRes);
		}			
	}
	/**/

#endif

	if(MyPlug->Speed != SPEEDMAX) {
		if(MyPlug->Speed == SPEED20) while( ULONG(GetTickCount() - old_time) < 50) Sleep(1);
		else if(MyPlug->Speed == SPEED30) {
			if(pair < 3) {	// si synchro < 3
				while( ULONG(GetTickCount() - old_time) < 33) Sleep(1);
				//while( (GetTickCount() - old_time) < 31) Sleep(1);
				pair++;
			}
			else {	// synchro 30 FPS 3ème
				while( ULONG(GetTickCount() - old_time) < 34) Sleep(1);
				//while( (GetTickCount() - old_time) < 32) Sleep(1);
				pair = 0;
			}
		}
		else if(MyPlug->Speed == SPEED45) {
			if(pair < 4) {
				while( ULONG(GetTickCount() - old_time) < 22) Sleep(1);
				//while( (GetTickCount() - old_time) < 20) Sleep(1);
				pair++;
			}
			else {	// synchro 45 FPS 4ème
				while( ULONG(GetTickCount() - old_time) < 23) Sleep(1);
				//while( (GetTickCount() - old_time) < 21) Sleep(1);
				pair = 0;
			}
		}
		else {	// alors il est à 60 FPS
			pair^=1;
			if(!pair) while( ULONG(GetTickCount() - old_time) < 16) Sleep(1);
			else while( ULONG(GetTickCount() - old_time) < 17) Sleep(1);
			//if(!pair) while( (GetTickCount() - old_time) < 14) Sleep(1);
			//else while( (GetTickCount() - old_time) < 15) Sleep(1);
		}
	
		old_time = GetTickCount();
		// et on sauvegarde le temps courant
	}

	// on calcul la frame
	if(MyPlug->CalculRender()) { return 1; }
	// et on l'affiche
	if(MyPlug->DisplayRender()) { return 1; }
	
	return 0;
}

// libération des données (détruction la fenêtre, désenregistre la classe window...)
void quit(winampVisModule *this_mod)  
{
	delete MyPlug; // destruction du plugin (cela détruit également la fenêtre)

#ifdef _DEBUG
	if(debugfile) fclose(debugfile);
#endif

	if(!(UnregisterClass(szAppName, this_mod->hDllInstance))) // et enfin désenregistrement de la classe window
		MyPlug->Abandon(GetLastError(), "Désenregistrement de la classe:");
}


// procédure window pour notre fenêtre
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(MyPlug)	// les messages envoyés avant la création du plugin ne doivent pas être traités
	switch (message)
	{
		case WM_DESTROY: PostQuitMessage(0); return 0; // on dit à la fenêtre qu'un quit est demandé avec le retour de 0
		case WM_CLOSE: PostQuitMessage(0); return 0; // on dit à la fenêtre qu'un quit est demandé avec le retour de 0
		case WM_KEYDOWN: // on capte quelques touches du clavier et on passe 
			// les autres à la fenêtre Winamp principale (pour leur gestion)
			if((wParam == VK_ESCAPE)&&(MyPlug->ViewMode == FULLSCREEN)) { PostQuitMessage(0); return 0; }
			if(wParam == VK_F4) MyPlug->ShowFPS = (MyPlug->ShowFPS)?FALSE:TRUE;
		
		case WM_KEYUP: 	PostMessage(module.hwndParent,message,wParam,lParam); return 0;
		case WM_MOVING:
			if ( (MyPlug->ViewMode == WINDOWED)&&(MyPlug->DDActive()) )	render(&module);  // pour réactualiser la frame
			return 0;
		case WM_MOVE:
			if ( (MyPlug->ViewMode == WINDOWED)&&(MyPlug->DDActive()) )
			{	// on récupère XWin et YWin pour la configuration
				MyPlug->XWin = (int)(short) LOWORD(lParam);    // horizontal position 
				MyPlug->YWin = (int)(short) HIWORD(lParam);    // vertical position 
				render(&module);  // pour réactualiser la frame
			}
			return 0;
		case WM_SIZING: // possible seulement en mode fenêtré
			if(MyPlug->ViewMode == WINDOWED) {
				char ch[50];
				RECT RectClWnd;

				GetClientRect(MyPlug->hMainWnd, &RectClWnd);
				wsprintf(ch, "%s (%d X %d)", module.description, MyPlug->WidthWin = RectClWnd.right, MyPlug->HeightWin = RectClWnd.bottom);
				SetWindowText(MyPlug->hMainWnd, ch);

				MyPlug->NoDisplayTransform = 1;
				render(&module);  // pour réactualiser la frame
/*
//				int reste;
				RECT* RectWnd;
				HRESULT hret;

				RectWnd = (LPRECT)lParam;
				
				
				/*switch(MyPlug->ResMode) {
					case AUTORES:
						reste = RectClWnd.right%4;
						MyPlug->HeightWin = RectClWnd.bottom;
						break;	// pour avoir un width multiple de 4
					case AUTODEMIRES: 
						reste = RectClWnd.right%8;	// pour avoir un width multiple de 8
						if(RectClWnd.bottom%2) {
							MyPlug->HeightWin = ((RectClWnd.bottom>>1)<<1);	// on vire le bit de poids faible
							RectWnd->bottom--;
						} else MyPlug->HeightWin = RectClWnd.bottom;
						break;	
					case DEFINEDRES:
						MyPlug->HeightWin = RectClWnd.bottom;
						reste = ( (RectClWnd.right < MyPlug->DefXRes)?(RectClWnd.right%4):0);
				}*//*
				if(MyPlug->ResMode == AUTODEMIRES) {
					if(RectClWnd.right%2) { RectWnd->right--; RectClWnd.right--; }
					if(RectClWnd.bottom%2) { RectWnd->bottom--; RectClWnd.bottom--;}
				}

				MyPlug->WidthWin = RectClWnd.right;
				MyPlug->HeightWin = RectClWnd.bottom;

				//MyPlug->SetSizeRectWnd();
				
				//MyPlug->WidthWin = RectClWnd.right - reste;
				hret = MyPlug->CheckRes(WINDOWED);
				if(hret) {
					MyPlug->ResMode = DEFINEDRES; MyPlug->XRes = MyPlug->WidthWin = 320; MyPlug->YRes = MyPlug->HeightWin = 200;
					hret = MyPlug->SetRes(WINDOWED, MyPlug->XRes, MyPlug->YRes);
					if(hret) { MessageBox(NULL, "Erreur SetRes lors du sizing", "foirage", MB_OK); PostQuitMessage(1); }
				}
				render(&module);  // pour réactualiser la frame
	
				wsprintf(ch, "%s (%d X %d)", module.description, MyPlug->WidthWin, MyPlug->HeightWin);
				SetWindowText(MyPlug->hMainWnd, ch);
*/			}
		return 0;
		case WM_EXITSIZEMOVE:
			if(MyPlug->ViewMode == WINDOWED) {	// aura du mal à être autrement mé bon ...
				char ch[50];
				HRESULT hret;
				RECT RectClWnd;
				GetClientRect(MyPlug->hMainWnd, &RectClWnd);

				if(MyPlug->ResMode == AUTODEMIRES) {
					MyPlug->WidthWin = ((RectClWnd.right>>1)<<1);
					MyPlug->HeightWin = ((RectClWnd.bottom>>1)<<1);
				}
				else {
					MyPlug->WidthWin = RectClWnd.right;
					MyPlug->HeightWin = RectClWnd.bottom;
				}

				MyPlug->SetSizeRectWnd();

				GetClientRect(MyPlug->hMainWnd, &RectClWnd);
				wsprintf(ch, "%s (%d X %d)", module.description, MyPlug->WidthWin = RectClWnd.right, MyPlug->HeightWin = RectClWnd.bottom);
				SetWindowText(MyPlug->hMainWnd, ch);

				MyPlug->NoDisplayTransform = 0;

				hret = MyPlug->CheckRes(WINDOWED);
				if(hret) {
					MyPlug->ResMode = DEFINEDRES; MyPlug->XRes = 320; MyPlug->YRes = 200;
					hret = MyPlug->SetRes(WINDOWED, MyPlug->XRes, MyPlug->YRes);
					if(hret) { MessageBox(NULL, "Erreur SetRes lors de la sortie du sizing", "foirage", MB_OK); PostQuitMessage(1); }
				}
			}
			return 0;
		case WM_RBUTTONDOWN:
			{ // gestion menu si bouton droit appuyé
				POINT coord_souris;
				RECT r;
				UINT Choix;
				
				while(ShowCursor(TRUE)<0);

				HMENU Menu = MyPlug->MakeMenuConfig(); 

				GetWindowRect(MyPlug->hMainWnd, &r);
				coord_souris.x = 0xFFFF&lParam;
				coord_souris.y = (0xFFFF0000&lParam)>>16; 
				r.left += coord_souris.x + (4 * (MyPlug->ViewMode == WINDOWED));
				r.top += coord_souris.y + (21 * (MyPlug->ViewMode == WINDOWED));
				Choix = TrackPopupMenuEx(Menu, TPM_NONOTIFY | TPM_RETURNCMD | TPM_LEFTALIGN | TPM_TOPALIGN | TPM_HORIZONTAL, r.left, r.top, hwnd, NULL);
				if(MyPlug->RunMenuConfigSel(Choix)) {
					MyPlug->Abandon(GetLastError(), "Erreur:");
					DestroyMenu(Menu);
					PostQuitMessage(1);
				}
				
				DestroyMenu(Menu);

				if(MyPlug->ViewMode == FULLSCREEN) while(ShowCursor(FALSE)>=0);
			}
			return 0;
		case WM_KILLFOCUS:
			if(MyPlug->DDActive()) {
				if( (!MyPlug->ChildWndDisplayed)&&(MyPlug->ViewMode == FULLSCREEN) ) {
					SendMessage(MyPlug->hMainWnd, WM_SETFOCUS, wParam , lParam);
					MyPlug->ChangeViewMode(WINDOWED);
					ShowWindow(MyPlug->hMainWnd, SW_RESTORE | SW_SHOWNORMAL);
				}
				return 0; 
			}
	}	//fin switch
	return DefWindowProc(hwnd,message,wParam,lParam);  
}


// procédure de notre boite de dialogue "about"
LRESULT CALLBACK AboutDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_CLOSE:
		EndDialog(hwnd, TRUE);
		return TRUE;
	
	case WM_COMMAND:
		if(wParam == IDOK) EndDialog(hwnd, TRUE);
		return TRUE;
 	}
	return FALSE;
}


// procédure de notre boite de dialogue pour la résolution
LRESULT CALLBACK DefinedResDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{ RECT r;
	case WM_DESTROY: MyPlug->ChildWndDisplayed = 0; return 0;
	case WM_INITDIALOG:
		if(MyPlug->ViewMode != BACKGROUND) {
			GetWindowRect(MyPlug->hMainWnd, &r);
			SetWindowPos(hwndDlg, NULL, ((r.left + r.right) / 2) - 80, ((r.top + r.bottom) / 2) - 40, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		} else SetWindowPos(hwndDlg, NULL, (GetSystemMetrics(SM_CXSCREEN) / 2) - 80, (GetSystemMetrics(SM_CYSCREEN) / 2) - 40, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		SetDlgItemInt( hwndDlg, IDC_X_RENDER, MyPlug->DefXRes, FALSE);
		SetDlgItemInt( hwndDlg, IDC_Y_RENDER, MyPlug->DefYRes, FALSE);
		return TRUE;
	case WM_COMMAND:
		switch(wParam)
		{
		UINT tmp_x, tmp_y;
		case IDOK:
			tmp_x = GetDlgItemInt( hwndDlg, IDC_X_RENDER, NULL, FALSE);
			if ((tmp_x >= 16) && (tmp_x <=  MAX_WIDTH))// && (!(tmp_x%4)))
				//MyPlug->DefXRes = MyPlug->XRes = tmp_x;
				MyPlug->DefXRes = tmp_x;
			tmp_y = GetDlgItemInt( hwndDlg, IDC_Y_RENDER, NULL, FALSE);
			if ((tmp_y >= 16) && (tmp_y <=  MAX_HEIGHT))
				//MyPlug->DefYRes = MyPlug->YRes = tmp_y;		
				MyPlug->DefYRes = tmp_y;		
			EndDialog(hwndDlg, TRUE);
			MyPlug->ResMode = DEFINEDRES;
			return TRUE;
		case IDCANCEL: EndDialog(hwndDlg, TRUE); return TRUE;
		}
 	}
	return FALSE;
}



// procédure de notre boite de dialogue pour la sauvegarde de config
UINT CALLBACK ConfigNameDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_INITDIALOG) {
		SetWindowPos(hwndDlg, NULL, 0, 0, NULL, NULL, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		return TRUE;
	}
	return FALSE;
}
