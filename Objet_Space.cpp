#include "objet_space.h"

void Space::GetIniConf(char *file)
{
	char *p;

	GetModuleFileName(module.hDllInstance, file, MAX_PATH);
	p = file + strlen(file);
	while (p >= file && *p != '\\') p--;
	if (++p >= file) *p = 0;
	strcat(file, "space\\");
}



int Space::InitViewMode(int NewMode)
{  // on se sert de InitDDraw et InitWnd

#ifdef _DEBUG
	print_log("Début InitView \n");
#endif

	{ // Gestion fenêtre winamp
		// on minimise winamp en fullscreen avant d'initialiser le mode
		// car en fullscreen (DD actif) la perte de focus quitte l'applic
		// sinon on le restore toujours (plus simple)
	
		if(NewMode == FULLSCREEN) ShowWindow(module.hwndParent, SW_SHOWMINNOACTIVE);
		else ShowWindow(module.hwndParent, SW_RESTORE);
	}
		
//	MessageBox(NULL, "Procedure InitView après replacement fenêtre winamp", "info", MB_OK);

	if(NewMode == BACKGROUND)
	{
		HKEY tmp1, tmp2;
		unsigned long taille = MAX_PATH;
		unsigned long type = REG_SZ;
		/*
		char* bgbitmap = (char*)malloc(MAX_PATH * sizeof(char));
	
		GetIniConf(bgbitmap);
		strcat(bgbitmap, "space.bmp");
		RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel", 0, KEY_READ, &tmp1);
		RegOpenKeyEx(tmp1, "Desktop", 0, KEY_READ, &tmp2);
		RegQueryValueEx(tmp2, "Wallpaper", NULL, &type, WallPaperWindow, &taille);
		taille = 2;
		RegQueryValueEx(tmp2, "WallpaperStyle", NULL, &type, WallPaperWindowStyle, &taille);
		RegSetValueEx(tmp2, "WallpaperStyle", NULL, type, (const unsigned char*)"1", 2);
		SystemParametersInfo(SPI_SETDESKWALLPAPER, NULL, bgbitmap, 0);
		RegSetValueEx(tmp2, "WallpaperStyle", NULL, type, WallPaperWindowStyle, taille + 1);
		RegCloseKey(tmp2); RegCloseKey(tmp1); 

		free(bgbitmap);
		*/
		/**/
		int flags = COLOR_BACKGROUND;
		const ULONG green = 0xFF00;

		// recherche du wallpaper courant
		RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel", 0, KEY_READ, &tmp1);
		RegOpenKeyEx(tmp1, "Desktop", 0, KEY_READ, &tmp2);
		RegQueryValueEx(tmp2, "Wallpaper", NULL, &type, WallPaperWindow, &taille);
		RegCloseKey(tmp2); RegCloseKey(tmp1); 
		
		// on récupère la couleur de fond
		BackgroundColor = GetSysColor(COLOR_BACKGROUND);

		// on enlève le wallpaper
		SystemParametersInfo(SPI_SETDESKWALLPAPER, NULL, "(aucun)", 0);
		// et on met du vert en couleur de fond
		SetSysColors(1, &flags, &green);
		/**/

	}

	if(NewMode == WINDOWED)
	{
		DEVMODE disp_mode;

		ZeroMemory(&disp_mode, sizeof(DEVMODE));
		disp_mode.dmSize = sizeof(DEVMODE);
		disp_mode.dmDriverExtra = 0;
		disp_mode.dmBitsPerPel = 16;
		disp_mode.dmFields = DM_BITSPERPEL;


		if(ChangeDisplaySettings(&disp_mode, 0) != DISP_CHANGE_SUCCESSFUL)
			MessageBox(hMainWnd, "Space plugin need your desktop to be in 16 bpp in order to run correctly in windowed mode", "warning", MB_OK);
		else DispModeHasChanged = TRUE;
	}

//	MessageBox(NULL, "Procedure InitView après récupération wallpaper", "info", MB_OK);

	if(InitWnd(NewMode)) return 1;

//	MessageBox(NULL, "Procedure InitView après initialisation Windows", "info", MB_OK);

	if(CheckRes(NewMode)) return 1;
	
//	MessageBox(NULL, "Procedure InitView après Contrôle Résolution", "info", MB_OK);

	if(InitDDraw(NewMode) != DD_OK) return 1;

//	MessageBox(NULL, "Procedure InitView après initialisation DirectDraw", "info", MB_OK);
	
	if(NewMode == FULLSCREEN) while(ShowCursor(FALSE)>=0);
	
#ifdef _DEBUG
	print_log("Fin InitView \n");
#endif

	return 0;
}



int Space::CheckRes(int NewMode, int window_done)
{
	int Width, Height, ResHasChanged = 0;

#ifdef _DEBUG
	print_log("début checkres - ");
#endif
	///**/affiche("pass3");
	// on doit déterminer avant tout quel width et height utiliser
	switch(NewMode) {
	case FULLSCREEN: 
		if( (XRes != WidthMode)||(YRes != HeightMode) ) {
			XRes = WidthMode; YRes = HeightMode;
			/**/ResHasChanged = 1;
			//if(DDActive()) return SetRes(NewMode, XRes, YRes);
		}
		//return 0;
		/**/goto fin;
	case BACKGROUND: 
		if( (XRes != WidthBack)||(YRes != HeightBack) ) {
			XRes = WidthBack; YRes = HeightBack;
			/**/ResHasChanged = 1;
			//if(DDActive()) return SetRes(NewMode, XRes, YRes);
		}
		//return 0;
		/**/goto fin;
	case DOCKED: case WINDOWED: 
		{
			RECT rect;
			GetClientRect(hMainWnd, &rect);
			if(NewMode == WINDOWED) {
				if(window_done) {
					WidthWin = rect.right;
					HeightWin = rect.bottom; 
				}
				Width = (WidthWin>MAX_WIDTH)?MAX_WIDTH:WidthWin;
				Height = (HeightWin>MAX_HEIGHT)?MAX_HEIGHT:HeightWin;
			}
			else {
				Width = rect.right;
				Height = rect.bottom; 
				Width = (Width>MAX_WIDTH)?MAX_WIDTH:Width;
				Height = (Height>MAX_HEIGHT)?MAX_HEIGHT:Height;
			}
			if(ResMode == AUTODEMIRES) { Width = (Width < 32)?32:Width; Height = (Height < 32)?32:Height; }
			else { Width = (Width < 16)?16:Width; Height = (Height < 16)?16:Height; }
		}
		break;
	default:
		MessageBox(module.hwndParent,"Mode d'écran non reconnue (fichier ini endommagé ?), vérification de résolution impossible","dommage ...",MB_OK);
		return 1;
	}

#ifdef _DEBUG
	print_log("selection taille pass - ");
#endif
	///**/affiche("pass4");

	if(blitrotozoom) {
		if(ResMode == AUTORES)
		{
			if(XRes != ((Width>>2)<<2)) { XRes = ((Width>>2)<<2); ResHasChanged = 1; }
			if(YRes != Height) { YRes = Height; ResHasChanged = 1; }
		}
		else if(ResMode == AUTODEMIRES)
		{
			if(XRes != ((Width>>3)<<2)) { XRes = ((Width>>3)<<2); ResHasChanged = 1; }
			if(YRes != (Height>>1)) { YRes = (Height>>1); ResHasChanged = 1; }
		}
		else
		{  // ResMode = DEFINED
			DefXRes = ((DefXRes>>2)<<2);
			/**/Width = ((Width>>2)<<2);
			//DefYRes = ((DefYRes>>2)<<2);
			int MaxXRes = (DefXRes>Width)?Width:DefXRes;
			int MaxYRes = (DefYRes>Height)?Height:DefYRes;

			if(XRes != MaxXRes)
			{ XRes = MaxXRes; ResHasChanged = 1;}

			if(YRes != MaxYRes)
			{ YRes = MaxYRes; ResHasChanged = 1;}
		}
	}
	else {
		if(ResMode == AUTORES)
		{
			if(XRes != Width) { XRes = Width; ResHasChanged = 1; }
			if(YRes != Height) { YRes = Height; ResHasChanged = 1; }
		}
		else if(ResMode == AUTODEMIRES)
		{
			if(XRes != (Width>>1)) { XRes = (Width>>1); ResHasChanged = 1; }
			if(YRes != (Height>>1)) { YRes = (Height>>1); ResHasChanged = 1; }
		}
		else
		{  // ResMode = DEFINED
			int MaxXRes = ( (DefXRes>Width)?Width:DefXRes );
			int MaxYRes = ( (DefYRes>Height)?Height:DefYRes );

			if(XRes != MaxXRes)
			{ XRes = MaxXRes; ResHasChanged = 1;}

			if(YRes != MaxYRes)
			{ YRes = MaxYRes; ResHasChanged = 1;}
		}
	}

#ifdef _DEBUG
	print_log("fin checkres (avant appel SetRes)\n");
#endif

fin:
	if( ((DDActive())&&( (ResHasChanged)||((blitrotozoom)&&(!rotoblitstruct.SurfBack)) )) ) 
		return(SetRes(NewMode, XRes, YRes));

	return 0;
}


int Space::InitWnd(int NewMode)
{
	DWORD WsExStyle = 0, WsStyle = 0;
	RECT RectWnd = {0, 0, 0, 0};

	if(!(hMainWnd)) 
	{
		// création de la fenêtre
		hMainWnd = CreateWindowEx(
			0,						// pas de Ex_Style par défaut
			szAppName,				// window class name
			module.description,		// use description for a window title
			0,						// pas de Style par défaut
			0,0,					// coordonnées
			0,0,					// width & height 
			module.hwndParent,		// parent window (winamp main window)
			NULL,					// no menu
			module.hDllInstance,	// hInstance of DLL
			0); // no window creation data

		//affiche("Procedure InitWnd après création fenêtre plugin");
		
		if (!hMainWnd) return Abandon(1, "Erreur de création de la fenêtre. Essayer de quitter et relancer Winamp");
		//ShowWindow(hMainWnd,SW_SHOWNORMAL); // la fenêtre est affichée pour la première fois 
	}

	// application des styles et positionnement de la fenêtre
	switch(NewMode) {
	case WINDOWED:

		WsExStyle = WS_EX_TOOLWINDOW;
		WsStyle = WS_THICKFRAME | WS_SYSMENU | WS_BORDER | WS_CAPTION;
		SetRect(&RectWnd, XWin, YWin, XWin + WidthWin - 1, YWin + HeightWin - 1);
		if(!(AdjustWindowRectEx(&RectWnd, WsStyle, GetMenu(hMainWnd) != NULL, WsExStyle)))
			MessageBox(NULL, "Echec d'ajustement de la zone cliente de la fenêtre ..." , "crut !", MB_OK);
		break;
	//case DOCKED:
	//	RectWnd = GetRectDock(DockMode);
	//	break;
	//case FULLSCREEN:
	//	WsExStyle = WS_EX_TOPMOST;
	//	WsStyle = WS_VISIBLE;
	//	SetRect(&RectWnd, 0, 0, WidthMode - 1, HeightMode - 1);
	//	break;
	case BACKGROUND:
		WsExStyle = WS_EX_TOPMOST;
		WsStyle = WS_VISIBLE;
		SetRect(&RectWnd, 0, 0, 10, 10);
		break;
	}
			

	//affiche("Procedure InitWnd après le switch");
	
	if(NewMode == WINDOWED) {
		char ch[30];
		wsprintf(ch, "%s (%d X %d)", module.description, WidthWin, HeightWin);
		SetWindowText(hMainWnd, ch);
	}
		
	SetWindowLong(hMainWnd, GWL_EXSTYLE, WsExStyle);
	SetWindowLong(hMainWnd, GWL_STYLE, WsStyle);

	//affiche("Procedure InitWnd après application des styles de la fenêtre");
	
	/**/if(NewMode == DOCKED) SetRectDock(DockMode, TRUE);
	else /**/AfficheMainWnd(RectWnd);

	if(NewMode == WINDOWED) SetSizeRectWnd();
	
	//affiche("Procedure InitWnd après affichage fenêtre");
	
	return 0;
}



int Space::InitDDraw(int NewMode)
{
	DDSURFACEDESC2 ddsd;			// descripteur de surface directdraw (structure)
	HRESULT ddrval;				// valeur de retour des fonctions d'initialisation ddraw
	
#ifdef _DEBUG
	print_log("Début InitDDraw - ");
#endif

	ddrval = DirectDrawCreate( NULL, &lpDD1, NULL );
	
	if( ddrval != DD_OK ) return Abandon(ddrval, "Création Objet Directdraw:", TRUE);

	ddrval = lpDD1->QueryInterface(IID_IDirectDraw4, (LPVOID*)&lpDD);

	if( ddrval != DD_OK ) return Abandon(ddrval, "Demande Interface DirectDraw3:", TRUE);

	// n'a pû besoin, on detruit
	lpDD1->Release();
	lpDD1 = NULL;

	CapsDrv.dwSize = sizeof(CapsDrv);
	ddrval = lpDD->GetCaps(&CapsDrv, NULL); 
	if (ddrval != DD_OK) return Abandon(ddrval, "Obtention capacités directdraw", TRUE); 

#ifdef _DEBUG
	print_log("Création objet directdraw4 et getcap pass - ");
#endif
	
	//création liste des modes supportés
	if(ModeFounded == FALSE) {
		cont1 cont; cont.i = 0; cont.spc = this;
			lpDD->EnumDisplayModes(0, NULL, &cont, (LPDDENUMMODESCALLBACK2)MakeSupportListe);
			if(cont.i) ModeSupport[cont.i][0] = ModeSupport[cont.i][1] = 0; 
			ModeFounded = TRUE;	// même si on a pas réussi à avoir les modes on a essayé
	}

	switch(NewMode) {
	case FULLSCREEN: // mode exclusif
		ddrval = lpDD->SetCooperativeLevel(hMainWnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN);
		if(ddrval != DD_OK ) return Abandon(ddrval, "SetCooperativeLevel DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN:", TRUE);

		//ddrval = lpDD->SetDisplayMode( WidthMode, HeightMode, DepthMode );
		ddrval = lpDD->SetDisplayMode( WidthMode, HeightMode, DepthMode, NULL, NULL);
		if(ddrval != DD_OK ) return Abandon(ddrval, "SetDisplayMode:", TRUE);
		break;
	case BACKGROUND:
	case WINDOWED:
	case DOCKED:
		ddrval = lpDD->SetCooperativeLevel(hMainWnd, DDSCL_NORMAL);
		if(ddrval != DD_OK ) return Abandon(ddrval, "SetCooperativeLevel DDSCL_NORMAL:", TRUE);
		break;
	default:  return Abandon(DDERR_GENERIC, "Initialisation du directdraw impossible car mode inconnu spécifié:");
	}
	
	// création de la surface principale (celle qui est affichée)
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);	
	ddsd.dwFlags = DDSD_CAPS; 		
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE ;	
	ddrval = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );  // on crée la surface dont l'adresse est lpDDSPrimary
	if( ddrval != DD_OK ) return Abandon(ddrval, "Création Première Surface: ", TRUE);
		
	// création d'un clipper pour les modes fenêtrés
	if( (NewMode != FULLSCREEN)&&(NewMode != BACKGROUND) ) {
		if(lpDD->CreateClipper(0, &lpDDClipper, NULL) != DD_OK ) return Abandon(ddrval, "Création Clipeur", TRUE);
		if(lpDDClipper->SetHWnd(0, hMainWnd) != DD_OK) return Abandon(ddrval, "Attachement clipeur fenêtre:", TRUE);
		if(lpDDSPrimary->SetClipper(lpDDClipper) != DD_OK) return	Abandon(ddrval, "Attachement clipeur surface:", TRUE);
	}
	
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );	
	ddsd.dwFlags = DDSD_PITCH ;
	if(lpDDSPrimary->GetSurfaceDesc(&ddsd) != DD_OK) return Abandon(ddrval, "Obtention Description Surface Primaire:", TRUE);
	
	SurfacePrimaryPitch = ddsd.lPitch;
	SurfacePrimaryPixFormat = ddsd.ddpfPixelFormat;

#ifdef _DEBUG
	print_log("Création primary surf, clipper et getdesc sur prim pass avant setres\n");
#endif

	// création des tableaux de travail et
	// de la seconde surface qui va permettre le blit
	if(SetRes(NewMode, XRes, YRes)) return Abandon(DDERR_GENERIC, "SetRes.");
	
	ddsd.dwSize = sizeof( ddsd );	
	ddrval = lpDDSPrimary->Lock(NULL, &ddsd,  DDLOCK_WAIT, NULL);
	if(ddrval != DD_OK) return Abandon(ddrval, "Lock surface primary après récupération pointeur:", TRUE);
	
	SurfacePrimPointer = ddsd.lpSurface;  // screen pointe sur la surface directdraw (primaire)
	//ddrval = lpDDSPrimary->Unlock(ddsd.lpSurface);
	ddrval = lpDDSPrimary->Unlock(NULL);
	if(ddrval != DD_OK) return Abandon(ddrval, "DéLock surface primary après récupération pointeur:", TRUE);
	
#ifdef _DEBUG
	print_log("Fin InitDDraw\n");
#endif

	return ddrval;
}



void Space::KillViewMode(int Mode) {
// on se sert de KillDDraw
///***/MessageBox(NULL, "Avant KillDDraw" , "info", MB_OK);
		KillDDraw();

 		if(Mode == FULLSCREEN) {   // on restore winamp si on était en fullscreen après avoir détruit le directdraw
			// pour ne pas que la perte de focus du plugin cause une fermeture de celui-ci
			/*
			RECT r;
			WINDOWPLACEMENT wpl;

			GetWindowRect(module.hwndParent, &r);
			wpl.length=sizeof(WINDOWPLACEMENT);
			wpl.flags=0;
			wpl.showCmd=SW_RESTORE;
			wpl.ptMaxPosition.x= wpl.ptMaxPosition.y= 0;
			wpl.rcNormalPosition=r;
			SetWindowPlacement(module.hwndParent, &wpl);
			*/
			ShowWindow(module.hwndParent, SW_RESTORE);

			while(ShowCursor(TRUE)<0);
		}
		else if(Mode == BACKGROUND) //SystemParametersInfo(SPI_SETDESKWALLPAPER, NULL, WallPaperWindow, 0);
		/**/
		{
			int flags = COLOR_BACKGROUND;
			
			// winamp freeze s'il reçoit un message lorsqu'il essai d'arrêter lui-même le plugin
			// je vois pas comment empêcher ça, fô attendre la prochaine version de winamp ?
			SetSysColors(1, &flags, &BackgroundColor);
			SystemParametersInfo(SPI_SETDESKWALLPAPER, NULL, WallPaperWindow, 0);
		}
		/**/
}



void Space::KillDDraw()
{
///**/MessageBox(NULL, "Début KillDDraw", "info", MB_OK);
	
	if(lpDD)  
	{
///**/MessageBox(NULL, "Avant Release lpDDClipper", "info", MB_OK);
		if(lpDDClipper)
		{	
			lpDDClipper->Release();
            lpDDClipper = NULL;
	    }
		
///**/MessageBox(NULL, "Avant Release lpDDSBack", "info", MB_OK);
		if(lpDDSBack)
		{	
			if(ViewMode == BACKGROUND) lpDDSBack->UpdateOverlay(NULL, lpDDSPrimary, NULL, DDOVER_HIDE, NULL); 
			lpDDSBack->Release();
            lpDDSBack = NULL;
	    }
		
///**/MessageBox(NULL, "Avant Release lpDDSPrimary", "info", MB_OK);
			
		if( lpDDSPrimary)
		{
			lpDDSPrimary->Release();
            lpDDSPrimary = NULL;
	    }
///**/MessageBox(NULL, "Avant Release lpDD", "info", MB_OK);
		
		if(ViewMode == FULLSCREEN) {
			lpDD->RestoreDisplayMode();
			lpDD->SetCooperativeLevel(hMainWnd, DDSCL_NORMAL);
		}
		
		/**/ if(blitrotozoom) end_rotoblit();

		lpDD->Release();
		lpDD = NULL;
	}
///**/MessageBox(NULL, "Fin KillDDraw", "info", MB_OK);
}


RECT Space::SetRectDock(int DMode, bool init)
{
	RECT rect;
	BOOL winamp_is_shade = FALSE;
	UINT winamp_is_double_mode = 0;
	/**/static int old_width = 0, old_height = 0;
	/**/static int old_left = 0, old_top = 0;
	/**/int decalage = 0;

	GetWindowRect(module.hwndParent, &rect);

	if((rect.bottom - rect.top) < 40)
	{
		winamp_is_shade = TRUE; 
		if((rect.bottom - rect.top) > 20) winamp_is_double_mode = 1;
		else winamp_is_double_mode = 0;
	}
	else if((rect.bottom - rect.top) > 150) winamp_is_double_mode = 1;
	
	switch(DMode) {
	case LITTLEDOCK: 
		if(winamp_is_shade)
		{
			rect.left = rect.left += (78 << winamp_is_double_mode);
			rect.right =  rect.left + (41 << winamp_is_double_mode);
			rect.top += (3 << winamp_is_double_mode);
			rect.bottom = rect.top + (8 << winamp_is_double_mode);
		}
		else
		{
			rect.left = rect.left += (24 << winamp_is_double_mode);
			rect.right =  rect.left + (76 << winamp_is_double_mode);
			rect.top += (26 << winamp_is_double_mode);
			rect.bottom = rect.top + (34 << winamp_is_double_mode);
		} 
		break;
	default: 
	case BOTTOMDOCK:
		rect.top = rect.bottom;
		rect.bottom += (116 << winamp_is_double_mode);
		break;
	case TOPDOCK:
		rect.bottom = rect.top;
		rect.top -= (116 << winamp_is_double_mode);
		break;
	case LEFTDOCK: {
		int tmp = rect.right;
		rect.right = rect.left;
		rect.left -= (tmp - rect.left);
		break; }
	case RIGHTDOCK: {
		int tmp = rect.left;
		rect.left = rect.right;
		rect.right += (rect.right - tmp);
		break; }
	}

	/**/
	if(ResMode == AUTODEMIRES) {
		if(DMode == LEFTDOCK) rect.left += ((rect.right - rect.left + 1)%2);
		else rect.right -= ((rect.right - rect.left + 1)%2);
		if(DMode == TOPDOCK) rect.top += ((rect.bottom - rect.top + 1)%2);
		else rect.bottom -= ((rect.bottom - rect.top + 1)%2);
	}

	if( (old_left != rect.left)||(old_top != rect.top)||(old_width != (rect.right - rect.left + 1) )||(old_height != (rect.bottom - rect.top + 1) )||(init) ) {
		old_width = rect.right - rect.left + 1;
		old_height = rect.bottom - rect.top + 1;
		SetSizeRectWnd(old_width, old_height, 0, 0, &decalage);

		if(decalage) {
			if(DMode == LEFTDOCK) rect.left += decalage;
			else rect.right -= decalage;
		}

		old_left = rect.left;
		old_top = rect.top;
		SetWindowPos(hMainWnd, NULL, old_left, old_top, 0, 0, SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

		if(DDActive()) CheckRes(ViewMode);
	}
	/**/

	return rect;
}



int Space::SetRes(int NewMode, int ResX, int ResY)
{
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;

#ifdef _DEBUG
	print_log("début SetRes - ");
#endif
	///**/affiche("pass6");
	if(Screen) free(Screen); Screen = NULL;
	Screen = (USHORT*)malloc((ResX * ResY + 16) * (DepthMode >> 3));
	if(ScreenBack) free(ScreenBack); ScreenBack = NULL;
	ScreenBack = (USHORT*)malloc((ResX * ResY + 16) * (DepthMode >> 3));
	if( (Screen == NULL)||(ScreenBack == NULL) ) return 1;
	ZeroMemory(Screen, ResX * ResY * (DepthMode >> 3));
	ZeroMemory(ScreenBack, ResX * ResY * (DepthMode >> 3));
	// le dernier pixel du tableau ne sert que pour les transformations 

#ifdef _DEBUG
	print_log("init tabs pass - ");
#endif

	if(lpDDSBack != NULL) { lpDDSBack->Release(); lpDDSBack = NULL; }
		
	if(lpDD == NULL) return Abandon(1, "Pas d'objet directdraw ?");
	
	/**/
	if(blitrotozoom) {
		ddrval = init_rotoblit(ResX, ResY);
		if(ddrval != DD_OK) {
			blitrotozoom = FALSE;

#ifdef _DEBUG
			print_log("Appel Init Rotoblit dans SetRes failed...\n");
#endif
//			Abandon(ddrval, "Initialisation blit effects failed:", TRUE);
			end_rotoblit();
		}
#ifdef _DEBUG
		else print_log("Appel Init Rotoblit pass dans SetRes\n");
#endif
	}
	/**/

	if(NewMode == FULLSCREEN) return 0;	//pas besoin de surface back en fullscreen

	ZeroMemory(&ddsd, sizeof(ddsd));// on rempli la structure par des zéros
	ddsd.dwSize = sizeof(ddsd);		// on rempli les champs du descripteur de surface

	
	// pour l'instant on suppose DepthMode toujours = 16
	DDPIXELFORMAT DdPixelFormat[/*2*/6] = 
{   {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0x7C00, 0x03e0, 0x001F, 0},   // 16-bit RGB 5:5:5 
    {sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_RGBTOYUV, 0, 16,  0x7C00, 0x03e0, 0x001F, 0},	// 16-bit YUV 5:5:5 
    {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_RGBTOYUV, 0, 16,  0xF800, 0x07e0, 0x001F, 0},	// 16-bit YUV 5:6:5 
	/*{sizeof(DDPIXELFORMAT), DDPF_RGB, 0, 32, 0xFF0000, 0xFF00, 0xFF, 0},   // 32-bit RGB
    {sizeof(DDPIXELFORMAT), DDPF_RGB | DDPF_RGBTOYUV, 0, 32, 0xFF0000, 0xFF00, 0xFF, 0},	// 32-bit YUV
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','T','1'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','T','2'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','T','3'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','T','4'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','T','5'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','0'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','1'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','2'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','3'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','4'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('N','V','S','5'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_FOURCC,MAKEFOURCC('I','V','3','2'), 32,  0xF800F800, 0x07e007e0, 0x001F001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('I','V','3','1'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('Y','U','N','V'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('U','Y','N','V'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('Y','V','1','2'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_YUV | DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('Y','V','U','9'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 
    {sizeof(DDPIXELFORMAT), DDPF_RGBTOYUV | DDPF_FOURCC,MAKEFOURCC('I','F','0','9'), 16,  0xF800, 0x07e0, 0x001F, 0},   // 16-bit RGB 5:6:5 */
    {sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('U','Y','V','Y'),0,0,0,0,0},	// UYVY , et on peut rien n'en foutre de ces putains de formats de merde !
    {sizeof(DDPIXELFORMAT), DDPF_FOURCC,MAKEFOURCC('Y','U','Y','2'),0,0,0,0,0}   };  // YUY2 

	if(NewMode != BACKGROUND) {
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;	  
		ddsd.dwWidth= ResX; ddsd.dwHeight= ResY;
    	
		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
		if(ddrval != DD_OK) return Abandon(ddrval, "Creation Surface Back:", TRUE);	
	}	
	else {
		if (!(CapsDrv.dwCaps & DDCAPS_OVERLAY)) return Abandon(1, "Votre carte vidéo n'a pas de support overlay (serait temps d'en changer ...)");

		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth= ResX; ddsd.dwHeight= ResY;

	    int i=0;
		do{
    	ddsd.ddpfPixelFormat =  DdPixelFormat[i];
 		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
		} while( (ddrval != DD_OK)&&(++i < /*2*/6) );
		if(ddrval != DD_OK) return Abandon(ddrval, "Creation Surface Back:", TRUE);	
		else if(i>3) {
			char ch[80];
			wsprintf(ch, "Sorry, video color format isn't RGB: format N°%d", i);
			Abandon(1,ch);
		}
		
		//enfin on "update" la surface overlay
		RECT srccoord = {0, 0, XRes - 1, YRes - 1};
		if (CapsDrv.dwCaps & DDCAPS_ALIGNSIZESRC && CapsDrv.dwAlignSizeSrc)
        srccoord.right -= srccoord.right % CapsDrv.dwAlignSizeSrc; 
		RECT destcoord = {0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)};
		DDOVERLAYFX ovfx; ZeroMemory(&ovfx, sizeof(ovfx)); ovfx.dwSize = sizeof(ovfx);
    	ovfx.dckDestColorkey.dwColorSpaceLowValue = ovfx.dckDestColorkey.dwColorSpaceHighValue = SurfacePrimaryPixFormat.dwGBitMask;
    	//ovfx.dckDestColorkey.dwColorSpaceLowValue= ((SurfacePrimaryPixFormat.dwGBitMask)&0x780);// 0x7C0 ou 0x3C0
    	//ovfx.dckDestColorkey.dwColorSpaceHighValue = SurfacePrimaryPixFormat.dwGBitMask;
		
		ddrval = lpDDSBack->UpdateOverlay(&srccoord, lpDDSPrimary, &destcoord, DDOVER_KEYDESTOVERRIDE | DDOVER_SHOW, &ovfx);
		if(ddrval != DD_OK) return Abandon(ddrval, "UpdateOverlay:", TRUE);
	}

#ifdef _DEBUG
	print_log("création surface pass - ");
#endif
	
	ddsd.dwSize = sizeof( ddsd );	
	ddrval = lpDDSBack->Lock(NULL, &ddsd,  DDLOCK_WAIT, NULL);
	if(ddrval != DD_OK) return Abandon(ddrval, "Lock surface back avant récupération pointeur:", TRUE);

	SurfaceBackPointer = ddsd.lpSurface;  // screen pointe sur la surface directdraw (secondaire)
	ddrval = lpDDSBack->Unlock(NULL);
	if(ddrval != DD_OK) return Abandon(ddrval, "DéLock surface Back après récupération pointeur:", TRUE);
	
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );	
	ddsd.dwFlags = DDSD_PITCH ;
	if(lpDDSBack->GetSurfaceDesc(&ddsd) != DD_OK)  return Abandon(ddrval, "Obtention Pitch Surface Back:", TRUE);
	
	SurfaceBackPitch = ddsd.lPitch;

#ifdef _DEBUG
	print_log("récup info pass .. Fin SetRes\n");
#endif
	
	///**/affiche("pass8");

	return 0;
}


void Space::AfficheMainWnd(RECT NewRect)
{
	//SetWindowPos(hMainWnd, NULL, NewRect.left , NewRect.top, NewRect.right - NewRect.left + 1, NewRect.bottom - NewRect.top + 1, SWP_NOZORDER | SWP_NOACTIVATE);
	MoveWindow(hMainWnd, NewRect.left , NewRect.top, NewRect.right - NewRect.left + 1, NewRect.bottom - NewRect.top + 1, TRUE);
	ShowWindow(hMainWnd, SW_SHOW);
}


char* Space::Version = "Space";

Space::Space() : lpDD(NULL), lpDDSPrimary(NULL), lpDDSBack(NULL), lpDDClipper(NULL), Screen(NULL), ScreenBack(NULL),
SurfacePrimPointer(NULL), SurfaceBackPointer(NULL), hMainWnd(NULL), XWin(50), ModeFounded(FALSE), LoadRandom(FALSE), //LimitFPS(0),
YWin(50), WidthWin(320), HeightWin(200), WidthMode(320), HeightMode(200), WidthBack(320), HeightBack(200), DockMode(BOTTOMDOCK),
DepthMode(16), Speed(SPEED30), ViewMode(WINDOWED), ResMode(AUTORES), DefXRes(400), DefYRes(300), YRes(200), XRes(320), Flashis(FALSE),
Mode555(0), Blend(FALSE), TransBefore(FALSE), ChildWndDisplayed(0), NoDisplayTransform(FALSE), Render(NBLUR), Mirror(NOMIRROR), RandomMirror(FALSE),
Fade(FALSE), RandomWaveForm(TRUE), RandomCercleWave(TRUE), RandomCercleBass(TRUE), RandomConfettis(TRUE), RandomMathShape(TRUE),
RandomRotaLine(TRUE), RandomLittleWave(TRUE), RandomLittleMath(TRUE), RandomGeoGraph(TRUE), ShowFPS(FALSE), DispModeHasChanged(FALSE),
BeatEffect(FALSE), BeatTrans(FALSE), blitrotozoom(FALSE), blitzoom(BLITZOOMIN), blitrotation(BLITNOROTATE)
{  //constructeur = initialisation

#ifdef _DEBUG
	print_log("Construction Space\n");
#endif

	Trans.enabled = TRUE;
	Trans.random = 1;
	Trans.slide = FALSE;
	Trans.noise = FALSE;
	Trans.shift = NOSHIFT;
	Trans.presourcecoordtable = Trans.sourcecoordtable = NULL;
	//Trans.fast = FALSE;
	//Trans.fast = TRUE;

	rotoblitstruct.SurfBack = rotoblitstruct.SurfDest = NULL;
	WaveFormEff::nombre = 0;
	CercleBassEff::nombre = 0;
	CercleWaveEff::nombre = 0;
	ConfettisEff::nombre = 0;
	LittleWaveEff::nombre = 0;
	MathShapeEff::nombre = 0;
	RotaLineEff::nombre = 0;

	//on initialise les modes fullscreen par défaut
	ModeSupport[0][0] = 320;
	ModeSupport[0][1] = 200;
	ModeSupport[1][0] = 320;
	ModeSupport[1][1] = 240;
	ModeSupport[2][0] = 400;
	ModeSupport[2][1] = 300;
	ModeSupport[3][0] = 640;
	ModeSupport[3][1] = 480;
	ModeSupport[4][0] = 800;
	ModeSupport[4][1] = 600;
	ModeSupport[5][0] = 1024;
	ModeSupport[5][1] = 768;
	ModeSupport[6][0] = ModeSupport[6][1] = 0;

		
/**
	{
		DEVMODE disp_mode;

		ZeroMemory(&disp_mode, sizeof(DEVMODE));
		disp_mode.dmSize = sizeof(DEVMODE);
		disp_mode.dmDriverExtra = 0;
		disp_mode.dmBitsPerPel = 16;
		disp_mode.dmFields = DM_BITSPERPEL;


		if(ChangeDisplaySettings(&disp_mode, 0) != DISP_CHANGE_SUCCESSFUL)
			MessageBox(hMainWnd, "Space plugin need your desktop to be in 16 bpp in order to run correctly in windowed mode", "warning", MB_OK);
		else DispModeHasChanged = TRUE;
	}
**/

	srand(GetTickCount()); // initialisation des nombres aléatoires

	//on charge la config
	char* ini_file = (char*)malloc(MAX_PATH * sizeof(char));

	GetIniConf(ini_file); strcat(ini_file, "space.ini");
	ViewMode = GetPrivateProfileInt("Space Config", "View_Mode", ViewMode, ini_file);
	XWin = GetPrivateProfileInt("Space Config", "X_Win", XWin, ini_file);
	YWin = GetPrivateProfileInt("Space Config", "Y_Win", YWin, ini_file);
	DockMode = GetPrivateProfileInt("Space Config", "Dock_Mode", DockMode, ini_file);
	WidthMode = GetPrivateProfileInt("Space Config", "Width_Mode", WidthMode, ini_file);
	HeightMode = GetPrivateProfileInt("Space Config", "Height_Mode", HeightMode, ini_file);
	WidthBack = GetPrivateProfileInt("Space Config", "Width_Back", WidthBack, ini_file);
	HeightBack = GetPrivateProfileInt("Space Config", "Height_Back", HeightBack, ini_file);
	WidthWin = GetPrivateProfileInt("Space Config", "Width_Win", WidthWin, ini_file);
	HeightWin = GetPrivateProfileInt("Space Config", "Height_Win", HeightWin, ini_file);
	DepthMode = GetPrivateProfileInt("Space Config", "Depth_Mode", DepthMode, ini_file);
	ResMode = GetPrivateProfileInt("Space Config", "Res_Mode", ResMode, ini_file);
	DefXRes = GetPrivateProfileInt("Space Config", "Def_X_Res", DefXRes, ini_file);
	DefYRes = GetPrivateProfileInt("Space Config", "Def_Y_Res", DefYRes, ini_file);
	Mode555 = GetPrivateProfileInt("Space Config", "Mode_555", Mode555, ini_file);
	LoadRandom = GetPrivateProfileInt("Space Config", "Load_Random", LoadRandom, ini_file);
	Speed = GetPrivateProfileInt("Space Config", "Speed", Speed, ini_file);

	free(ini_file);
}

	
Space::~Space()
{ // destructeur = on libère tout !!

#ifdef _DEBUG
	print_log("Destruction Space\n");
#endif

	KillViewMode(ViewMode);
	
	//on sauvegarde la configuration
	char* ini_file = (char*)malloc(MAX_PATH * sizeof(char)); char string[32];
	GetIniConf(ini_file); strcat(ini_file, "space.ini");
	wsprintf(string,"%d",XWin);
	WritePrivateProfileString("Space Config","X_Win",string,ini_file);
	wsprintf(string,"%d",YWin);
	WritePrivateProfileString("Space Config","Y_Win",string,ini_file);
	wsprintf(string,"%d",WidthWin);
	WritePrivateProfileString("Space Config","Width_Win",string,ini_file);
	wsprintf(string,"%d",HeightWin);
	WritePrivateProfileString("Space Config","Height_Win",string,ini_file);
	wsprintf(string,"%d",DockMode);
	WritePrivateProfileString("Space Config","Dock_Mode",string,ini_file);
	wsprintf(string,"%d",WidthMode);
	WritePrivateProfileString("Space Config","Width_Mode",string,ini_file);
	wsprintf(string,"%d",HeightMode);
	WritePrivateProfileString("Space Config","Height_Mode",string,ini_file);
	wsprintf(string,"%d",WidthBack);
	WritePrivateProfileString("Space Config","Width_Back",string,ini_file);
	wsprintf(string,"%d",HeightBack);
	WritePrivateProfileString("Space Config","Height_Back",string,ini_file);
	wsprintf(string,"%d",DepthMode);
	WritePrivateProfileString("Space Config","Depth_Mode",string,ini_file);
	wsprintf(string,"%d",ResMode);
	WritePrivateProfileString("Space Config","Res_Mode",string,ini_file);
	wsprintf(string,"%d",DefXRes);
	WritePrivateProfileString("Space Config","Def_X_Res",string,ini_file);
	wsprintf(string,"%d",DefYRes);
	WritePrivateProfileString("Space Config","Def_Y_Res",string,ini_file);
	wsprintf(string,"%d",Mode555);
	WritePrivateProfileString("Space Config","Mode_555",string,ini_file);
	wsprintf(string,"%d",ViewMode);
	WritePrivateProfileString("Space Config","View_Mode",string,ini_file);
	wsprintf(string,"%d",Speed);
	WritePrivateProfileString("Space Config","Speed",string,ini_file);
	wsprintf(string,"%d",LoadRandom);
	WritePrivateProfileString("Space Config","Load_Random",string,ini_file);
		
	SaveConf(Version);		// sauvegarde la config en cours (doit être avant killdata)

	free(ini_file);
	free(Screen);
	free(ScreenBack);

	KillEffects();	//destruction effets & trans ..

	DestroyWindow(hMainWnd);

	if(DispModeHasChanged) ChangeDisplaySettings(NULL, 0);
}



void Space::KillEffects()
{  // on détruit tous nos effets

//suppression de nos effets
	while(WaveFormEff::nombre)
		delete WaveForm[WaveFormEff::nombre - 1];

	while(CercleWaveEff::nombre)
		delete CercleWave[CercleWaveEff::nombre - 1];

	while(CercleBassEff::nombre)
		delete CercleBass[CercleBassEff::nombre - 1];

	while(ConfettisEff::nombre)
		delete Confettis[ConfettisEff::nombre - 1];

	while(MathShapeEff::nombre)
		delete MathShape[MathShapeEff::nombre - 1];

	while(RotaLineEff::nombre)
		delete RotaLine[RotaLineEff::nombre - 1];

	while(LittleWaveEff::nombre)
		delete LittleWave[LittleWaveEff::nombre - 1];

	while(LittleMathEff::nombre)
		delete LittleMath[LittleMathEff::nombre - 1];

	while(GeoGraphEff::nombre)
		delete GeoGraph[GeoGraphEff::nombre - 1];
}



int Space::ChangeViewMode(int NewMode)
{  // on se sert des fonctions privées KillViewMode et InitViewMode

#ifdef _DEBUG
	print_log("Début ChangeViewMode \n");
#endif

	KillViewMode(ViewMode);
///***/MessageBox(NULL, "Après KillViewMode" , "info", MB_OK);
	UINT Result = InitViewMode(NewMode);
///***/MessageBox(NULL, "Après InitViewMode" , "info", MB_OK);
	if(Result) {
		MessageBox(NULL, " La carte vidéo ne supporte pas ce mode", "info" , MB_OK);
		KillViewMode(NewMode);
		ViewMode = WINDOWED;
		Result = InitViewMode(ViewMode);
	} else ViewMode = NewMode;

	if(Result) return 1;

#ifdef _DEBUG
	print_log("Fin ChangeViewMode \n");
#endif

	return 0;
}



int Space::CalculRender() {	// c là que le rendu se fait (on travail sur XRes et YRes)
// on se sert des objets Effets et Render (définies dans Effects et Renders.cpp)
	static int ComptEffects = 0;
	static int ComptLoad = 600;
	static UINT decfade = 7;	//au moins toujours un fadeout toutes les 7 frames
	static int Accel_moy = 30;	// moyenne de la musique pour détecter les hausses (beat ?)
	/**/static int Accel_moy2 = 0;	// moyenne de la musique pour détecter les hausses (beat ?)
	/**/static int Accel_moy3 = 0;	// moyenne de la musique pour détecter les hausses (beat ?)
	int i = 0, j, HRet = 0;
	int new_xres = 0, new_yres = 0;
	//int dep_x = (Trans.deplacement_x - Trans.delta_nbx), dep_y = (Trans.deplacement_y - Trans.delta_nby);
	/**/bool beateff, beattrans;
	float BassSensibility = 3; /* varie de 1 (moins sensible) à 5 (très sensible) */
	//float BassSensibility = 4; /* varie de 1 (moins sensible) à 5 (très sensible) */


	// faudrait voir à arranger tout ce bordel 
	// calcul de Accel, de BassDetected et de Beat
	Accel = 0;
	Beat = FALSE;
	BassDetected = 0;

	/**
	//Accel_moy3 = Accel_moy2;
	Accel_moy2 = Accel;
	/**/
	
	int bass=0;
	int midbass=0;
	int highbass=0;
	while(i < 5) bass += module.spectrumData[0][i++];		
	bass /= 10;
	//bass /= 20;
	while(i < 35) midbass += module.spectrumData[0][i++];		
	midbass /= 20;
	while(i < 185) highbass += module.spectrumData[0][i++];		
	highbass /= 25;
	Accel = bass + midbass + highbass;
	
	BassDetected = DetectBass();

	/**/
	Accel_moy3 = Accel_moy2;
	Accel_moy2 = Accel_moy;
	/**/
	
	Accel_moy += Accel;
	Accel_moy >>= 1;


	//if( (Accel_moy2>Accel_moy3)&&(Accel>Accel_moy2)&&((Accel - Accel_moy3)>(Accel_moy3/2))||(BassDetected>60) ) {
	if( ( (Accel_moy>Accel_moy2)&&(Accel_moy2>Accel_moy3)&&(Accel>Accel_moy)&&((Accel - Accel_moy3)>(Accel_moy3/2)) )||(BassDetected>60) ) {
	//if( ((Accel_moy - Accel_moy3)>(Accel_moy3/BassSensibility))||(BassDetected>60) ) {
	//if( ((Accel - Accel_moy)>(Accel_moy/BassSensibility)) ) {
	//if( ( ((Accel_moy - Accel_moy2)>(Accel_moy2/BassSensibility))&&((Accel - Accel_moy)>(Accel_moy/BassSensibility)) )||(BassDetected>60) ) {
	//if( ((Accel - Accel_moy)>(Accel_moy/BassSensibility))||(BassDetected>60) ) {
		BassDetected = ( ((BassDetected*1.5)>255)?255:(BassDetected*1.5) );
		Accel <<= 1;
		Beat = TRUE;
	}

	//Accel = ( ((Accel - Accel_moy)>(Accel_moy/3))||(BassDetected>60) )?(Accel<<1):Accel;
	//Accel /= ((Accel_moy/40)?(Accel_moy/40):1);
	Accel /= ((Accel_moy/20)?(Accel_moy/20):1);
	
	
	/**
	Accel_moy += Accel;
	Accel_moy >>= 1;
	Accel = 30 + (Accel - Accel_moy2);
	if(Accel<0) Accel = 0;
	else if((Accel - Accel_moy)>(Accel_moy/3)) { Accel <<= 1; Beat = TRUE; }
	Accel /= ((Accel_moy/40)?(Accel_moy/40):1);
	/**/

	beateff = (BeatEffect)?Beat:FALSE;
	beattrans = (BeatTrans)?Beat:FALSE;	
	

	///**/ScreenBack = (USHORT*)SurfaceBackPointer; // possible si surface en mémoire système

	if( (RandomWaveForm)||(RandomCercleWave)||(RandomCercleBass)||(RandomConfettis)||(RandomMathShape)||(RandomRotaLine)||(RandomLittleWave)||(RandomLittleMath)||(RandomGeoGraph)||(RandomMirror) ) {
		if(--ComptEffects < 0) {
			ComptEffects = 140 + (rand()%300);
			ChangeEffects();
		}
	}

	
	if(LoadRandom)
		if(--ComptLoad < 0) {
			char** presets = GetPresets();
			int nb = 0;
			while(presets[nb++]); nb--;
			ComptLoad = 450 + rand()%500;
			init(presets[rand()%nb]);
			
			for(int i = 0; i < nb; i++) free(presets[i]);
			free(presets);
		}
	
	if(Mirror != NOMIRROR) {
		new_xres = ((Mirror == VERMIRROR)||(Mirror == DOUMIRROR))? (XRes>>1) + 2:XRes;
		new_yres = ((Mirror == HORMIRROR)||(Mirror == DOUMIRROR))? (YRes>>1) + 2:YRes;
	}

	
	/**/ // rotoblit
	if(blitrotozoom) {
		//rotoblit(ScreenBack, Screen, blitrotation, blitzoom, Accel, Beat, XRes, YRes);
		rotoblit(ScreenBack, Screen, blitrotation, blitzoom, Accel, (bool)BassDetected, XRes, YRes);
		USHORT* tmp = ScreenBack;
		ScreenBack = Screen; Screen = tmp;
	}




	//rendus avant effets
	if(!NoDisplayTransform)	{
		if(Mirror != NOMIRROR) {
/**/		if(Render == CLEARSCREEN) {
				if(new_xres == XRes) ZeroMemory(Screen, XRes * new_yres * (DepthMode>>3));
				else for(i = j = 0; i < new_yres; i++, j += XRes)
						ZeroMemory(Screen + j, new_xres * (DepthMode>>3));
			}
			else {
				if(Fade) {
					if(new_xres == XRes)
						FadeOut(Screen, XRes, new_yres, Mode555, SupportMMX);
					else
						for(j = i = 0; i < new_yres; i++, j += XRes)
							FadeOut(Screen + j, new_xres + 2, 1, Mode555, SupportMMX);
				}
				else if(!(--decfade) ) {
						decfade = 7; 
						if(new_xres == XRes)
							FadeOut(Screen, XRes, new_yres, Mode555, SupportMMX);
						else
							for(j = i = 0; i < new_yres; i++, j += XRes)
								FadeOut(Screen + j, new_xres + 2, 1, Mode555, SupportMMX);
				}
				
				//if( (Trans.enabled)&&(!TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel + BassDetected, new_xres, new_yres);
				if( (Trans.enabled)&&(!TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel + BassDetected, new_xres, new_yres, beattrans);
				//if( (Trans.enabled)&&(!TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel, new_xres, new_yres, Beat);

				if(!Blend) //{
					switch(Render) { USHORT* tmp;
					case QBLUR :
						if(new_xres == XRes)
							Quick(Screen, XRes, new_yres, Mode555, SupportMMX);
						else for(j = i = 0; i < new_yres; i++, j += XRes)
								Quick(Screen + j, new_xres + 2, 1, Mode555, SupportMMX);
					break;
					case NBLUR :	
						if(new_xres == XRes)
							Normal(ScreenBack, Screen, XRes, new_yres, Mode555, SupportMMX);
						else {	
							Normal(ScreenBack, Screen, XRes, 0, Mode555, SupportMMX, new_xres + 2);
							int max = new_yres - 1;
							if(new_yres == YRes) { ZeroMemory(ScreenBack + (YRes - 1) * XRes, new_xres * sizeof(short)); max = new_yres - 3; }
							for(i = 0, j = XRes; i < max; i++, j += XRes)
								Normal(ScreenBack + j, Screen + j, XRes, 0, Mode555, SupportMMX, new_xres + 2);
							ZeroMemory(ScreenBack, new_xres * sizeof(short));
							vertical_ligne(ScreenBack, 0, 0, new_yres, 0, XRes);
						}
						tmp = ScreenBack;
						ScreenBack = Screen; Screen = tmp;
					break;
					case SBLUR :
						if(new_xres == XRes)
							Slow(ScreenBack, Screen, XRes, new_yres, Mode555, SupportMMX);
						else {	
							Slow(ScreenBack, Screen, XRes, 0, Mode555, SupportMMX, new_xres + 2);
							int max = new_yres - 1;
							if(new_yres == YRes) { ZeroMemory(ScreenBack + (YRes - 1) * XRes, new_xres * sizeof(short)); max = new_yres - 3; }
							for(i = 0, j = XRes; i < max; i++, j += XRes)
								Slow(ScreenBack + j, Screen + j, XRes, 0, Mode555, SupportMMX, new_xres + 2);
							ZeroMemory(ScreenBack, new_xres * sizeof(short));
							vertical_ligne(ScreenBack, 0, 0, new_yres, 0, XRes);
						}
						tmp = ScreenBack;
						ScreenBack = Screen; Screen = tmp;
				}
			}
			
		}
		else {	// pas de mirror
/**/		if(Render == CLEARSCREEN) ZeroMemory(Screen, XRes * YRes * (DepthMode>>3));
			else {
				if(Fade) FadeOut(Screen, XRes, YRes, Mode555, SupportMMX);
				else if(!(--decfade) ) {
						decfade = 7; 
						FadeOut(Screen, XRes, YRes, Mode555, SupportMMX);
				}
			
				if( (Trans.enabled)&&(!TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel + BassDetected, 0, 0, beattrans);

				if(!Blend)
					switch(Render) { USHORT* tmp;
					case QBLUR : Quick(Screen, XRes, YRes, Mode555, SupportMMX); break;
					case NBLUR :
						Normal(ScreenBack, Screen, XRes, YRes, Mode555, SupportMMX);
						tmp = ScreenBack;
						ScreenBack = Screen; Screen = tmp;
						break;
					case SBLUR :
						Slow(ScreenBack, Screen, XRes, YRes, Mode555, SupportMMX);
						tmp = ScreenBack;
						ScreenBack = Screen; Screen = tmp;
					}
/**/		}
		
		}
	} // fin if(NoDisplayTransform)
	//else dep_x = dep_y = 0;

	if( (Trans.enabled)&&(Mirror != NOMIRROR) ) {	// on efface les parties non visibles suceptibles d'êtres transformées
			int i;
			int mi_x = (XRes>>1) + 1, t_x = (XRes>>1) + (XRes>>2);
			int mi_y = (YRes>>1) + 1, t_y = (YRes>>1) + (YRes>>2), q_y = (YRes>>2);
			//int mi_x = (XRes>>1) + 1, t_x = (XRes>>1) + (XRes>>2), q_x = (XRes>>2), e_x = XRes -1;
			//int mi_y = (YRes>>1) + 1, t_y = (YRes>>1) + (YRes>>2), q_y = (YRes>>2), e_y = YRes -1;
			//int yres2 = YRes*YRes, t_x2 = ((t_x - mi_x)<<1), q_y2 = q_y*YRes, t_y2 = t_y*YRes, mi_y2 = mi_y*YRes, t_x3 = (t_x<<1);

			if((new_xres != XRes)&&(new_yres == YRes)) {
				
				for(i = 0; i < YRes; i++) horizontal_ligne(Screen, i, mi_x, t_x, 0, XRes); 
				for(i = 0; i < YRes; i++) horizontal_ligne(ScreenBack, i, mi_x, t_x, 0, XRes); 
								
				//for(i = 0; i < yres2; i += YRes) ZeroMemory(Screen + i + mi_x, t_x2);
				//for(i = 0; i < yres2; i += YRes) ZeroMemory(ScreenBack + i + mi_x, t_x2);
			}
			else if((new_yres != YRes)&&(new_xres == XRes)) { 
				ZeroMemory(Screen + XRes * mi_y, XRes * mi_y);
				ZeroMemory(ScreenBack + XRes * mi_y, XRes * mi_y);
			}
			else if((new_yres != YRes)&&(new_xres != XRes)) {
				for(i = 0; i < q_y; i++) horizontal_ligne(Screen, mi_y + i, 0, t_x, 0, XRes); 
				for(i = 0; i < t_y; i++) horizontal_ligne(Screen, i, mi_x, t_x, 0, XRes); 
				for(i = 0; i < q_y; i++) horizontal_ligne(ScreenBack, mi_y + i, 0, t_x, 0, XRes); 
				for(i = 0; i < t_y; i++) horizontal_ligne(ScreenBack, i, mi_x, t_x, 0, XRes); 
				//for(i = 0; i < q_y2; i += YRes) ZeroMemory(Screen + mi_y2 + i, t_x3); 
				//for(i = 0; i < t_y2; i += YRes) ZeroMemory(Screen + i, t_x2); 
				//for(i = 0; i < q_y2; i += YRes) ZeroMemory(ScreenBack + mi_y2 + i, t_x3);
				//for(i = 0; i < t_y2; i += YRes) ZeroMemory(ScreenBack + i, t_x2); 
			}
	}
	
	
	//effets
	for(i = 0; i < WaveFormEff::nombre; i++)
		WaveForm[i]->Render(Screen, XRes, YRes, module.waveformData[0], Mode555, Accel, Flashis, beateff);

	for(i = 0; i < CercleWaveEff::nombre; i++)
		CercleWave[i]->Render(Screen, XRes, YRes, module.waveformData[0], Mode555, Accel, Flashis, beateff);

	for(i = 0; i < CercleBassEff::nombre; i++)
		CercleBass[i]->Render(Screen, BassDetected, XRes, YRes, Mode555, Flashis);

	for(i = 0; i < ConfettisEff::nombre; i++)
		Confettis[i]->Render(Screen, XRes, YRes, Mode555, ((Accel + BassDetected)>>1), Flashis, beateff);

	for(i = 0; i < MathShapeEff::nombre; i++)
		MathShape[i]->Render(Screen, XRes, YRes, module.waveformData[0], Mode555, Accel, Flashis, beateff);

	for(i = 0; i < RotaLineEff::nombre; i++)
		RotaLine[i]->Render(Screen, XRes, YRes, module.waveformData[0], Mode555, Accel, Flashis, beateff);

	for(i = 0; i < LittleWaveEff::nombre; i++)
		LittleWave[i]->Render(Screen, XRes, YRes, module.waveformData[0], Mode555, Accel, Flashis, beateff);
	
	for(i = 0; i < LittleMathEff::nombre; i++)
		LittleMath[i]->Render(Screen, XRes, YRes, Mode555, Accel, Flashis, beateff);
	
	for(i = 0; i < GeoGraphEff::nombre; i++)
		GeoGraph[i]->Render(Screen, XRes, YRes, Mode555, Accel, Flashis, beateff);
	
	//rendus après effets
	if(!NoDisplayTransform) {
		if(Mirror != NOMIRROR) {
			if(Blend) {
				switch(Render) { USHORT* tmp;
				case QBLUR :
					if(new_xres == XRes)
						Quick(Screen, XRes, new_yres, Mode555, SupportMMX);
					else
						for(j = i = 0; i < new_yres; i++, j += XRes)
							Quick(Screen + j, new_xres + 2, 1, Mode555, SupportMMX);
					break;
				case NBLUR :
					if(new_xres == XRes)
						Normal(ScreenBack, Screen, XRes, new_yres, Mode555, SupportMMX);
					else {	
						Normal(ScreenBack, Screen, XRes, 0, Mode555, SupportMMX, new_xres + 2);
						int max = new_yres - 1;
						if(new_yres == YRes) { ZeroMemory(ScreenBack + (YRes - 1) * XRes, new_xres * sizeof(short)); /*max = new_yres - 2;*/max = new_yres - 3; }
						for(i = 0, j = XRes; i < max; i++, j += XRes)
							Normal(ScreenBack + j, Screen + j, XRes, 0, Mode555, SupportMMX, new_xres + 2);
						ZeroMemory(ScreenBack, new_xres * sizeof(short));
						vertical_ligne(ScreenBack, 0, 0, new_yres, 0, XRes);
					}
					tmp = ScreenBack;
					ScreenBack = Screen; Screen = tmp;
					break;
				case SBLUR :
					if(new_xres == XRes)
						Slow(ScreenBack, Screen, XRes, new_yres, Mode555, SupportMMX);
					else {	
						Slow(ScreenBack, Screen, XRes, 0, Mode555, SupportMMX, new_xres + 2);
						int max = new_yres - 1;
						if(new_yres == YRes) { ZeroMemory(ScreenBack + (YRes - 1) * XRes, new_xres * sizeof(short)); /*max = new_yres - 2;*/max = new_yres - 3; }
						for(i = 0, j = XRes; i < max; i++, j += XRes)
							Slow(ScreenBack + j, Screen + j, XRes, 0, Mode555, SupportMMX, new_xres + 2);
						ZeroMemory(ScreenBack, new_xres * sizeof(short));
						vertical_ligne(ScreenBack, 0, 0, new_yres, 0, XRes);
					}
					tmp = ScreenBack;
					ScreenBack = Screen; Screen = tmp;
				}
			}
			
			if( (Trans.enabled)&&(TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel + BassDetected, new_xres, new_yres, beattrans);
		}
		else { // pas de mirror
			if(Blend)
				switch(Render) { USHORT* tmp;
				case QBLUR : Quick(Screen, XRes, YRes, Mode555, SupportMMX); break;
				case NBLUR :
					Normal(ScreenBack, Screen, XRes, YRes, Mode555, SupportMMX);
					tmp = ScreenBack;
					ScreenBack = Screen; Screen = tmp;
					break;
				case SBLUR :
					Slow(ScreenBack, Screen, XRes, YRes, Mode555, SupportMMX);
					tmp = ScreenBack;
					ScreenBack = Screen; Screen = tmp;
				}

			if( (Trans.enabled)&&(TransBefore) ) Trans.Render(&ScreenBack, &Screen, XRes, YRes, SupportMMX, Accel + BassDetected, 0, 0, beattrans);
		}
	} // fin if(!NoDisplayTransform)


	return HRet;
}



//UCHAR Space::DetectBass()
UCHAR Space::DetectBass()
{ 
	const int IMP = 1;
	const int SEUIL = 250;
	const int RANGE = 6;
	
	/* méthode 1: détection de valeur dans la bande basse au dessus du seuil : bof */
	static int old_valbasse = 0;
	static int old_max2 = 0;
	static int old_max = 0;
	static int old_compt = 0;
	static int compt = 5;
	/**static int rythme = 0;/**/
	int valbasse = 0;
	int max = 0;
	int ch_bass = 0, ch_bass2 = 0;

	for(int i = 0; i < RANGE; i++)
	{
		//if(module.spectrumData[0][i] > SEUIL) max++;
		if(module.spectrumData[0][i] > max) max = module.spectrumData[0][i];
		valbasse += (unsigned)module.spectrumData[0][i];
	}

	// on peut paramétre le seuil de détection des basse (ici 250)
	// "l'importance" (l'étendue) de la basse (ici 1)
	// et enfin la bande (largeur) de détection (ici 4)
	valbasse >>= 3; 
	valbasse -= 20;
	
	compt++;
	if(max>SEUIL) ch_bass++;
	else if(max>((old_max*3)/2)) ch_bass2++;
	if(old_max>(SEUIL>>1)) ch_bass++;
	else if(old_max>(old_max2<<1)) ch_bass2++;
	if(old_max2>(SEUIL>>2)) ch_bass++;
	else if(old_max2>(SEUIL>>3)) ch_bass2++;
	if( fabsolue(float((old_compt - compt)*8)/float(old_compt + 1.0)) < .7) {
		ch_bass++; ch_bass2++; /**rythme++;/**/ }
	else if(compt>70) { ch_bass++; ch_bass2++; /**rythme = 0;/**/ }
	/**if(rythme > 4) { ch_bass++; ch_bass2++; }/**/
	if( ( (ch_bass >= 3)||(ch_bass2 >= 3) )&&(compt>18)&&(valbasse > 0) ) {	//if(ch_bass >= 3) {
		valbasse += 65;
		old_compt = compt;
		compt = 0;
	} else valbasse = 0;
	valbasse = (valbasse>128)?128:valbasse;
	old_max2 = old_max;
	old_max = max;
	old_valbasse = valbasse;
	return UCHAR(valbasse);
	/**/

	if( (max >= IMP)&&(old_valbasse == 0) ) old_valbasse = valbasse;
	else old_valbasse = 0;
	
	return UCHAR(old_valbasse);
}

int Space::DisplayRender()
{  
	HRESULT hRet = DD_OK;			// valeur de retour des fonctions d'initialisation ddraw
	RECT rectdest;					// coordonnées rectangulaires des zones à "bliter"
	USHORT* src = Screen, *dest;
	int i, xres = (XRes<<1);
	LPDIRECTDRAWSURFACE4 dd_surf;
	LONG pitch;

	static ULONG N_Time = 0;
	static int Num_Fr = 0;
	static char Str_Tmp[32] = "";

	switch(ViewMode) {
	case FULLSCREEN: case BACKGROUND: {
	
			if(ViewMode == FULLSCREEN) {
				pitch = (SurfacePrimaryPitch>>1);
				dest = (USHORT*)SurfacePrimPointer;
				dd_surf = lpDDSPrimary;
			}
			else {
				pitch = (SurfaceBackPitch>>1);
				dest = (USHORT*) SurfaceBackPointer;
				dd_surf = lpDDSBack;
			}
		
			switch(Mirror) {
			default: case NOMIRROR:
				if(pitch == XRes) memcpy(dest, src, XRes * YRes * (DepthMode >> 3));
				else for(i = 0; i < YRes ; i++, dest += pitch, src += XRes)
						memcpy(dest, src, xres); 
				break;
			case HORMIRROR: Horizontal(dest, src, XRes, YRes, pitch); break;
			case VERMIRROR: Vertical(dest, src, XRes, YRes, pitch); break;
			case DOUMIRROR: Double(dest, src, XRes, YRes, pitch); 
			}
		}
		break;
	default: {	// docked ou windowed
			/**/
			POINT point;						// coordonnées du coin supérieur gauche de la one client de la fenêtre
			point.x = 0; point.y = 0;
			GetClientRect(hMainWnd, &rectdest);	// on récupère les coordonnées rectangulaire de la zone cliente en non-absolue
			ClientToScreen(hMainWnd, &point);	// on récupère les coordonnées absolues du coin supérieur gauche de cette zone
			OffsetRect(&rectdest, point.x, point.y);	// et on rend absolue les coordonnées de la zone cliente 
			/**/

			// copie sur la surface secondaire
			DDSURFACEDESC2 ddsd;			// descripteur de surface directdraw (structure)
			ZeroMemory(&ddsd, sizeof(ddsd));
			ddsd.dwSize = sizeof( ddsd );	
			hRet = lpDDSBack->Lock(NULL, &ddsd,  DDLOCK_WAIT, NULL);
			if(hRet != DD_OK) return Abandon(hRet, "Lock surface back:", TRUE);
			dest = (USHORT*)ddsd.lpSurface;  // screen pointe sur la surface directdraw (primaire)
			pitch = (SurfaceBackPitch>>1);

			switch(Mirror) {
			default: case NOMIRROR: 
				if(pitch == XRes) memcpy(dest, src, XRes * YRes * (DepthMode >> 3));
				else for(i = 0; i < YRes ; i++, dest += pitch, src += XRes)
						memcpy(dest, src, xres); 
				break;
			case HORMIRROR: Horizontal(dest, src, XRes, YRes, pitch); break;
			case VERMIRROR: Vertical(dest, src, XRes, YRes, pitch); break;
			case DOUMIRROR: Double(dest, src, XRes, YRes, pitch); 
			}

			//hRet = lpDDSBack->Unlock(ddsd.lpSurface);
			hRet = lpDDSBack->Unlock(NULL);
			if(hRet != DD_OK)  return Abandon(hRet, "DéLock surface back:", TRUE);
			dd_surf = lpDDSBack;
		}
	}
			
	if((ShowFPS)&&(XRes > 80)&&(YRes > 20)) {
		if (++Num_Fr >= 16)
		{
			Num_Fr = 0;
			ULONG tmp2 = (GetTickCount() - N_Time);
			N_Time = GetTickCount();
			if (tmp2>0) {
				wsprintf(Str_Tmp, "%d Fps", 16000 / tmp2);
		// pourquoi ce putain de nb de fps est faux ???
		//		wsprintf(Str_Tmp, "%d Fps", 18500 / tmp2);
			} else wsprintf(Str_Tmp, "16000+ Fps");
		}

		
		HDC surfaceDC;
		dd_surf->GetDC(&surfaceDC);
		SetTextColor(surfaceDC, RGB(255,255,255));
		SetBkMode(surfaceDC, TRANSPARENT);  
		TextOut(surfaceDC, 0, 0, Str_Tmp, strlen(Str_Tmp));
		/**
		char ch[30];
		wsprintf(ch, "Accel = %d", Accel);
		TextOut(surfaceDC, 0, 20, ch, strlen(ch));
		/**/
		dd_surf->ReleaseDC(surfaceDC);
	} 

	if( (ViewMode == WINDOWED)||(ViewMode == DOCKED))
		hRet = lpDDSPrimary->Blt(&rectdest, lpDDSBack, NULL, DDBLT_WAIT , NULL);
		
	if( (hRet != DD_OK)&&(hRet != DDERR_INVALIDRECT) ) return Abandon(hRet, "Blit:", TRUE);
	
	if(ViewMode == DOCKED) SetRectDock(DockMode);

	return 0;
}


void Space::SetConf(char* fname/* char* section*/)
{	// utilise CheckConf
	char* ini_file = (char*)malloc(MAX_PATH * sizeof(char));
	
	GetIniConf(ini_file);
	strcat(ini_file, fname);
	strcat(ini_file, ".spc");

  	Blend = GetPrivateProfileInt("Space Preset", "Blend", Blend, ini_file);
	TransBefore = GetPrivateProfileInt("Space Preset", "Trans_Before", TransBefore, ini_file);
	Mirror = GetPrivateProfileInt("Space Preset", "Mirror", Mirror, ini_file);

	blitrotozoom = GetPrivateProfileInt("Space Preset", "Blit_RotoZoom", blitrotozoom, ini_file);
	blitrotation = GetPrivateProfileInt("Space Preset", "Blit_Rotation", blitrotation, ini_file);
	blitzoom = GetPrivateProfileInt("Space Preset", "Blit_Zoom", blitzoom, ini_file);
	
// initialisation des objets
	//Effets
	int i, NbEff;

	RandomMirror = GetPrivateProfileInt("Space Preset", "Random_Mirror", RandomMirror, ini_file);
	Flashis =  GetPrivateProfileInt("Space Preset", "Flashis", Flashis, ini_file);
	
	RandomWaveForm = GetPrivateProfileInt("Space Preset", "Random_WaveForm", RandomWaveForm, ini_file);
	RandomCercleWave = GetPrivateProfileInt("Space Preset", "Random_CercleWave", RandomCercleWave, ini_file);
	RandomCercleBass = GetPrivateProfileInt("Space Preset", "Random_CercleBass", RandomCercleBass, ini_file);
	RandomConfettis = GetPrivateProfileInt("Space Preset", "Random_Confettis", RandomConfettis, ini_file);
	RandomMathShape = GetPrivateProfileInt("Space Preset", "Random_MathShape", RandomMathShape, ini_file);
	RandomRotaLine = GetPrivateProfileInt("Space Preset", "Random_RotaLine", RandomRotaLine, ini_file);
	RandomLittleWave = GetPrivateProfileInt("Space Preset", "Random_LittleWave", RandomLittleWave, ini_file);
	RandomLittleMath = GetPrivateProfileInt("Space Preset", "Random_LittleMath", RandomLittleMath, ini_file);
	RandomGeoGraph = GetPrivateProfileInt("Space Preset", "Random_GeoGraph", RandomGeoGraph, ini_file);

	//WaveForm
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_WaveForm", WaveFormEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		WaveForm[i] = new WaveFormEff(WaveForm);

	//CercleWave
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_CercleWave", CercleWaveEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		CercleWave[i] = new CercleWaveEff(CercleWave);
	
	//CercleBass
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_CercleBass", CercleBassEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		CercleBass[i] = new CercleBassEff();
		
	//Confettis
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_Confettis", ConfettisEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		Confettis[i] = new ConfettisEff();
	
	//MathShape
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_MathShape", MathShapeEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		MathShape[i] = new MathShapeEff();
	
	//RotaLine
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_RotaLine", RotaLineEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		RotaLine[i] = new RotaLineEff(RotaLine);
	
	//LittleWave
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_LittleWave", LittleWaveEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		LittleWave[i] = new LittleWaveEff();
	
	//LittleMath
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_LittleMath", LittleMathEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		LittleMath[i] = new LittleMathEff();
	
	//GeoGraph
	NbEff= GetPrivateProfileInt("Space Preset", "Nb_GeoGraph", GeoGraphEff::nombre, ini_file);
	for(i = 0; i < NbEff; i++)
		GeoGraph[i] = new GeoGraphEff(GeoGraph);
	
	//Rendu et Transformation
	Fade = GetPrivateProfileInt("Space Preset", "Fade", Fade, ini_file);
	Render = GetPrivateProfileInt("Space Preset", "Render", Render, ini_file);
	Trans.enabled = GetPrivateProfileInt("Space Preset", "Trans", Trans.enabled, ini_file);
	Trans.random = GetPrivateProfileInt("Space Preset", "Random_Trans", Trans.random, ini_file);
	Trans.slide = GetPrivateProfileInt("Space Preset", "Trans_Slide", Trans.slide, ini_file);
	Trans.noise = GetPrivateProfileInt("Space Preset", "Trans_Noise", Trans.noise, ini_file);
	Trans.type = GetPrivateProfileInt("Space Preset", "Trans_Type", Trans.type, ini_file);
	Trans.shift = GetPrivateProfileInt("Space Preset", "Trans_Shift", Trans.shift, ini_file);
	//Trans.fast = GetPrivateProfileInt("Space Preset", "Trans_Fast", Trans.fast, ini_file);
	
//	OnBassChange = GetPrivateProfileInt("Space Preset", "On_Bass_Change", OnBassChange, ini_file);
	BeatEffect = GetPrivateProfileInt("Space Preset", "Beat_Effect", BeatEffect, ini_file);
	BeatTrans = GetPrivateProfileInt("Space Preset", "Beat_Trans", BeatTrans, ini_file);
	//detection MMX
	i = 0; _asm {
		mov eax, 1			
		cpuid				
		and edx, 00800000h	
		mov i, edx			
	}
	SupportMMX = (i)?TRUE:FALSE;
	//**/SupportMMX = FALSE;

	free(ini_file);
}


void Space::CheckConf()
{
	if ((XWin > 2048) || (YWin > 1600) || (XWin < 0) || (YWin < 0))
	{ XWin = 50; YWin = 50; }
	
	if(WidthWin > 2048) WidthWin = 2048;
	if(WidthWin < 16) WidthWin = 16;
	if(HeightWin > 1600) HeightWin = 1600;
	if(HeightWin < 16) HeightWin = 16;
	
	if(WidthMode > MAX_WIDTH) WidthMode = MAX_WIDTH;
	if(WidthMode < 320) WidthMode = 320;
	if(HeightMode > MAX_HEIGHT) HeightMode = MAX_HEIGHT;
	if(HeightMode < 200) HeightMode = 200;
	
	if(WidthBack > MAX_WIDTH) WidthBack = MAX_WIDTH;
	if(WidthBack < 160) WidthBack = 160;
	if(HeightBack > MAX_HEIGHT) HeightBack = MAX_HEIGHT;
	if(HeightBack < 100) HeightBack = 100;
	
	if(XRes > MAX_WIDTH) XRes = MAX_WIDTH;
	if(XRes < 16) XRes = 16;
	if(YRes > MAX_HEIGHT) YRes = MAX_HEIGHT;
	if(YRes < 16) YRes = 16;
	
	if((ResMode < AUTORES)||(ResMode > DEFINEDRES)) ResMode = DEFINEDRES;

	switch(DepthMode) {	// sert à rien ce truc ...
	//case 8 :
	//case 24 :
	//case 32 :
	case 16 : break;
	default: DepthMode = 16;
	}

	switch(DockMode) {
	case BOTTOMDOCK : case LEFTDOCK : case RIGHTDOCK : case TOPDOCK : case LITTLEDOCK : break;
	default: DockMode = BOTTOMDOCK;
	}

	switch(ViewMode) {
	case FULLSCREEN :  case DOCKED : case WINDOWED : case BACKGROUND : break;
	default : ViewMode  = WINDOWED;
	}

	switch(Mirror) {
	case NOMIRROR: case HORMIRROR: case VERMIRROR: case DOUMIRROR: break;
	default: Mirror = NOMIRROR;
	}

	if( (Speed!=SPEED20)&&(Speed!=SPEED30)&&(Speed!=SPEED45)&&(Speed!=SPEED60) ) Speed = SPEEDMAX;
}

	
void Space::SaveConf(char* fname)
{
	char* ini_file = (char*)malloc(MAX_PATH * sizeof(char));
	char string[32];

	GetIniConf(ini_file);
	strcat(ini_file, fname);
	strcat(ini_file, ".spc");

	wsprintf(string,"%d",Blend);
	if(!WritePrivateProfileString("Space Preset","Blend",string,ini_file))	{
		MessageBox(hMainWnd, "Config cannot be saved !", "Error", MB_OK);
		return;
	}
	wsprintf(string,"%d",TransBefore);
	WritePrivateProfileString("Space Preset","Trans_Before",string,ini_file);
	wsprintf(string,"%d",Mirror);
	WritePrivateProfileString("Space Preset","Mirror",string,ini_file);
	wsprintf(string,"%d",RandomWaveForm);
	WritePrivateProfileString("Space Preset","Random_WaveForm",string,ini_file);
	wsprintf(string,"%d",RandomCercleWave);
	WritePrivateProfileString("Space Preset","Random_CercleWave",string,ini_file);
	wsprintf(string,"%d",RandomCercleBass);
	WritePrivateProfileString("Space Preset","Random_CercleBass",string,ini_file);
	wsprintf(string,"%d",RandomConfettis);
	WritePrivateProfileString("Space Preset","Random_Confettis",string,ini_file);
	wsprintf(string,"%d",RandomMathShape);
	WritePrivateProfileString("Space Preset","Random_MathShape",string,ini_file);
	wsprintf(string,"%d",RandomRotaLine);
	WritePrivateProfileString("Space Preset","Random_RotaLine",string,ini_file);
	wsprintf(string,"%d",RandomLittleWave);
	WritePrivateProfileString("Space Preset","Random_LittleWave",string,ini_file);
	wsprintf(string,"%d",RandomLittleMath);
	WritePrivateProfileString("Space Preset","Random_LittleMath",string,ini_file);
	wsprintf(string,"%d",RandomGeoGraph);
	WritePrivateProfileString("Space Preset","Random_GeoGraph",string,ini_file);

	wsprintf(string,"%d",WaveFormEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_WaveForm",string,ini_file);
	wsprintf(string,"%d",CercleWaveEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_CercleWave",string,ini_file);
	wsprintf(string,"%d",CercleBassEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_CercleBass",string,ini_file);
	wsprintf(string,"%d",ConfettisEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_Confettis",string,ini_file);
	wsprintf(string,"%d",MathShapeEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_MathShape",string,ini_file);
	wsprintf(string,"%d",RotaLineEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_RotaLine",string,ini_file);
	wsprintf(string,"%d",LittleWaveEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_LittleWave",string,ini_file);
	wsprintf(string,"%d",LittleMathEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_LittleMath",string,ini_file);
	wsprintf(string,"%d",GeoGraphEff::nombre);
	WritePrivateProfileString("Space Preset","Nb_GeoGraph",string,ini_file);
	
	wsprintf(string,"%d",Flashis);
	WritePrivateProfileString("Space Preset","Flashis",string,ini_file);
	wsprintf(string,"%d",RandomMirror);
	WritePrivateProfileString("Space Preset","Random_Mirror",string,ini_file);
	//wsprintf(string,"%d",OnBassChange);
	//WritePrivateProfileString("Space Preset","On_Bass_Change",string,ini_file);
	wsprintf(string,"%d",BeatEffect);
	WritePrivateProfileString("Space Preset","Beat_Effect",string,ini_file);
	wsprintf(string,"%d",BeatTrans);
	WritePrivateProfileString("Space Preset","Beat_Trans",string,ini_file);

	wsprintf(string,"%d",Fade);
	WritePrivateProfileString("Space Preset","Fade",string,ini_file);
	wsprintf(string,"%d",Render);
	WritePrivateProfileString("Space Preset","Render",string,ini_file);
	
	wsprintf(string,"%d",Trans.enabled);
	WritePrivateProfileString("Space Preset","Trans",string,ini_file);
	wsprintf(string,"%d",Trans.random);
	WritePrivateProfileString("Space Preset","Random_Trans",string,ini_file);
	wsprintf(string,"%d",Trans.slide);
	WritePrivateProfileString("Space Preset","Trans_Slide",string,ini_file);
	wsprintf(string,"%d",Trans.noise);
	WritePrivateProfileString("Space Preset","Trans_Noise",string,ini_file);
	wsprintf(string,"%d",Trans.type);
	WritePrivateProfileString("Space Preset","Trans_Type",string,ini_file);
	wsprintf(string,"%d",Trans.shift);
	WritePrivateProfileString("Space Preset","Trans_Shift",string,ini_file);
	//wsprintf(string,"%d",Trans.fast);
	//WritePrivateProfileString("Space Preset","Trans_Fast",string,ini_file);

	wsprintf(string,"%d",blitrotozoom);
	WritePrivateProfileString("Space Preset","Blit_RotoZoom",string,ini_file);
	wsprintf(string,"%d",blitrotation);
	WritePrivateProfileString("Space Preset","Blit_Rotation",string,ini_file);
	wsprintf(string,"%d",blitzoom);
	WritePrivateProfileString("Space Preset","Blit_Zoom",string,ini_file);

	free(ini_file);
}

	
int Space::Abandon(LONG error, char *mes, int DDError)
{
	char *ch;
	ch = new char[strlen(mes) + 150];
	strcpy(ch, mes);

	if(DDError) {
		switch(error) {
		//les  importants
		case DDERR_NOOVERLAYHW : strcat(ch, "Pas de support hardware overlay"); break;
		case DDERR_NOCOLORKEYHW : strcat(ch, "Pas de support hardware destination color key"); break;
	
		//les autres
        case DDERR_ALREADYINITIALIZED : strcat(ch, "DDERR_ALREADYINITIALIZED"); break; 
	    case DDERR_CANNOTATTACHSURFACE : strcat(ch, "DDERR_CANNOTATTACHSURFACE"); break; 
        case DDERR_CANNOTDETACHSURFACE : strcat(ch, "DDERR_CANNOTDETACHSURFACE"); break; 
        case DDERR_CURRENTLYNOTAVAIL : strcat(ch, "DDERR_CURRENTLYNOTAVAIL"); break; 
        case DDERR_EXCEPTION : strcat(ch, "DDERR_EXCEPTION"); break; 
        case DDERR_GENERIC : strcat(ch, "DDERR_GENERIC"); break; 
        case DDERR_HEIGHTALIGN : strcat(ch, "DDERR_HEIGHTALIGN"); break; 
        case DDERR_INCOMPATIBLEPRIMARY : strcat(ch, "DDERR_INCOMPATIBLEPRIMARY"); break; 
        case DDERR_INVALIDCAPS : strcat(ch, "DDERR_INVALIDCAPS"); break; 
        case DDERR_INVALIDCLIPLIST : strcat(ch, "DDERR_INVALIDCLIPLIST"); break; 
        case DDERR_INVALIDMODE : strcat(ch, "DDERR_INVALIDMODE"); break; 
        case DDERR_INVALIDOBJECT : strcat(ch, "DDERR_INVALIDOBJECT"); break; 
        case DDERR_INVALIDPARAMS : strcat(ch, "DDERR_INVALIDPARAMS"); break; 
        case DDERR_INVALIDPIXELFORMAT : strcat(ch, "DDERR_INVALIDPIXELFORMAT"); break; 
        case DDERR_INVALIDRECT : strcat(ch, "DDERR_INVALIDRECT"); break; 
        case DDERR_LOCKEDSURFACES : strcat(ch, "DDERR_LOCKEDSURFACES"); break; 
        case DDERR_NO3D : strcat(ch, "DDERR_NO3D"); break; 
        case DDERR_NOALPHAHW : strcat(ch, "DDERR_NOALPHAHW"); break; 
        case DDERR_NOCLIPLIST : strcat(ch, "DDERR_NOCLIPLIST"); break; 
        case DDERR_NOCOLORCONVHW : strcat(ch, "DDERR_NOCOLORCONVHW"); break; 
        case DDERR_NOCOOPERATIVELEVELSET : strcat(ch, "DDERR_NOCOOPERATIVELEVELSET"); break; 
        case DDERR_NOCOLORKEY : strcat(ch, "DDERR_NOCOLORKEY"); break; 
        case DDERR_NODIRECTDRAWSUPPORT : strcat(ch, "DDERR_NODIRECTDRAWSUPPORT"); break; 
        case DDERR_NOEXCLUSIVEMODE : strcat(ch, "DDERR_NOEXCLUSIVEMODE"); break; 
        case DDERR_NOFLIPHW : strcat(ch, "DDERR_NOFLIPHW"); break; 
        case DDERR_NOGDI : strcat(ch, "DDERR_NOGDI"); break; 
        case DDERR_NOMIRRORHW : strcat(ch, "DDERR_NOMIRRORHW"); break; 
        case DDERR_NOTFOUND : strcat(ch, "DDERR_NOTFOUND"); break; 
        case DDERR_NORASTEROPHW : strcat(ch, "DDERR_NORASTEROPHW"); break; 
        case DDERR_NOROTATIONHW : strcat(ch, "DDERR_NOROTATIONHW"); break; 
	    case DDERR_NOSTRETCHHW : strcat(ch, "DDERR_NOSTRETCHHW"); break; 
        case DDERR_NOT4BITCOLOR : strcat(ch, "DDERR_NOT4BITCOLOR"); break; 
        case DDERR_NOT4BITCOLORINDEX : strcat(ch, "DDERR_NOT4BITCOLORINDEX"); break; 
        case DDERR_NOT8BITCOLOR : strcat(ch, "DDERR_NOT8BITCOLOR"); break; 
        case DDERR_NOTEXTUREHW : strcat(ch, "DDERR_NOTEXTUREHW"); break; 
        case DDERR_NOVSYNCHW : strcat(ch, "DDERR_NOVSYNCHW"); break; 
        case DDERR_NOZBUFFERHW : strcat(ch, "DDERR_NOZBUFFERHW"); break; 
        case DDERR_NOZOVERLAYHW : strcat(ch, "DDERR_NOZOVERLAYHW"); break; 
        case DDERR_OUTOFCAPS : strcat(ch, "DDERR_OUTOFCAPS"); break; 
        case DDERR_OUTOFMEMORY : strcat(ch, "DDERR_OUTOFMEMORY"); break; 
        case DDERR_OUTOFVIDEOMEMORY : strcat(ch, "DDERR_OUTOFVIDEOMEMORY"); break; 
        case DDERR_OVERLAYCANTCLIP : strcat(ch, "DDERR_OVERLAYCANTCLIP"); break; 
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE : strcat(ch, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"); break; 
        case DDERR_PALETTEBUSY : strcat(ch, "DDERR_PALETTEBUSY"); break; 
        case DDERR_COLORKEYNOTSET : strcat(ch, "DDERR_COLORKEYNOTSET"); break; 
        case DDERR_SURFACEALREADYATTACHED : strcat(ch, "DDERR_SURFACEALREADYATTACHED"); break; 
        case DDERR_SURFACEALREADYDEPENDENT : strcat(ch, "DDERR_SURFACEALREADYDEPENDENT"); break; 
        case DDERR_SURFACEBUSY : strcat(ch, "DDERR_SURFACEBUSY"); break; 
        case DDERR_CANTLOCKSURFACE : strcat(ch, "DDERR_CANTLOCKSURFACE"); break; 
        case DDERR_SURFACEISOBSCURED : strcat(ch, "DDERR_SURFACEISOBSCURED"); break; 
        case DDERR_SURFACELOST : strcat(ch, "DDERR_SURFACELOST"); break; 
        case DDERR_SURFACENOTATTACHED : strcat(ch, "DDERR_SURFACENOTATTACHED"); break; 
        case DDERR_TOOBIGHEIGHT : strcat(ch, "DDERR_TOOBIGHEIGHT"); break; 
        case DDERR_TOOBIGSIZE : strcat(ch, "DDERR_TOOBIGSIZE"); break; 
        case DDERR_TOOBIGWIDTH : strcat(ch, "DDERR_TOOBIGWIDTH"); break; 
        case DDERR_UNSUPPORTED : strcat(ch, "DDERR_UNSUPPORTED"); break; 
        case DDERR_UNSUPPORTEDFORMAT : strcat(ch, "DDERR_UNSUPPORTEDFORMAT"); break; 
        case DDERR_UNSUPPORTEDMASK : strcat(ch, "DDERR_UNSUPPORTEDMASK"); break; 
        case DDERR_VERTICALBLANKINPROGRESS : strcat(ch, "DDERR_VERTICALBLANKINPROGRESS"); break; 
        case DDERR_WASSTILLDRAWING : strcat(ch, "DDERR_WASSTILLDRAWING"); break; 
        case DDERR_XALIGN : strcat(ch, "DDERR_XALIGN"); break; 
        case DDERR_INVALIDDIRECTDRAWGUID : strcat(ch, "DDERR_INVALIDDIRECTDRAWGUID"); break; 
        case DDERR_DIRECTDRAWALREADYCREATED : strcat(ch, "DDERR_DIRECTDRAWALREADYCREATED"); break; 
        case DDERR_NODIRECTDRAWHW : strcat(ch, "DDERR_NODIRECTDRAWHW"); break; 
        case DDERR_PRIMARYSURFACEALREADYEXISTS : strcat(ch, "DDERR_PRIMARYSURFACEALREADYEXISTS"); break; 
        case DDERR_NOEMULATION : strcat(ch, "DDERR_NOEMULATION"); break; 
        case DDERR_REGIONTOOSMALL : strcat(ch, "DDERR_REGIONTOOSMALL"); break; 
        case DDERR_CLIPPERISUSINGHWND : strcat(ch, "DDERR_CLIPPERISUSINGHWND"); break; 
        case DDERR_NOCLIPPERATTACHED : strcat(ch, "DDERR_NOCLIPPERATTACHED"); break; 
        case DDERR_NOHWND : strcat(ch, "DDERR_NOHWND"); break; 
        case DDERR_HWNDSUBCLASSED : strcat(ch, "DDERR_HWNDSUBCLASSED"); break; 
        case DDERR_HWNDALREADYSET : strcat(ch, "DDERR_HWNDALREADYSET"); break; 
        case DDERR_NOPALETTEATTACHED : strcat(ch, "DDERR_NOPALETTEATTACHED"); break; 
        case DDERR_NOPALETTEHW : strcat(ch, "DDERR_NOPALETTEHW"); break; 
        case DDERR_BLTFASTCANTCLIP : strcat(ch, "DDERR_BLTFASTCANTCLIP"); break; 
        case DDERR_NOBLTHW : strcat(ch, "DDERR_NOBLTHW"); break; 
        case DDERR_NODDROPSHW : strcat(ch, "DDERR_NODDROPSHW"); break; 
        case DDERR_OVERLAYNOTVISIBLE : strcat(ch, "DDERR_OVERLAYNOTVISIBLE"); break; 
        case DDERR_NOOVERLAYDEST : strcat(ch, "DDERR_NOOVERLAYDEST"); break; 
        case DDERR_INVALIDPOSITION : strcat(ch, "DDERR_INVALIDPOSITION"); break; 
        case DDERR_NOTAOVERLAYSURFACE : strcat(ch, "DDERR_NOTAOVERLAYSURFACE"); break; 
        case DDERR_EXCLUSIVEMODEALREADYSET : strcat(ch, "DDERR_EXCLUSIVEMODEALREADYSET"); break; 
        case DDERR_NOTFLIPPABLE : strcat(ch, "DDERR_NOTFLIPPABLE"); break; 
		case DDERR_CANTDUPLICATE : strcat(ch, "DDERR_CANTDUPLICATE"); break; 
        case DDERR_NOTLOCKED : strcat(ch, "DDERR_NOTLOCKED"); break; 
        case DDERR_CANTCREATEDC : strcat(ch, "DDERR_CANTCREATEDC"); break; 
        case DDERR_NODC : strcat(ch, "DDERR_NODC"); break; 
        case DDERR_WRONGMODE : strcat(ch, "DDERR_WRONGMODE"); break; 
        case DDERR_IMPLICITLYCREATED : strcat(ch, "DDERR_IMPLICITLYCREATED"); break; 
        case DDERR_NOTPALETTIZED : strcat(ch, "DDERR_NOTPALETTIZED"); break; 
        case DDERR_UNSUPPORTEDMODE : strcat(ch, "DDERR_UNSUPPORTEDMODE"); break; 
        case DDERR_NOMIPMAPHW : strcat(ch, "DDERR_NOMIPMAPHW"); break;
		case DDERR_INVALIDSURFACETYPE : strcat(ch, "DDERR_INVALIDSURFACETYPE"); break; 
		case DDERR_DCALREADYCREATED : strcat(ch, "DDERR_DCALREADYCREATED"); break; 
		case DDERR_CANTPAGELOCK : strcat(ch, "DDERR_CANTPAGELOCK"); break; 
        case DDERR_CANTPAGEUNLOCK : strcat(ch, "DDERR_CANTPAGEUNLOCK"); break; 
        case DDERR_NOTPAGELOCKED : strcat(ch, "DDERR_NOTPAGELOCKED"); break; 
        case DDERR_NOTINITIALIZED : strcat(ch, "DDERR_NOTINITIALIZED"); break;

		default: strcat(ch, "Erreur Directdraw inconnu");
		}
	}
	
	MessageBox( NULL, ch, "Erreur", MB_OK | MB_ICONEXCLAMATION );

	delete[] ch;

	return error;
}



void Space::ResizeMainWnd()
{
	RECT rectwnd;
	
	SetRect(&rectwnd, 0, 0, WidthWin - 1, HeightWin - 1);
	AdjustWindowRectEx(&rectwnd, GetWindowLong(hMainWnd, GWL_STYLE), GetMenu(hMainWnd) != NULL, GetWindowLong(hMainWnd, GWL_EXSTYLE));
	SetWindowPos(hMainWnd, NULL, 0, 0, rectwnd.right - rectwnd.left + 1, rectwnd.bottom - rectwnd.top + 1, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}


HMENU Space::MakeMenuConfig()
{
	UINT i;
	char string[100];
	HMENU MainMenuConfig;
	HMENU DisplayMode;
	HMENU FullScreen;
	HMENU Resolution;
	HMENU SpeedMenu;
	HMENU Effects;
	HMENU RandomEffects;
	HMENU AddEffects;
	HMENU RemoveEffects;
	HMENU AllEffects;
	HMENU ClearEffects;
	HMENU RenderMenu;
	/**/HMENU PresetMenu;
	HMENU LoadConfig;
	HMENU SaveConfig;
	HMENU TransMenu;
	HMENU TransShift;
	HMENU MirrorMenu;
	HMENU AllTrans;
	HMENU Background;
	HMENU Windowed;
	HMENU Docked;
	/**/
	HMENU Config;
	HMENU Customize;
	HMENU Misc;
	HMENU RotoBlitMenu;
	HMENU RotationBlit;
	HMENU ZoomBlit;
	/**/
	
	MainMenuConfig = CreatePopupMenu();
	/**/
	Config = CreatePopupMenu();
	Customize = CreatePopupMenu();
	Misc = CreatePopupMenu();
	RotoBlitMenu = CreatePopupMenu();
	RotationBlit = CreatePopupMenu();
	ZoomBlit = CreatePopupMenu();
	/**/
	DisplayMode = CreatePopupMenu();
	FullScreen = CreatePopupMenu();
	Resolution = CreatePopupMenu();
	SpeedMenu = CreatePopupMenu();
	Effects = CreatePopupMenu();
	RandomEffects = CreatePopupMenu();
	AddEffects = CreatePopupMenu();
	RemoveEffects = CreatePopupMenu();
	AllEffects = CreatePopupMenu();
	ClearEffects = CreatePopupMenu();
	TransMenu = CreatePopupMenu();
	TransShift = CreatePopupMenu();
	MirrorMenu = CreatePopupMenu();
	AllTrans = CreatePopupMenu();
	RenderMenu = CreatePopupMenu();
	Background = CreatePopupMenu();
	Windowed = CreatePopupMenu();
	Docked = CreatePopupMenu();
	/**/PresetMenu = CreatePopupMenu();
	LoadConfig = CreatePopupMenu();
	SaveConfig = CreatePopupMenu();
	
	//Création des sous-menu pricipaux
	/**/
	UINT Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(MainMenuConfig, 0, Option, (UINT)Config, "Config");
	InsertMenu(MainMenuConfig, 1, MF_SEPARATOR, NULL, NULL);
	InsertMenu(MainMenuConfig, 2, Option, (UINT)Customize, "Custom");
	InsertMenu(MainMenuConfig, 3, MF_SEPARATOR, NULL, NULL);
	InsertMenu(MainMenuConfig, 4, Option, (UINT)PresetMenu, "Presets");
	InsertMenu(MainMenuConfig, 5, MF_SEPARATOR, NULL, NULL);
	InsertMenu(MainMenuConfig, 6, MF_BYPOSITION | MF_STRING, CLOSEPLUGIN, "Close Plugin");

	InsertMenu(Config, 0, Option, (UINT)DisplayMode, "Display Mode");
	InsertMenu(Config, 1, MF_SEPARATOR, NULL, NULL);
	if( (ViewMode != FULLSCREEN)&&(ViewMode != BACKGROUND) )  {
		InsertMenu(Config, 2, Option, (UINT)Resolution, "Resolution");
		InsertMenu(Config, 3, MF_SEPARATOR, NULL, NULL);
	}
	InsertMenu(Config, 4, Option, (UINT)SpeedMenu, "Speed");

	InsertMenu(Customize, 1, Option, (UINT)Effects, "Effects");
	InsertMenu(Customize, 2, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Customize, 3, Option, (UINT)RenderMenu, "Render");
	InsertMenu(Customize, 4, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Customize, 5, Option, (UINT)TransMenu, "Trans");
	InsertMenu(Customize, 6, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Customize, 7, Option, (UINT)Misc, "Misc");
	
	Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(Misc, 0, Option, (UINT)MirrorMenu, "Mirror");
	InsertMenu(Misc, 1, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Misc, 2, Option, (UINT)RotoBlitMenu, "Blit");
	InsertMenu(Misc, 3, MF_SEPARATOR, NULL, NULL);

	if(BeatEffect) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Misc, 4, Option, BEATEFFECT, "Effects Beat React");
	if(BeatTrans) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Misc, 5, Option, BEATTRANS, "Trans Beat React");

	//menu effets 
	if(Flashis) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Effects, 0, Option, FLASHIS, "Prim color");
	//if(OnBassChange) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	//else Option = MF_BYPOSITION | MF_STRING;
	//InsertMenu(Effects, 2, Option, ONBASSCHANGE, "Beat Reaction");
	InsertMenu(Effects, 3, MF_SEPARATOR, NULL, NULL);

	Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	if( (RandomWaveForm)||(RandomCercleWave)||(RandomCercleBass)||(RandomConfettis)||(RandomMathShape)||(RandomRotaLine)||(RandomLittleWave)||(RandomLittleMath)||(RandomGeoGraph) )
		InsertMenu(Effects, 4, MF_BYPOSITION | MF_POPUP | MF_STRING | MF_CHECKED, (UINT)RandomEffects, "Randomly");
	else InsertMenu(Effects, 4, Option, (UINT)RandomEffects, "Randomly");
	InsertMenu(Effects, 5, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Effects, 6, Option, (UINT)AddEffects, "Add");
	InsertMenu(Effects, 7, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Effects, 8, Option, (UINT)RemoveEffects, "Remove");
	InsertMenu(Effects, 9, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Effects, 10, Option, (UINT)AllEffects, "All");
	InsertMenu(Effects, 11, MF_SEPARATOR, NULL, NULL);
	InsertMenu(Effects, 12, Option, (UINT)ClearEffects, "Clear");


	//création des différents modes de vue
	if(ViewMode == FULLSCREEN) Option = MF_BYPOSITION | MF_POPUP | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(DisplayMode, 0, Option, (UINT)FullScreen, "Fullscreen");
	InsertMenu(DisplayMode, 1, MF_SEPARATOR, NULL, NULL);

	if(ViewMode == BACKGROUND) Option = MF_BYPOSITION | MF_POPUP | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(DisplayMode, 2, Option, (UINT)Background, "Background");
	InsertMenu(DisplayMode, 3, MF_SEPARATOR, NULL, NULL);

	if(ViewMode == WINDOWED) Option = MF_BYPOSITION | MF_POPUP | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(DisplayMode, 4, Option, (UINT)Windowed, "Windowed");
	InsertMenu(DisplayMode, 5, MF_SEPARATOR, NULL, NULL);

	if(ViewMode == DOCKED) Option = MF_BYPOSITION | MF_POPUP | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(DisplayMode, 6, Option, (UINT)Docked, "Docked");
	InsertMenu(DisplayMode, 7, MF_SEPARATOR, NULL, NULL);

	if(Mode555 == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(DisplayMode, 8, Option, MODE555, "15 bpp (555)");
	InsertMenu(DisplayMode, 9, MF_SEPARATOR, NULL, NULL);

	if(ShowFPS == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(DisplayMode, 10, Option, FPS, "View FPS (F4)");


	//création des différents fullscreen
	i = 0;
	while(ModeSupport[i][0])
	{
		wsprintf(string, "%dX%d", ModeSupport[i][0], ModeSupport[i][1]);
		if( (WidthMode == ModeSupport[i][0])&&(HeightMode == ModeSupport[i][1]) ) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(FullScreen, i, Option, FS + i, string);
		i++;
	}

	//création des différents background
	if(WidthBack == 160) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 0, Option, BG160, "160 X 100");

	if(HeightBack == 200) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 1, Option, BG200, "320 X 200");

	if(HeightBack == 240) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 2, Option, BG240, "320 X 240");

	if(WidthBack == 400) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 3, Option, BG400, "400 X 300");

	if(WidthBack == 512) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 4, Option, BG512, "512 X 384");

	if(WidthBack == 640) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 5, Option, BG640, "640 X 480");

	if(WidthBack == 800) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 6, Option, BG800, "800 X 600");

	if(WidthBack == 1024) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Background, 7, Option, BG1024, "1024 X 768");


	//création des différents Docks
	if(DockMode == LITTLEDOCK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Docked, 0, Option, LITTLEDOCK, "Little Dock");

	if(DockMode == BOTTOMDOCK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Docked, 1, Option, BOTTOMDOCK, "Bottom Dock");

	if(DockMode == TOPDOCK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Docked, 2, Option, TOPDOCK, "Top Dock");

	if(DockMode == RIGHTDOCK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Docked, 3, Option, RIGHTDOCK, "RightDock");

	if(DockMode == LEFTDOCK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Docked, 4, Option, LEFTDOCK, "LeftDock");


	//création des différents windows
	if( (WidthWin == 320)&&(HeightWin == 200) ) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Windowed, 1, Option, WIN320, "320 X 200");

	if( (WidthWin == 400)&&(HeightWin == 300) ) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Windowed, 2, Option, WIN400, "400 X 300");

	if( (WidthWin == 512)&&(HeightWin == 384) ) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Windowed, 3, Option, WIN512, "512 X 384");

	if( (WidthWin == 640)&&(HeightWin == 480) ) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Windowed, 4, Option, WIN640, "640 X 480");

	wsprintf(string, "defined (%d X %d)", WidthWin, HeightWin);
	if( ( (WidthWin != 320)||(HeightWin != 200) )&&( (WidthWin != 400)||(HeightWin != 300) )&&( (WidthWin != 512)||(HeightWin != 384) )&&( (WidthWin != 640)||(HeightWin != 480) )/*&&( (WidthWin != 800)||(HeightWin != 600) )&&( (WidthWin != 1024)||(HeightWin != 768) )*/ )
		Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(Windowed, 7, Option, WINDEFINED, string);


	//création des différents modes de résolutions
	if( (ViewMode != FULLSCREEN)&&(ViewMode != BACKGROUND) ) {
		if(ResMode == AUTORES) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(Resolution, 5, Option, AUTORES, "Auto");

		if(ResMode == AUTODEMIRES) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(Resolution, 1, Option, AUTODEMIRES, "Auto / 2 (Faster)");

		if(ResMode == DEFINEDRES) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(Resolution, 2, Option, DEFINEDRES, "Defined");
	}

	
	//création des différents rapiditées
	if(Speed == SPEED20) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(SpeedMenu, 0, Option, SPEED20, "20 FPS");

	if(Speed == SPEED30) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(SpeedMenu, 1, Option, SPEED30, "30 FPS");

	if(Speed == SPEED45) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(SpeedMenu, 2, Option, SPEED45, "45 FPS");

	if(Speed == SPEED60) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(SpeedMenu, 3, Option, SPEED60, "60 FPS");

	if(Speed == SPEEDMAX) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(SpeedMenu, 4, Option, SPEEDMAX, "MAX");

	// création des effets
	Option = MF_BYPOSITION | MF_STRING;
	
	wsprintf(string, "Waveform (%d)", WaveFormEff::nombre);
	if(RandomWaveForm) InsertMenu(RandomEffects, 0, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMWAVEFORM, string);
	else InsertMenu(RandomEffects, 0, MF_BYPOSITION | MF_STRING, RANDOMWAVEFORM, string);
	InsertMenu(AddEffects, 0, Option, ADDWAVEFORM, string);
	InsertMenu(RemoveEffects, 0, Option, REMOVEWAVEFORM, string);
	InsertMenu(AllEffects, 0, Option, ALLWAVEFORM, string);
	InsertMenu(ClearEffects, 0, Option, CLEARWAVEFORM, string);
	
	wsprintf(string, "CercleWave (%d)", CercleWaveEff::nombre);
	if(RandomCercleWave) InsertMenu(RandomEffects, 1, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMCERCLEWAVE, string);
	else InsertMenu(RandomEffects, 1, MF_BYPOSITION | MF_STRING, RANDOMCERCLEWAVE, string);
	InsertMenu(AddEffects, 1, Option, ADDCERCLEWAVE, string);
	InsertMenu(RemoveEffects, 1, Option, REMOVECERCLEWAVE, string);
	InsertMenu(AllEffects, 1, Option, ALLCERCLEWAVE, string);
	InsertMenu(ClearEffects, 1, Option, CLEARCERCLEWAVE, string);
		
	wsprintf(string, "Concentric Bass (%d)", CercleBassEff::nombre);
	if(RandomCercleBass) InsertMenu(RandomEffects, 2, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMCERCLEBASS, string);
	else InsertMenu(RandomEffects, 2, MF_BYPOSITION | MF_STRING, RANDOMCERCLEBASS, string);
	InsertMenu(AddEffects, 2, Option, ADDCERCLEBASS, string);
	InsertMenu(RemoveEffects, 2, Option, REMOVECERCLEBASS, string);
	InsertMenu(AllEffects, 2, Option, ALLCERCLEBASS, string);
	InsertMenu(ClearEffects, 2, Option, CLEARCERCLEBASS, string);
	
	wsprintf(string, "Confettis (%d)", ConfettisEff::nombre);
	if(RandomConfettis) InsertMenu(RandomEffects, 3, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMCONFETTIS, string);
	else InsertMenu(RandomEffects, 3, MF_BYPOSITION | MF_STRING, RANDOMCONFETTIS, string);
	InsertMenu(AddEffects, 3, Option, ADDCONFETTIS, string);
	InsertMenu(RemoveEffects, 3, Option, REMOVECONFETTIS, string);
	InsertMenu(AllEffects, 3, Option, ALLCONFETTIS, string);
	InsertMenu(ClearEffects, 3, Option, CLEARCONFETTIS, string);

	wsprintf(string, "MathShape (%d)", MathShapeEff::nombre);
	if(RandomMathShape) InsertMenu(RandomEffects, 4, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMMATHSHAPE, string);
	else InsertMenu(RandomEffects, 4, MF_BYPOSITION | MF_STRING, RANDOMMATHSHAPE, string);
	InsertMenu(AddEffects, 4, Option, ADDMATHSHAPE, string);
	InsertMenu(RemoveEffects, 4, Option, REMOVEMATHSHAPE, string);
	InsertMenu(AllEffects, 4, Option, ALLMATHSHAPE, string);
	InsertMenu(ClearEffects, 4, Option, CLEARMATHSHAPE, string);

	wsprintf(string, "RotaLine (%d)", RotaLineEff::nombre);
	if(RandomRotaLine) InsertMenu(RandomEffects, 5, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMROTALINE, string);
	else InsertMenu(RandomEffects, 5, MF_BYPOSITION | MF_STRING, RANDOMROTALINE, string);
	InsertMenu(AddEffects, 5, Option, ADDROTALINE, string);
	InsertMenu(RemoveEffects, 5, Option, REMOVEROTALINE, string);
	InsertMenu(AllEffects, 5, Option, ALLROTALINE, string);
	InsertMenu(ClearEffects, 5, Option, CLEARROTALINE, string);

	wsprintf(string, "LittleWave (%d)", LittleWaveEff::nombre);
	if(RandomLittleWave) InsertMenu(RandomEffects, 6, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMLITTLEWAVE, string);
	else InsertMenu(RandomEffects, 6, MF_BYPOSITION | MF_STRING, RANDOMLITTLEWAVE, string);
	InsertMenu(AddEffects, 6, Option, ADDLITTLEWAVE, string);
	InsertMenu(RemoveEffects, 6, Option, REMOVELITTLEWAVE, string);
	InsertMenu(AllEffects, 6, Option, ALLLITTLEWAVE, string);
	InsertMenu(ClearEffects, 6, Option, CLEARLITTLEWAVE, string);


	wsprintf(string, "LittleMath (%d)", LittleMathEff::nombre);
	if(RandomLittleMath) InsertMenu(RandomEffects, 7, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMLITTLEMATH, string);
	else InsertMenu(RandomEffects, 7, MF_BYPOSITION | MF_STRING, RANDOMLITTLEMATH, string);
	InsertMenu(AddEffects, 7, Option, ADDLITTLEMATH, string);
	InsertMenu(RemoveEffects, 7, Option, REMOVELITTLEMATH, string);
	InsertMenu(AllEffects, 7, Option, ALLLITTLEMATH, string);
	InsertMenu(ClearEffects, 7, Option, CLEARLITTLEMATH, string);


	wsprintf(string, "GeoGraph (%d)", GeoGraphEff::nombre);
	if(RandomGeoGraph) InsertMenu(RandomEffects, 8, MF_BYPOSITION | MF_STRING | MF_CHECKED, RANDOMGEOGRAPH, string);
	else InsertMenu(RandomEffects, 8, MF_BYPOSITION | MF_STRING, RANDOMGEOGRAPH, string);
	InsertMenu(AddEffects, 8, Option, ADDGEOGRAPH, string);
	InsertMenu(RemoveEffects, 8, Option, REMOVEGEOGRAPH, string);
	InsertMenu(AllEffects, 8, Option, ALLGEOGRAPH, string);
	InsertMenu(ClearEffects, 8, Option, CLEARGEOGRAPH, string);


	//transformation
	if(Trans.enabled == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(TransMenu, 0, Option, MATHENABLED, "Enabled");
	InsertMenu(TransMenu, 1, MF_SEPARATOR, NULL, NULL);

	//if(!Trans.random) {
	if(TransBefore == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(TransMenu, 2, Option, MATHTRANSBEFORE, "After Effects");
	InsertMenu(TransMenu, 3, MF_SEPARATOR, NULL, NULL);
	//}
	
	if(Trans.random) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(TransMenu, 4, Option, MATHRANDOM, "Random Mode");
	InsertMenu(TransMenu, 5, MF_SEPARATOR, NULL, NULL);

	if(Trans.noise) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(TransMenu, 6, Option, MATHNOISE, "Noise");
	InsertMenu(TransMenu, 7, MF_SEPARATOR, NULL, NULL);

	if(Trans.random) {
		if(Trans.slide) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransMenu, 8, Option, MATHSLIDE, "Slide");
		InsertMenu(TransMenu, 9, MF_SEPARATOR, NULL, NULL);
		InsertMenu(TransMenu, 10, MF_BYPOSITION | MF_STRING, MATHNEXT, "Next");
	}
	else {
//		if(Trans.type != NOMOVE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED | MF_POPUP;
		/*else*/ Option = MF_BYPOSITION | MF_STRING | MF_POPUP;
		InsertMenu(TransMenu, 8, Option, (UINT)AllTrans, "Trans");

		if(Trans.type == NOMOVE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 0, Option, MATHNOMOVE, "NoMove");

		if(Trans.type == ZOOMIN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 1, Option, MATHZOOMIN, "Zoom In");

		if(Trans.type == ZOOMOUT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 2, Option, MATHZOOMOUT, "Zoom Out");

		if(Trans.type == ROTATION) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 3, Option, MATHROTATION, "Rotation");

		if(Trans.type == ROTOZOOMIN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 4, Option, MATHROTOZOOMIN, "Rotozoom In");

		if(Trans.type == ROTOZOOMOUT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 5, Option, MATHROTOZOOMOUT, "Rotozoom Out");

		if(Trans.type == PETALES) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 6, Option, MATHPETALES, "Crunchy");

		if(Trans.type == ZARB) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 7, Option, MATHZARB, "Spider");

		if(Trans.type == TUNNEL) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 8, Option, MATHTUNNEL, "Tunnel");
		
		if(Trans.type == CYCLONE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 9, Option, MATHCYCLONE, "Cyclone");
		
		/*if(Trans.type == ROTOBRICK) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 8, Option, MATHROTOBRICK, "RotoBrick");
		*/
		if(Trans.type == HORIZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 10, Option, MATHHORIZOOM, "HoriZoom");
		
		if(Trans.type == VERTIZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 11, Option, MATHVERTIZOOM, "VertiZoom");
		
		if(Trans.type == RAYSUN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 12, Option, MATHRAYSUN, "SunRay");
		
/*		if(Trans.type == WATER) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 12, Option, MATHWATER, "Water");
*/		
		if(Trans.type == BIROTOZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 13, Option, MATHBIROTOZOOM, "Bi-RotoZoom");
		
		if(Trans.type == BIZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 14, Option, MATHBIZOOM, "Bi-Zoom");
		
		if(Trans.type == ROTOSUN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 15, Option, MATHROTOSUN, "WaveSun");
		
		if(Trans.type == BOX) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 16, Option, MATHBOX, "Box");
		
		if(Trans.type == ROTOTUNNEL) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 17, Option, MATHROTOTUNNEL, "RotoTunnel");
		
		if(Trans.type == WATERZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 18, Option, MATHWATERZOOM, "WaterZoom");
		
		if(Trans.type == GALAXY) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 19, Option, MATHGALAXY, "Galaxy");
		
		if(Trans.type == VERTIPERSP) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 20, Option, MATHVERTIPERSP, "VertiPersp");

		if(Trans.type == HORIPERSP) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 21, Option, MATHHORIPERSP, "HoriPersp");
		
		if(Trans.type == CYCLOFLOWER) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 22, Option, MATHCYCLOFLOWER, "CyCloFlower");

		if(Trans.type == ILLUSION) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 23, Option, MATHILLUSION, "Illusion");

		if(Trans.type == SWIRLOUT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 24, Option, MATHSWIRLOUT, "Swirl Out");

		if(Trans.type == IMPLOSE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 25, Option, MATHIMPLOSE, "Implose");

		if(Trans.type == SHADOWIN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(AllTrans, 26, Option, MATHSHADOWIN, "Shadow In");
	}

		InsertMenu(TransMenu, 11, MF_SEPARATOR, NULL, NULL);
		if(Trans.shift != NOSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED | MF_POPUP;
		else Option = MF_BYPOSITION | MF_STRING | MF_POPUP;
		InsertMenu(TransMenu, 12, Option, (UINT)TransShift, "Moving");

		if(Trans.shift == NOSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 1, Option, MATHNOSHIFT, "No");

		if(Trans.shift == TOPLEFTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 2, Option, MATHTOPLEFTSHIFT, "Top Left");

		if(Trans.shift == TOPSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 3, Option, MATHTOPSHIFT, "Top");

		if(Trans.shift == TOPRIGHTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 4, Option, MATHTOPRIGHTSHIFT, "Top Right");

		if(Trans.shift == RIGHTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 5, Option, MATHRIGHTSHIFT, "Right");

		if(Trans.shift == BOTTOMRIGHTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 6, Option, MATHBOTTOMRIGHTSHIFT, "Bottom Right");

		if(Trans.shift == BOTTOMSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 7, Option, MATHBOTTOMSHIFT, "Bottom");

		if(Trans.shift == BOTTOMLEFTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 8, Option, MATHBOTTOMLEFTSHIFT, "Bottom Left");

		if(Trans.shift == LEFTSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 9, Option, MATHLEFTSHIFT, "Left");

		if(Trans.shift == BASSSHIFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransShift, 10, Option, MATHBASSSHIFT, "Free");

		/*	// bof ! sert pas à grand chose
		InsertMenu(TransMenu, 13, MF_SEPARATOR, NULL, NULL);
		if(Trans.fast) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(TransMenu, 14, Option, MATHTRANSFAST, "Fast");
		*/

	//création des différents rendus
	if(Fade == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RenderMenu, 0, Option, FADEOUT, "Fadeout");
	InsertMenu(RenderMenu, 1, MF_SEPARATOR, NULL, NULL);

	if( (Render != CLEARSCREEN)&&(Render != NORENDER) ) {
		if(Blend == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
		else Option = MF_BYPOSITION | MF_STRING;
		InsertMenu(RenderMenu, 2, Option, BLEND, "Blend");
		InsertMenu(RenderMenu, 3, MF_SEPARATOR, NULL, NULL);
	}

	if(Render == CLEARSCREEN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RenderMenu, 4, Option, CLEARSCREEN, "Clear Buffer");
	
	if(Render == QBLUR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RenderMenu, 5, Option, QBLUR, "Fast BLur");

	if(Render == NBLUR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RenderMenu, 6, Option, NBLUR, "Normal Blur");

	if(Render == SBLUR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RenderMenu, 7, Option, SBLUR, "Slow Blur");

	if(RandomMirror) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(MirrorMenu, 0, Option, RANDOMMIRROR, "Random");
	InsertMenu(MirrorMenu, 1, MF_SEPARATOR, NULL, NULL);

	if(Mirror == NOMIRROR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(MirrorMenu, 2, Option, NOMIRROR, "No");

	if(Mirror == VERMIRROR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(MirrorMenu, 3, Option, VERMIRROR, "Vertical");

	if(Mirror == HORMIRROR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(MirrorMenu, 4, Option, HORMIRROR, "Horizontal");

	if(Mirror == DOUMIRROR) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(MirrorMenu, 5, Option, DOUMIRROR, "Double");

	if(LoadRandom == TRUE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(PresetMenu, 0, Option, LOADRANDOM, "Load Randomly");
	InsertMenu(PresetMenu, 1, MF_SEPARATOR, NULL, NULL);
	Option = MF_BYPOSITION | MF_POPUP | MF_STRING;
	InsertMenu(PresetMenu, 2, Option, (UINT)LoadConfig, "Load");
	InsertMenu(PresetMenu, 3, MF_SEPARATOR, NULL, NULL);
	InsertMenu(PresetMenu, 4, Option, (UINT)SaveConfig, "Save As");

	//Chargement Config et Sauvegarde Config
	{
		UINT pos = 0;
		char** presets = GetPresets();
		
		InsertMenu(SaveConfig, 0, MF_BYPOSITION | MF_STRING, SAVECONF, "new");
		InsertMenu(LoadConfig, 0, MF_BYPOSITION | MF_STRING, LOADCONF, "new");

		while( (presets[pos++])&&(pos < 0x1000) )  {
			if(strcmp(presets[pos - 1], Version)) InsertMenu(LoadConfig, pos, MF_BYPOSITION | MF_STRING, LOADCONF + pos, presets[pos - 1]);
			if( (strcmp(presets[pos - 1], "default"))&&(strcmp(presets[pos - 1], Version)) ) InsertMenu(SaveConfig, pos, MF_BYPOSITION | MF_STRING, SAVECONF + pos, presets[pos - 1]);
			free(presets[pos - 1]);		
		}
	
		free(presets);
	}

	if(blitrotozoom) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 0, Option, BLITENABLED, "Enabled");
	InsertMenu(RotoBlitMenu, 1, MF_SEPARATOR, NULL, NULL);
	
	if(blitzoom == BLITNOZOOM) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 2, Option, BLITNOZOOM, "No Zoom");
	if(blitzoom == BLITZOOMIN) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 3, Option, BLITZOOMIN, "Zoom In");
	if(blitzoom == BLITZOOMOUT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 4, Option, BLITZOOMOUT, "Zoom Out");
	if(blitzoom == BLITZOOMBEAT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 5, Option, BLITZOOMBEAT, "Zoom Beat");
	InsertMenu(RotoBlitMenu, 6, MF_SEPARATOR, NULL, NULL);
	
	if(blitrotation == BLITNOROTATE) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 7, Option, BLITNOROTATE, "No Rotation");
	if(blitrotation == BLITROTATELEFT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 8, Option, BLITROTATELEFT, "Rot. Left");
	if(blitrotation == BLITROTATERIGHT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 9, Option, BLITROTATERIGHT, "Rot. Right");
	if(blitrotation == BLITROTATEBEAT) Option = MF_BYPOSITION | MF_STRING | MF_CHECKED;
	else Option = MF_BYPOSITION | MF_STRING;
	InsertMenu(RotoBlitMenu, 10, Option, BLITROTATEBEAT, "Rot. On Beat");



	return MainMenuConfig;
}


int Space::RunMenuConfigSel(UINT Choix)
{

UINT Result = 0;

	switch(Choix) {

//mode vidéo
	case BG160 : if( (ViewMode != BACKGROUND)||(HeightBack != 100) ) { WidthBack = 160; HeightBack = 100; Result = ChangeViewMode(BACKGROUND); } break;
	case BG200 : if( (ViewMode != BACKGROUND)||(HeightBack != 200) ) { WidthBack = 320; HeightBack = 200; Result = ChangeViewMode(BACKGROUND); } break;
	case BG240 : if( (ViewMode != BACKGROUND)||(HeightBack != 240) ) { WidthBack = 320; HeightBack = 240; Result = ChangeViewMode(BACKGROUND); } break;
	case BG400 : if( (ViewMode != BACKGROUND)||(HeightBack != 300) ) { WidthBack = 400; HeightBack = 300; Result = ChangeViewMode(BACKGROUND); } break;
	case BG512 : if( (ViewMode != BACKGROUND)||(HeightBack != 384) ) { WidthBack = 512; HeightBack = 384; Result = ChangeViewMode(BACKGROUND); } break;
	case BG640 : if( (ViewMode != BACKGROUND)||(HeightBack != 480) ) { WidthBack = 640; HeightBack = 480; Result = ChangeViewMode(BACKGROUND); } break;
	case BG800 : if( (ViewMode != BACKGROUND)||(HeightBack != 600) ) { WidthBack = 800; HeightBack = 600; Result = ChangeViewMode(BACKGROUND); } break;
	case BG1024 : if( (ViewMode != BACKGROUND)||(HeightBack != 768) ) { WidthBack = 1024; HeightBack = 768; Result = ChangeViewMode(BACKGROUND); } break;

	case LITTLEDOCK : case TOPDOCK : case BOTTOMDOCK : case LEFTDOCK : case RIGHTDOCK : 
		if( (ViewMode != DOCKED)||(DockMode != Choix) ) { DockMode = Choix; Result = ChangeViewMode(DOCKED); } break;

	case WIN320 : if( (ViewMode != WINDOWED)||(HeightWin != 200)||(WidthWin != 320) ) { WidthWin = 320; HeightWin = 200; Result = ChangeViewMode(WINDOWED); } break;
	case WIN400 : if( (ViewMode != WINDOWED)||(HeightWin != 300)||(WidthWin != 400) ) { WidthWin = 400; HeightWin = 300; Result = ChangeViewMode(WINDOWED); } break;
	case WIN512 : if( (ViewMode != WINDOWED)||(HeightWin != 384)||(WidthWin != 512) ) { WidthWin = 512; HeightWin = 384; Result = ChangeViewMode(WINDOWED); } break;
	case WIN640 : if( (ViewMode != WINDOWED)||(HeightWin != 480)||(WidthWin != 640) ) { WidthWin = 640; HeightWin = 480; Result = ChangeViewMode(WINDOWED); } break;
	case WINDEFINED : 
		if(ViewMode != WINDOWED) {
			//SetWindowSize();
			Result = ChangeViewMode(WINDOWED);
		}
		break;

//resolutions
	case AUTORES :
		ResMode = AUTORES;
		Result = CheckRes(ViewMode);
		break;
	case AUTODEMIRES :
		ResMode = AUTODEMIRES;
		if(ViewMode == WINDOWED) {
			WidthWin = ((WidthWin>>1)<<1);
			HeightWin = ((HeightWin>>1)<<1);
			SetSizeRectWnd();
		}
		Result = CheckRes(ViewMode);
		break; 
	case DEFINEDRES :
		ChildWndDisplayed = 1;
		DialogBox(module.hDllInstance, MAKEINTRESOURCE(IDD_DEFINEDRESDIALOG), hMainWnd,(DLGPROC)DefinedResDialogProc);
		Result = CheckRes(ViewMode); 
		break;  
	
//vitesse
	case SPEED20 : case SPEED30 : case SPEED45 : case SPEED60 : case SPEEDMAX : 
		Speed = Choix; break;
	
//effets
	case FLASHIS: Flashis = (Flashis)?FALSE:TRUE; break;
	//case ONBASSCHANGE: OnBassChange = (OnBassChange)?FALSE:TRUE; break;
	case BEATEFFECT: BeatEffect = (BeatEffect)?FALSE:TRUE; break;
	case BEATTRANS: BeatTrans = (BeatTrans)?FALSE:TRUE; break;

	case RANDOMWAVEFORM : RandomWaveForm = (RandomWaveForm)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMCERCLEWAVE : RandomCercleWave = (RandomCercleWave)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMCERCLEBASS : RandomCercleBass = (RandomCercleBass)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMCONFETTIS : RandomConfettis = (RandomConfettis)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMMATHSHAPE : RandomMathShape = (RandomMathShape)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMROTALINE : RandomRotaLine = (RandomRotaLine)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMLITTLEWAVE : RandomLittleWave = (RandomLittleWave)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMLITTLEMATH : RandomLittleMath = (RandomLittleMath)?FALSE:TRUE; ChangeEffects(); break;
	case RANDOMGEOGRAPH : RandomGeoGraph = (RandomGeoGraph)?FALSE:TRUE; ChangeEffects(); break;

	case ADDWAVEFORM : 	if(WaveFormEff::nombre < MAX_WAVEFORM_EFFECTS) { int indice = WaveFormEff::nombre; WaveForm[indice] = new WaveFormEff(WaveForm); } break;
	case REMOVEWAVEFORM : if(WaveFormEff::nombre > 0) { int indice = WaveFormEff::nombre - 1;	delete WaveForm[indice]; } break;
	case ALLWAVEFORM : { int temp = WaveFormEff::nombre; for(int i = temp; i < MAX_WAVEFORM_EFFECTS; i++) WaveForm[i] = new WaveFormEff(WaveForm); } break;
	case CLEARWAVEFORM : { int temp = WaveFormEff::nombre; for(int i = 0; i < temp; i++)  delete WaveForm[i]; } break;

	case ADDCERCLEWAVE : 	if(CercleWaveEff::nombre < MAX_CERCLEWAVE_EFFECTS) { int indice = CercleWaveEff::nombre; CercleWave[indice] = new CercleWaveEff(CercleWave); } break;
	case REMOVECERCLEWAVE : if(CercleWaveEff::nombre > 0) { int indice = CercleWaveEff::nombre - 1;	delete CercleWave[indice]; } break;
	case ALLCERCLEWAVE : { int temp = CercleWaveEff::nombre; for(int i = temp; i < MAX_CERCLEWAVE_EFFECTS; i++) CercleWave[i] = new CercleWaveEff(CercleWave); } break;
	case CLEARCERCLEWAVE : { int temp = CercleWaveEff::nombre; for(int i = 0; i < temp; i++)  delete CercleWave[i]; } break;

	case ADDCERCLEBASS : if(CercleBassEff::nombre < MAX_CERCLEBASS_EFFECTS) { int indice = CercleBassEff::nombre; CercleBass[indice] = new CercleBassEff(); } break;
	case REMOVECERCLEBASS : if(CercleBassEff::nombre > 0) { int indice = CercleBassEff::nombre - 1;	delete CercleBass[indice]; } 
		if(CercleBassEff::nombre == 0) break;
	case ALLCERCLEBASS : { int temp = CercleBassEff::nombre; for(int i = temp; i < MAX_CERCLEBASS_EFFECTS; i++) CercleBass[i] = new CercleBassEff(); } break;
	case CLEARCERCLEBASS : { int temp = CercleBassEff::nombre; for(int i = 0; i < temp; i++)  delete CercleBass[i]; /*NeedBass = 0;*/ } break;

	case ADDCONFETTIS : if(ConfettisEff::nombre < MAX_CONFETTIS_EFFECTS) { int indice = ConfettisEff::nombre; Confettis[indice] = new ConfettisEff(); } break;
	case REMOVECONFETTIS : if(ConfettisEff::nombre > 0) { int indice = ConfettisEff::nombre - 1;	delete Confettis[indice]; } break;
	case ALLCONFETTIS : { int temp = ConfettisEff::nombre; for(int i = temp; i < MAX_CONFETTIS_EFFECTS; i++) Confettis[i] = new ConfettisEff(); } break;
	case CLEARCONFETTIS : { int temp = ConfettisEff::nombre; for(int i = 0; i < temp; i++)  delete Confettis[i]; } break;

	case ADDMATHSHAPE : if(MathShapeEff::nombre < MAX_MATHSHAPE_EFFECTS) { int indice = MathShapeEff::nombre; MathShape[indice] = new MathShapeEff(); } break;
	case REMOVEMATHSHAPE : if(MathShapeEff::nombre > 0) { int indice = MathShapeEff::nombre - 1;	delete MathShape[indice]; } break;
	case ALLMATHSHAPE : { int temp = MathShapeEff::nombre; for(int i = temp; i < MAX_MATHSHAPE_EFFECTS; i++) MathShape[i] = new MathShapeEff(); } break;
	case CLEARMATHSHAPE : { int temp = MathShapeEff::nombre; for(int i = 0; i < temp; i++)  delete MathShape[i]; } break;

	case ADDROTALINE : if(RotaLineEff::nombre < MAX_ROTALINE_EFFECTS) { int indice = RotaLineEff::nombre; RotaLine[indice] = new RotaLineEff(RotaLine); } break;
	case REMOVEROTALINE : if(RotaLineEff::nombre > 0) { int indice = RotaLineEff::nombre - 1;	delete RotaLine[indice]; } break;
	case ALLROTALINE : { int temp = RotaLineEff::nombre; for(int i = temp; i < MAX_ROTALINE_EFFECTS; i++) RotaLine[i] = new RotaLineEff(RotaLine); } break;
	case CLEARROTALINE : { int temp = RotaLineEff::nombre; for(int i = 0; i < temp; i++)  delete RotaLine[i]; } break;

	case ADDLITTLEWAVE : if(LittleWaveEff::nombre < MAX_LITTLEWAVE_EFFECTS) { int indice = LittleWaveEff::nombre; LittleWave[indice] = new LittleWaveEff(); } break;
	case REMOVELITTLEWAVE : if(LittleWaveEff::nombre > 0) { int indice = LittleWaveEff::nombre - 1;	delete LittleWave[indice]; } break;
	case ALLLITTLEWAVE : { int temp = LittleWaveEff::nombre; for(int i = temp; i < MAX_LITTLEWAVE_EFFECTS; i++) LittleWave[i] = new LittleWaveEff(); } break;
	case CLEARLITTLEWAVE : { int temp = LittleWaveEff::nombre; for(int i = 0; i < temp; i++)  delete LittleWave[i]; } break;

	case ADDLITTLEMATH : if(LittleMathEff::nombre < MAX_LITTLEMATH_EFFECTS) { int indice = LittleMathEff::nombre; LittleMath[indice] = new LittleMathEff(); } break;
	case REMOVELITTLEMATH : if(LittleMathEff::nombre > 0) { int indice = LittleMathEff::nombre - 1;	delete LittleMath[indice]; } break;
	case ALLLITTLEMATH : { int temp = LittleMathEff::nombre; for(int i = temp; i < MAX_LITTLEMATH_EFFECTS; i++) LittleMath[i] = new LittleMathEff(); } break;
	case CLEARLITTLEMATH : { int temp = LittleMathEff::nombre; for(int i = 0; i < temp; i++)  delete LittleMath[i]; } break;

	case ADDGEOGRAPH : if(GeoGraphEff::nombre < MAX_GEOGRAPH_EFFECTS) { int indice = GeoGraphEff::nombre; GeoGraph[indice] = new GeoGraphEff(GeoGraph); } break;
	case REMOVEGEOGRAPH : if(GeoGraphEff::nombre > 0) { int indice = GeoGraphEff::nombre - 1;	delete GeoGraph[indice]; } break;
	case ALLGEOGRAPH : { int temp = GeoGraphEff::nombre; for(int i = temp; i < MAX_GEOGRAPH_EFFECTS; i++) GeoGraph[i] = new GeoGraphEff(GeoGraph); } break;
	case CLEARGEOGRAPH : { int temp = GeoGraphEff::nombre; for(int i = 0; i < temp; i++)  delete GeoGraph[i]; } break;

//rendus
	case CLEARSCREEN :	Render = (Render == CLEARSCREEN)?NORENDER:CLEARSCREEN; break;
	case FADEOUT :	Fade = (Fade)?FALSE:TRUE; break;
	case BLEND :	Blend = (Blend)?FALSE:TRUE; break;
	case QBLUR :	Render = (Render == QBLUR)?NORENDER:QBLUR; break;
	case NBLUR :	Render = (Render == NBLUR)?NORENDER:NBLUR; break;
	case SBLUR :	Render = (Render == SBLUR)?NORENDER:SBLUR; break;

//transformation
	case MATHENABLED : Trans.enabled = Trans.newtrans = (Trans.enabled)?FALSE:TRUE; break;
	case MATHNORANDOM : Trans.random = 0; Trans.newtrans = TRUE; break;
	case MATHRANDOM : Trans.random = (Trans.random)?FALSE:TRUE; Trans.newtrans = TRUE; break;
	case MATHNOISE : Trans.noise = (Trans.noise)?FALSE:TRUE; Trans.newtrans = TRUE; break;
	case MATHSLIDE : Trans.slide = (Trans.slide)?FALSE:TRUE; Trans.newtrans = TRUE; break;
	case MATHNEXT : Trans.newtrans = TRUE; break;
	case MATHTRANSBEFORE : TransBefore = (TransBefore)?FALSE:TRUE; break; 
	//case MATHTRANSFAST : Trans.fast = (Trans.fast)?FALSE:TRUE; break;
	case MATHROTOZOOMIN : if(Trans.type != ROTOZOOMIN) { Trans.type = ROTOZOOMIN; Trans.newtrans = TRUE; } break;
	case MATHROTOZOOMOUT : if(Trans.type != ROTOZOOMOUT) { Trans.type = ROTOZOOMOUT; Trans.newtrans = TRUE; } break;
	case MATHZOOMIN : if(Trans.type != ZOOMIN) { Trans.type = ZOOMIN; Trans.newtrans = TRUE; } break; 
	case MATHZOOMOUT : if(Trans.type != ZOOMOUT) { Trans.type = ZOOMOUT; Trans.newtrans = TRUE; } break; 
	case MATHROTATION : if(Trans.type != ROTATION) { Trans.type = ROTATION; Trans.newtrans = TRUE; } break; 
	case MATHPETALES : if(Trans.type != PETALES) { Trans.type = PETALES; Trans.newtrans = TRUE; } break; 
	case MATHZARB : if(Trans.type != ZARB) { Trans.type = ZARB; Trans.newtrans = TRUE; } break; 
	case MATHTUNNEL : if(Trans.type != TUNNEL) { Trans.type = TUNNEL; Trans.newtrans = TRUE; } break; 
	case MATHCYCLONE : if(Trans.type != CYCLONE) { Trans.type = CYCLONE; Trans.newtrans = TRUE; } break; 
//	case MATHROTOBRICK : if(Trans.type != ROTOBRICK) { Trans.type = ROTOBRICK; Trans.newtrans = TRUE; } break; 
	case MATHHORIZOOM : if(Trans.type != HORIZOOM) { Trans.type = HORIZOOM; Trans.newtrans = TRUE; } break; 
	case MATHVERTIZOOM : if(Trans.type != VERTIZOOM) { Trans.type = VERTIZOOM; Trans.newtrans = TRUE; } break; 
	case MATHRAYSUN : if(Trans.type != RAYSUN) { Trans.type = RAYSUN; Trans.newtrans = TRUE; } break; 
//	case MATHWATER : if(Trans.type != WATER) { Trans.type = WATER; Trans.newtrans = TRUE; } break; 
	case MATHBIROTOZOOM : if(Trans.type != BIROTOZOOM) { Trans.type = BIROTOZOOM; Trans.newtrans = TRUE; } break; 
	case MATHBIZOOM : if(Trans.type != BIZOOM) { Trans.type = BIZOOM; Trans.newtrans = TRUE; } break; 
	case MATHROTOSUN : if(Trans.type != ROTOSUN) { Trans.type = ROTOSUN; Trans.newtrans = TRUE; } break; 
	case MATHBOX : if(Trans.type != BOX) { Trans.type = BOX; Trans.newtrans = TRUE; } break; 
	case MATHROTOTUNNEL : if(Trans.type != ROTOTUNNEL) { Trans.type = ROTOTUNNEL; Trans.newtrans = TRUE; } break; 
	case MATHWATERZOOM : if(Trans.type != WATERZOOM) { Trans.type = WATERZOOM; Trans.newtrans = TRUE; } break; 
	case MATHGALAXY : if(Trans.type != GALAXY) { Trans.type = GALAXY; Trans.newtrans = TRUE; } break; 
	case MATHVERTIPERSP : if(Trans.type != VERTIPERSP) { Trans.type = VERTIPERSP; Trans.newtrans = TRUE; } break; 
	case MATHHORIPERSP : if(Trans.type != HORIPERSP) { Trans.type = HORIPERSP; Trans.newtrans = TRUE; } break; 
	case MATHCYCLOFLOWER : if(Trans.type != CYCLOFLOWER) { Trans.type = CYCLOFLOWER; Trans.newtrans = TRUE; } break; 
	case MATHILLUSION : if(Trans.type != ILLUSION) { Trans.type = ILLUSION; Trans.newtrans = TRUE; } break; 
	case MATHSWIRLOUT : if(Trans.type != SWIRLOUT) { Trans.type = SWIRLOUT; Trans.newtrans = TRUE; } break; 
	case MATHIMPLOSE : if(Trans.type != IMPLOSE) { Trans.type = IMPLOSE; Trans.newtrans = TRUE; } break; 
	case MATHSHADOWIN : if(Trans.type != SHADOWIN) { Trans.type = SHADOWIN; Trans.newtrans = TRUE; } break; 
	
	case MATHNOMOVE : if(Trans.type != NOMOVE) { Trans.type = NOMOVE; Trans.newtrans = TRUE; } break; 
	case MATHNOSHIFT : if(Trans.shift != NOSHIFT) Trans.newtrans = TRUE; Trans.shift = NOSHIFT; break;
	case MATHBASSSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = BASSSHIFT; break;
	case MATHTOPLEFTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = TOPLEFTSHIFT; break;
	case MATHTOPSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = TOPSHIFT; break;
	case MATHTOPRIGHTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = TOPRIGHTSHIFT; break;
	case MATHRIGHTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = RIGHTSHIFT; break;
	case MATHBOTTOMRIGHTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = BOTTOMRIGHTSHIFT; break;
	case MATHBOTTOMSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = BOTTOMSHIFT; break;
	case MATHBOTTOMLEFTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = BOTTOMLEFTSHIFT; break;
	case MATHLEFTSHIFT : if(Trans.shift == NOSHIFT) Trans.newtrans = TRUE; Trans.shift = LEFTSHIFT; break;

// mirrors
	case RANDOMMIRROR: RandomMirror = (RandomMirror)?FALSE:TRUE; break;
	case NOMIRROR: case HORMIRROR: case VERMIRROR: case DOUMIRROR:
		Mirror = Choix;
		ZeroMemory(Screen, XRes * YRes * (DepthMode>>3));
		ZeroMemory(ScreenBack, XRes * YRes * (DepthMode>>3));
		break;
// blitrotozoom
	case BLITNOZOOM: case BLITZOOMIN: case BLITZOOMOUT: case BLITZOOMBEAT: blitzoom = Choix; break;
	case BLITNOROTATE: case BLITROTATELEFT: case BLITROTATERIGHT: case BLITROTATEBEAT: blitrotation = Choix; break;
	case BLITENABLED: 
		if(blitrotozoom) {
			blitrotozoom = FALSE;
			end_rotoblit();
			/**/
			if(ViewMode == WINDOWED) SetSizeRectWnd();
			else if(ViewMode == DOCKED) SetRectDock(DockMode, TRUE);
			/**/
		}
		else {
			blitrotozoom = TRUE;
			/**/
			if(ViewMode == WINDOWED) SetSizeRectWnd();
			else if(ViewMode == DOCKED) SetRectDock(DockMode, TRUE);
			/**/
			HRESULT ddrval = CheckRes(ViewMode);
			if(ddrval != DD_OK) {
				blitrotozoom = FALSE;
				end_rotoblit();
			}
		}
		
		break;

		
//autres
	case MODE555: Mode555 = (Mode555)?FALSE:TRUE; break;
	case FPS: ShowFPS = (ShowFPS)?FALSE:TRUE; break;
	
	case LOADRANDOM: LoadRandom = (LoadRandom)?FALSE:TRUE; break;
	
	default: {
		if( (Choix >= LOADCONF)&&(Choix < SAVECONF + 0x1000) ) { // save et load config
			char** presets = GetPresets();
		
			if(Choix < SAVECONF) { // load config
				if(Choix == LOADCONF) {
					ZeroMemory(Screen, XRes * YRes * (DepthMode>>3));
					ZeroMemory(ScreenBack, XRes * YRes * (DepthMode>>3));
					RandomWaveForm = FALSE;
					RandomCercleWave = FALSE;
					RandomCercleBass = FALSE;
					RandomConfettis = FALSE;
					RandomMathShape = FALSE;
					RandomRotaLine = FALSE;
					RandomLittleWave = FALSE;
					RandomLittleMath = FALSE;
					RandomGeoGraph = FALSE;
					KillEffects();
					Render = NORENDER;
					Fade = FALSE;
					Mirror = NOMIRROR;
					Blend = FALSE;
					TransBefore = FALSE;
					Trans.enabled = FALSE;
					Trans.random = 0;
					Trans.newtrans = TRUE;
					Trans.type = NOMOVE;
					Trans.noise = FALSE;
					Trans.slide = FALSE;
					//Trans.fast = FALSE;
					Trans.shift = NO;
					RandomMirror = FALSE;
					Flashis = FALSE;
					//OnBassChange = FALSE;
					BeatEffect = FALSE;
					BeatTrans = FALSE;
					if(blitrotozoom) end_rotoblit();
					blitrotozoom = FALSE;
					blitzoom = BLITZOOMIN;
					blitrotation = BLITNOROTATE;

				}
				else init(presets[Choix - LOADCONF - 1]);
			}
			else {	// save config
				if(Choix == SAVECONF) {
					ChildWndDisplayed = 1;
					char* dir = (char*)malloc(MAX_PATH * sizeof(char));
					char* path = (char*)malloc(MAX_PATH * sizeof(char));
					char* fname = (char*)malloc(_MAX_FNAME * sizeof(char));
					OPENFILENAME o;
					
					memset(&o, 0, sizeof(OPENFILENAME));
					o.lStructSize  = sizeof(OPENFILENAME);
					o.hwndOwner = hMainWnd;
					o.lpstrFilter = "Space Preset File\0*.spc\0\0";
					GetIniConf(dir);
					o.lpstrInitialDir = dir;
					o.lpstrDefExt = "spc";
					o.lpstrTitle = "Save your Space preset as";
					o.nFilterIndex = 1;
					if(ViewMode == FULLSCREEN) {
						o.lpfnHook = (LPOFNHOOKPROC)ConfigNameDialogProc;
						o.Flags = OFN_EXPLORER | OFN_ENABLEHOOK;
					}
					else o.Flags = OFN_EXPLORER;
					o.nMaxFile = MAX_PATH;
					strcpy(path, "MyConfig");
					o.lpstrFile = path;
					o.lpstrFileTitle = fname;
					o.nMaxFileTitle = _MAX_FNAME;

					if(GetSaveFileName(&o)) {	// si on a le nom de fichier de sauvegarde
						char* i = fname;
						while((i - fname) < (o.nFileExtension - o.nFileOffset)) i++;
						*(--i) = '\0';	// on enlève l'extension
						if( (strcmp(fname, Version))&&(strcmp(fname, "default")) ) SaveConf(fname);	// et on sauvegarde
						else MessageBox(hMainWnd, "You must choose another name for save your preset", "Space - Info", MB_OK);
					}
					
					ChildWndDisplayed = 0;
					free(dir); free(path); free(fname);
				}
				else SaveConf(presets[Choix - SAVECONF - 1]);
			}
			
			UINT pos = 0;
			while(presets[pos++]) free(presets[pos - 1]);		
			free(presets);
		}
		else if( (Choix >= FS)&&(Choix < FS + 0x100)&&(lpDD) ) { // fullscreen modes
			if( (ViewMode != FULLSCREEN)||(HeightMode != ModeSupport[Choix - FS][1])||(WidthMode = ModeSupport[Choix - FS][0]) ) {
				WidthMode = ModeSupport[Choix - FS][0];
				HeightMode = ModeSupport[Choix - FS][1];
				if (ChangeViewMode(FULLSCREEN)) return Abandon(1, "Problème lors du changement de mode");
			}
		}
					 }
		break;
	
	case CLOSEPLUGIN : PostQuitMessage(0); 
	}

	return Result;
}


int Space::DDActive() { return (lpDD != NULL); }


void Space::ChangeEffects() {
// On recrée des effets (en accord avec les transformations ?)

	int nbWaveForm = 0, nbCercleWave = 0, nbCercleBass = 0, nbConfettis = 0;
	int nbRotaLine = 0, nbMathShape = 0, nbLittleWave = 0, nbLittleMath = 0;
	int nbGeoGraph = 0, coef, coef1;

	if( (Trans.enabled == FALSE)||(Trans.type == NOMOVE) ) {
		//waveform, geograph, cerclewave, littlewave, littlemath  et rotaline
		nbCercleWave = rand()%(MAX_CERCLEWAVE_EFFECTS>>1);
		while( ((rand()%10)>4)&&(nbCercleWave<MAX_CERCLEWAVE_EFFECTS) ) nbCercleWave++;
		
		nbGeoGraph = rand()%(MAX_GEOGRAPH_EFFECTS>>1);
		while( ((rand()%10)>4)&&(nbGeoGraph<MAX_GEOGRAPH_EFFECTS) ) nbGeoGraph++;
		
		nbWaveForm = rand()%(MAX_WAVEFORM_EFFECTS>>3);
		while( ((rand()%10)>6)&&(nbWaveForm<MAX_WAVEFORM_EFFECTS) ) nbWaveForm++;
		
		nbLittleWave = rand()%(MAX_LITTLEWAVE_EFFECTS>>4);
		while( ((rand()%10)>6)&&(nbLittleWave<MAX_LITTLEWAVE_EFFECTS) ) nbLittleWave++;
		
		nbLittleMath = rand()%(MAX_LITTLEMATH_EFFECTS>>2);
		while( ((rand()%10)>6)&&(nbLittleMath<MAX_LITTLEMATH_EFFECTS) ) nbLittleMath++;
		
		nbRotaLine = rand()%(MAX_ROTALINE_EFFECTS>>2);
		while( ((rand()%10)>5)&&(nbRotaLine<MAX_ROTALINE_EFFECTS) ) nbRotaLine++;
		
		//cerclebass
		if((rand()%5)>3) { 
			nbCercleBass++;
			if((rand()%7)<3) {
				nbCercleBass += 4;
				if((rand()%7)<3) nbCercleBass += 4;
			}
		}  
	
		//confettis
		if((rand()%7)<3) {
			nbConfettis++;
			while( ((rand()%10)<2)&&(nbConfettis<MAX_CONFETTIS_EFFECTS) )  nbConfettis++;
		}

		//mathshape
		if ((rand()%7) > 4) nbMathShape++; 
		if ((rand()%10) < 2) while( ((rand()%10)>5)&&(nbMathShape<MAX_MATHSHAPE_EFFECTS) ) nbMathShape++;
	}
	else switch(Trans.type) { 
	default: case ROTATION: case ZARB: case PETALES:
		//waveform, cerclewave et rotateline
		
		nbCercleWave = rand()%(MAX_CERCLEWAVE_EFFECTS>>1);
		while( ((rand()%10)>6)&&(nbCercleWave<MAX_CERCLEWAVE_EFFECTS) ) nbCercleWave++;
		
		nbGeoGraph = rand()%(MAX_GEOGRAPH_EFFECTS>>1);
		while( ((rand()%10)>6)&&(nbGeoGraph<MAX_GEOGRAPH_EFFECTS) ) nbGeoGraph++;
		
		nbWaveForm = rand()%(MAX_WAVEFORM_EFFECTS>>3);
		while( ((rand()%10)>7)&&(nbWaveForm<MAX_WAVEFORM_EFFECTS) ) nbWaveForm++;

		nbLittleWave = rand()%(MAX_LITTLEWAVE_EFFECTS>>4);
		while( ((rand()%10)>7)&&(nbLittleWave<MAX_LITTLEWAVE_EFFECTS) ) nbLittleWave++;
		
		nbLittleMath = rand()%(MAX_LITTLEMATH_EFFECTS>>2);
		while( ((rand()%10)>7)&&(nbLittleMath<MAX_LITTLEMATH_EFFECTS) ) nbLittleMath++;
		
		nbRotaLine = rand()%(MAX_ROTALINE_EFFECTS>>2);
		while( ((rand()%10)>6)&&(nbWaveForm<MAX_ROTALINE_EFFECTS) ) nbRotaLine++;

		//mathshape
		if ((rand()%9) > 5) nbMathShape++; 
		if( (rand()%10) < 7) while( ((rand()%10)>7)&&(nbMathShape<MAX_MATHSHAPE_EFFECTS) ) nbMathShape++;

		//cerclebass
		if((rand()%7)>3) { 
			nbCercleBass++;
			if((rand()%7)<4) {
				nbCercleBass += 4;
				if((rand()%7)<4) nbCercleBass += 4;
			}
		}  
	
		// confettis
		if((rand()%7)<3) {
			nbConfettis++;
			while( ((rand()%10)<3)&&(nbConfettis<MAX_CONFETTIS_EFFECTS) )  nbConfettis++;
		} break;

	case ZOOMIN: case TUNNEL: case ROTOZOOMIN:
		//waveform, cerclewave et rotateline
		if ((rand()%6) > 2) nbCercleWave++; 
		if ((rand()%10) < 3) while( ((rand()%10)>4)&&(nbCercleWave<MAX_CERCLEWAVE_EFFECTS) ) nbCercleWave++;
		
		if ((rand()%6) > 2) nbGeoGraph++; 
		if ((rand()%10) < 3) while( ((rand()%10)>4)&&(nbGeoGraph<MAX_GEOGRAPH_EFFECTS) ) nbGeoGraph++;
		
		if ((rand()%9) > 7) nbWaveForm++;
		if ((rand()%10) < 2) while( ((rand()%10)>6)&&(nbWaveForm<MAX_WAVEFORM_EFFECTS) ) nbWaveForm++;
		
		if ((rand()%9) > 6) nbLittleWave++;
		if ((rand()%10) < 4) while( ((rand()%10)>6)&&(nbLittleWave<MAX_LITTLEWAVE_EFFECTS) ) nbLittleWave++;
		
		if ((rand()%9) > 6) nbLittleMath++;
		if ((rand()%10) < 4) while( ((rand()%10)>6)&&(nbLittleMath<MAX_LITTLEMATH_EFFECTS) ) nbLittleMath++;
		
		if ((rand()%9) > 3) nbRotaLine++;
		while( ((rand()%10)>4)&&(nbWaveForm<MAX_ROTALINE_EFFECTS) ) nbRotaLine++;

		//mathshape
		if ((rand()%9) > 4) nbMathShape++; 
		if ((rand()%10) < 3) while( ((rand()%10)>5)&&(nbMathShape<MAX_MATHSHAPE_EFFECTS) ) nbMathShape++;
		

		//cerclebass
		if((rand()%5)>3) { 
			nbCercleBass++;
			if((rand()%8)<2) {
				nbCercleBass += 4;
				if((rand()%7)<1) nbCercleBass += 4;
			}
		}  
	
		// confettis
		coef = 4; if(Trans.type == TUNNEL) coef += 2;
		if((rand()%9)<coef) {
			nbConfettis++;
			while( ((rand()%15)<coef + 1)&&(nbConfettis<MAX_CONFETTIS_EFFECTS) )  nbConfettis++;
		} break;
	
	}//fin switch

	int i;

	if(RandomWaveForm) {
		while(WaveFormEff::nombre)
			delete WaveForm[WaveFormEff::nombre - 1];
		for(i = 0; i < nbWaveForm; i++) WaveForm[i] = new WaveFormEff(WaveForm);
	}
	if(RandomCercleWave) {
		if(RandomGeoGraph) coef1 = rand()%2;
		if( (coef1)||(nbCercleWave == 1) ) {
			while(CercleWaveEff::nombre)
				delete CercleWave[CercleWaveEff::nombre - 1];
			for(i = 0; i < nbCercleWave; i++) CercleWave[i] = new CercleWaveEff(CercleWave);
		}
	}
	if(RandomCercleBass) {
		while(CercleBassEff::nombre)
			delete CercleBass[CercleBassEff::nombre - 1];
		for(i = 0; i < nbCercleBass; i++) CercleBass[i] = new CercleBassEff();
	}
	if(RandomConfettis) {
		while(ConfettisEff::nombre)
			delete Confettis[ConfettisEff::nombre - 1];
		for(i = 0; i < nbConfettis; i++) Confettis[i] = new ConfettisEff();
	}
	if(RandomMathShape) {
		while(MathShapeEff::nombre)
			delete MathShape[MathShapeEff::nombre - 1];
		for(i = 0; i < nbMathShape; i++) MathShape[i] = new MathShapeEff();
	}
	if(RandomRotaLine) {
		while(RotaLineEff::nombre)
			delete RotaLine[RotaLineEff::nombre - 1];
		for(i = 0; i < nbRotaLine; i++) RotaLine[i] = new RotaLineEff(RotaLine);
	}
	if(RandomLittleWave) {
		while(LittleWaveEff::nombre)
			delete LittleWave[LittleWaveEff::nombre - 1];
		for(i = 0; i < nbLittleWave; i++) LittleWave[i] = new LittleWaveEff();
	}
	if(RandomLittleMath) {
		while(LittleMathEff::nombre)
			delete LittleMath[LittleMathEff::nombre - 1];
		for(i = 0; i < nbLittleMath; i++) LittleMath[i] = new LittleMathEff();
	}
	if(RandomGeoGraph) {
		if( (!coef1)||(nbCercleWave == 1) ) {
			if(nbCercleWave == 1) nbGeoGraph = max(2, nbGeoGraph);

			while(GeoGraphEff::nombre)
				delete GeoGraph[GeoGraphEff::nombre - 1];
			for(i = 0; i < nbGeoGraph; i++) GeoGraph[i] = new GeoGraphEff(GeoGraph);
		}
	}
	
	
	// il faut au moins quelquechose en dehors des cercles basses
	if( (WaveFormEff::nombre < 2)&&(!(CercleWaveEff::nombre))&&(!(ConfettisEff::nombre))&&(!(MathShapeEff::nombre))&&(!(RotaLineEff::nombre))&&(!(LittleWaveEff::nombre))&&(!(LittleMathEff::nombre))&&(!(GeoGraphEff::nombre)) ) {
		if(RandomMathShape) MathShape[0] = new MathShapeEff();
		else if(RandomWaveForm) {
			if(!WaveFormEff::nombre) WaveForm[0] = new WaveFormEff(WaveForm);
			if(Mirror != NOMIRROR) WaveForm[1] = new WaveFormEff(WaveForm);
		}
		else if(RandomGeoGraph) GeoGraph[0] = new GeoGraphEff(GeoGraph);
		else if(RandomCercleWave) CercleWave[0] = new CercleWaveEff(CercleWave);
		else if(RandomLittleWave) LittleWave[0] = new LittleWaveEff();
		else if(RandomLittleMath) LittleMath[0] = new LittleMathEff();
		else if(RandomRotaLine) RotaLine[0] = new RotaLineEff(RotaLine);
		else if(RandomConfettis) Confettis[0] = new ConfettisEff();
	}

	if(RandomMirror) {
		switch(rand()%4) {
		case 0 : Mirror = HORMIRROR; break;
		case 1 :	Mirror = VERMIRROR; break;
		case 2 : Mirror = DOUMIRROR; break;
		case 3 : Mirror = NOMIRROR;
		}
	
		ZeroMemory(Screen, XRes * YRes * (DepthMode>>3));
		ZeroMemory(ScreenBack, XRes * YRes * (DepthMode>>3));
	}
		
}


int Space::init(char* fname) {
	MathTrans::newtrans = TRUE;	
	if(Trans.sourcecoordtable) free(Trans.sourcecoordtable);
	if(Trans.presourcecoordtable) free(Trans.presourcecoordtable);
	Trans.sourcecoordtable = Trans.presourcecoordtable = NULL;
	if(Screen) ZeroMemory(Screen, XRes * YRes * (DepthMode>>3));
	if(ScreenBack) ZeroMemory(ScreenBack, XRes * YRes * (DepthMode>>3));

	KillEffects();
	
	SetConf(fname);	// chargement et initialisation des données

	CheckConf();

	//if(blitrotozoom) {
		if(ViewMode == WINDOWED) SetSizeRectWnd();
		else if( (ViewMode == DOCKED)&&strcmp(fname, Version) )
			SetRectDock(DockMode, TRUE);

		CheckRes(ViewMode, strcmp(fname, Version));
	//}

	return 0;
}


char** Space::GetPresets() {
	ULONG pre_alloue = 1;
	char* ini_file = (char*)malloc(MAX_PATH * sizeof(char));
	char** presets = (char**)malloc(pre_alloue * sizeof(char*));

	presets[0] = NULL;
	GetIniConf(ini_file);
	
	if(!(SetCurrentDirectory(ini_file))) 
		if(!(CreateDirectory(ini_file, NULL))) { free(ini_file); return presets; }
	
	{	// on est dans le repertoire winamp\plugin\space
		HANDLE hs;
		WIN32_FIND_DATA fd;
		fd.dwFileAttributes = FILE_ATTRIBUTE_ARCHIVE;

		if( (hs = FindFirstFile("*.spc", &fd)) != INVALID_HANDLE_VALUE) {
			if(!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {	// si ce n'est pas un repertoire
				char* i = fd.cFileName;
				while(*i++);	// on va à la fin du nom de fichier
				i -= 5;		
				if(i > fd.cFileName) {	// si on le nom du fichier est correct
					*i = '\0';	// on "enlève" l'extension
					presets[pre_alloue - 1] = (char*)malloc(_MAX_FNAME * sizeof(char));
					strcpy(presets[pre_alloue - 1], fd.cFileName);	// et on le copie
					pre_alloue++;
					presets = (char**)realloc(presets, pre_alloue * sizeof(char*));
				}
			}

			while(FindNextFile(hs, &fd)) { // tant qu'il y a des fichiers .spc
				if(!(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) { // on recommence
					char* i = fd.cFileName;
					while(*i++);
					i -= 5; 
					if(i > fd.cFileName) {
						*i = '\0';	// on "enlève" l'extension
						presets[pre_alloue - 1] = (char*)malloc(_MAX_FNAME * sizeof(char));
						strcpy(presets[pre_alloue - 1], fd.cFileName);
						pre_alloue++;
						presets = (char**)realloc(presets, pre_alloue * sizeof(char*));
					}
				}
			}
			
			presets[pre_alloue - 1] = NULL;	// on met le dernier nom de fichier = NULL
			FindClose(hs);	// et on ferme le handle
		}
	}	// fin if(SetCurrentDirectory)...

	free(ini_file);
	return presets;	//on retourne la liste des presets (il ne faut pas oublier de la libérer)
}

/*
int Space::CheckSection(char* section) {
	UINT i = 0, pb = FALSE;
	
	while( (section[i])&&(!pb) ) {
		if( (section[i] == '[')||(section[i] == ']') ) pb = TRUE;
		i++;
	}

	if(i>50) pb = TRUE;

	return pb;
}
*/

/*
void Space::SaveDefaultConfig() {
	char ini_file[MAX_PATH];
	char string[32];

	GetIniConf(ini_file);
	strcat(ini_file, "space.ini");
	
	//default config
	wsprintf(string,"%d",Mirror);
	if(!WritePrivateProfileString("default","Mirror",string,ini_file))	{
		MessageBox(hMainWnd, "Default Config cannot be writted !", "Error", MB_OK);
		return;
	}
	WritePrivateProfileString("Vision Life","Mirror",string,ini_file);
	WritePrivateProfileString("Bass Trip","Mirror",string,ini_file);
	WritePrivateProfileString("Poudroze","Mirror",string,ini_file);
	WritePrivateProfileString("Flashouille","Mirror",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Mirror","123",ini_file);
	wsprintf(string,"%d",RandomMirror);
	WritePrivateProfileString("default","Random_Mirror","1" ,ini_file);
	WritePrivateProfileString("Vision Life","Random_Mirror","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_Mirror","0",ini_file);
	WritePrivateProfileString("Poudroze","Random_Mirror","1",ini_file);
	WritePrivateProfileString("Flashouille","Random_Mirror",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Random_Mirror","0",ini_file);
	wsprintf(string,"%d",Blend);
	WritePrivateProfileString("default","Blend",string,ini_file);
	WritePrivateProfileString("Vision Life","Blend",string,ini_file);
	WritePrivateProfileString("Bass Trip","Blend",string,ini_file);
	WritePrivateProfileString("Poudroze","Blend",string,ini_file);
	WritePrivateProfileString("Flashouille","Blend",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Blend","1",ini_file);
	wsprintf(string,"%d",TransBefore);
	WritePrivateProfileString("default","Trans_Before",string,ini_file);
	WritePrivateProfileString("Vision Life","Trans_Before",string,ini_file);
	WritePrivateProfileString("Bass Trip","Trans_Before",string,ini_file);
	WritePrivateProfileString("Poudroze","Trans_Before",string,ini_file);
	WritePrivateProfileString("Flashouille","Trans_Before","1",ini_file);
	WritePrivateProfileString("Kalei Shape","Trans_Before",string,ini_file);
	wsprintf(string,"%d",RandomWaveForm);
	WritePrivateProfileString("default","Random_WaveForm",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_WaveForm",string,ini_file);
	WritePrivateProfileString("Bass Trip","Random_WaveForm",string,ini_file);
	WritePrivateProfileString("Poudroze","Random_WaveForm",string,ini_file);
	WritePrivateProfileString("Flashouille","Random_WaveForm","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_WaveForm","0",ini_file);
	wsprintf(string,"%d",RandomCercleWave);
	WritePrivateProfileString("default","Random_CercleWave",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_CercleWave","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_CercleWave",string,ini_file);
	WritePrivateProfileString("Poudroze","Random_CercleWave",string,ini_file);
	WritePrivateProfileString("Flashouille","Random_CercleWave","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_CercleWave","0",ini_file);
	wsprintf(string,"%d",RandomCercleBass);
	WritePrivateProfileString("default","Random_CercleBass",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_CercleBass","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_CercleBass","0",ini_file);
	WritePrivateProfileString("Poudroze","Random_CercleBass",string,ini_file);
	WritePrivateProfileString("Flashouille","Random_CercleBass","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_CercleBass","0",ini_file);
	wsprintf(string,"%d",RandomConfettis);
	WritePrivateProfileString("default","Random_Confettis",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_Confettis","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_Confettis","0",ini_file);
	WritePrivateProfileString("Poudroze","Random_Confettis",string,ini_file);
	WritePrivateProfileString("Flashouille","Random_Confettis","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_Confettis","0",ini_file);
	wsprintf(string,"%d",RandomMathShape);
	WritePrivateProfileString("default","Random_MathShape",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_MathShape","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_MathShape","0",ini_file);
	WritePrivateProfileString("Poudroze","Random_MathShape","0",ini_file);
	WritePrivateProfileString("Flashouille","Random_MathShape","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_MathShape","0",ini_file);
	wsprintf(string,"%d",WaveFormEff::nombre);
	WritePrivateProfileString("default","Nb_WaveForm","1",ini_file);
	WritePrivateProfileString("Vision Life","Nb_WaveForm",string,ini_file);
	WritePrivateProfileString("Bass Trip","Nb_WaveForm","2",ini_file);
	WritePrivateProfileString("Poudroze","Nb_WaveForm",string,ini_file);
	WritePrivateProfileString("Flashouille","Nb_WaveForm","1",ini_file);
	WritePrivateProfileString("Kalei Shape","Nb_WaveForm",string,ini_file);
	wsprintf(string,"%d",CercleWaveEff::nombre);
	WritePrivateProfileString("default","Nb_CercleWave","1",ini_file);
	WritePrivateProfileString("Vision Life","Nb_CercleWave","1",ini_file);
	WritePrivateProfileString("Bass Trip","Nb_CercleWave","4",ini_file);
	WritePrivateProfileString("Poudroze","Nb_CercleWave",string,ini_file);
	WritePrivateProfileString("Flashouille","Nb_CercleWave","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Nb_CercleWave","0",ini_file);
	wsprintf(string,"%d",CercleBassEff::nombre);
	WritePrivateProfileString("default","Nb_CercleBass","1",ini_file);
	WritePrivateProfileString("Vision Life","Nb_CercleBass",string,ini_file);
	WritePrivateProfileString("Poudroze","Nb_CercleBass",string,ini_file);
	WritePrivateProfileString("Flashouille","Nb_CercleBass",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Nb_CercleBass",string,ini_file);
	wsprintf(string,"%d",MAX_CERCLEBASS_EFFECTS);
	WritePrivateProfileString("Bass Trip","Nb_CercleBass",string,ini_file);
	wsprintf(string,"%d",ConfettisEff::nombre);
	WritePrivateProfileString("default","Nb_Confettis","1",ini_file);
	WritePrivateProfileString("Vision Life","Nb_Confettis","2",ini_file);
	WritePrivateProfileString("Bass Trip","Nb_Confettis","4",ini_file);
	WritePrivateProfileString("Poudroze","Nb_Confettis","1",ini_file);
	WritePrivateProfileString("Flashouille","Nb_Confettis","6",ini_file);
	WritePrivateProfileString("Kalei Shape","Nb_Confettis","1",ini_file);
	wsprintf(string,"%d",MathShapeEff::nombre);
	WritePrivateProfileString("default","Nb_MathShape","1",ini_file);
	WritePrivateProfileString("Vision Life","Nb_MathShape","1",ini_file);
	WritePrivateProfileString("Bass Trip","Nb_MathShape","0",ini_file);
	WritePrivateProfileString("Poudroze","Nb_MathShape","1",ini_file);
	WritePrivateProfileString("Flashouille","Nb_MathShape",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Nb_MathShape","3",ini_file);
	wsprintf(string,"%d", Flashis);
	WritePrivateProfileString("default","Flashis",string,ini_file);
	WritePrivateProfileString("Vision Life","Flashis","1",ini_file);
	WritePrivateProfileString("Bass Trip","Flashis","1",ini_file);
	WritePrivateProfileString("Poudroze","Flashis","0",ini_file);
	WritePrivateProfileString("Flashouille","Flashis","1",ini_file);
	WritePrivateProfileString("Kalei Shape","Flashis","0",ini_file);
	wsprintf(string,"%d",Fade);
	WritePrivateProfileString("default","Fade",string,ini_file);
	WritePrivateProfileString("Vision Life","Fade",string,ini_file);
	WritePrivateProfileString("Bass Trip","Fade",string,ini_file);
	WritePrivateProfileString("Poudroze","Fade","1",ini_file);
	WritePrivateProfileString("Flashouille","Fade","1",ini_file);
	WritePrivateProfileString("Kalei Shape","Fade","1",ini_file);
	wsprintf(string,"%d",Render);
	WritePrivateProfileString("default","Render",string,ini_file);
	WritePrivateProfileString("Vision Life","Render","71",ini_file);
	WritePrivateProfileString("Bass Trip","Render","69",ini_file);
	WritePrivateProfileString("Poudroze","Render","64",ini_file);
	WritePrivateProfileString("Flashouille","Render","64",ini_file);
	WritePrivateProfileString("Kalei Shape","Render","64",ini_file);
	wsprintf(string,"%d",Trans.enabled);
	WritePrivateProfileString("default","Trans",string,ini_file);
	WritePrivateProfileString("Vision Life","Trans",string,ini_file);
	WritePrivateProfileString("Bass Trip","Trans",string,ini_file);
	WritePrivateProfileString("Poudroze","Trans",string,ini_file);
	WritePrivateProfileString("Flashouille","Trans",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Trans",string,ini_file);
	wsprintf(string,"%d",Trans.random);
	WritePrivateProfileString("default","Random_Trans",string,ini_file);
	WritePrivateProfileString("Vision Life","Random_Trans","0",ini_file);
	WritePrivateProfileString("Bass Trip","Random_Trans","0",ini_file);
	WritePrivateProfileString("Poudroze","Random_Trans",string,ini_file);
	WritePrivateProfileString("Flashouille","Random_Trans","0",ini_file);
	WritePrivateProfileString("Kalei Shape","Random_Trans",string,ini_file);
	wsprintf(string,"%d",Trans.slide);
	WritePrivateProfileString("default","Trans_Slide",string,ini_file);
	WritePrivateProfileString("Vision Life","Trans_Slide",string,ini_file);
	WritePrivateProfileString("Bass Trip","Trans_Slide",string,ini_file);
	WritePrivateProfileString("Poudroze","Trans_Slide",string,ini_file);
	WritePrivateProfileString("Flashouille","Trans_Slide",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Trans_Slide","1",ini_file);
	wsprintf(string,"%d",Trans.noise);
	WritePrivateProfileString("default","Trans_Noise",string,ini_file);
	WritePrivateProfileString("Vision Life","Trans_Noise",string,ini_file);
	WritePrivateProfileString("Bass Trip","Trans_Noise",string,ini_file);
	WritePrivateProfileString("Poudroze","Trans_Noise","1",ini_file);
	WritePrivateProfileString("Flashouille","Trans_Noise",string,ini_file);
	WritePrivateProfileString("Kalei Shape","Trans_Noise",string,ini_file);
	wsprintf(string,"%d",Trans.type);
	WritePrivateProfileString("default","Trans_Type",string,ini_file);
	WritePrivateProfileString("Vision Life","Trans_Type","6",ini_file);
	WritePrivateProfileString("Bass Trip","Trans_Type","5",ini_file);
	WritePrivateProfileString("Poudroze","Trans_Type",string,ini_file);
	WritePrivateProfileString("Flashouille","Trans_Type","4",ini_file);
	WritePrivateProfileString("Kalei Shape","Trans_Type",string,ini_file);
}
*/

HRESULT WINAPI MakeSupportListe(LPDDSURFACEDESC2 ddsd, LPVOID lpcontext) {
	cont1* cont = (cont1*)lpcontext;

	if( (ddsd->ddpfPixelFormat.dwRGBBitCount == 16)&&(ddsd->dwWidth <= MAX_WIDTH)&&(ddsd->dwHeight <= MAX_HEIGHT)&&(cont->i < 29) ) {
		cont->spc->ModeSupport[cont->i][0] = ddsd->dwWidth; cont->spc->ModeSupport[cont->i][1] = ddsd->dwHeight; (cont->i)++;
	}

	return DDENUMRET_OK ;

}

int Space::init_rotoblit(int nb_x, int nb_y) {
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;

	// normalement nb_x est toujours multiple de 4 ...
	///**/MessageBox(NULL,"pass1","info",MB_OK);
	
	end_rotoblit();
		
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth= nb_x; ddsd.dwHeight= nb_y;
	ddrval = lpDD->CreateSurface(&ddsd, &(rotoblitstruct.SurfBack), NULL);
	if(ddrval != DD_OK) { MessageBox(NULL, "Error: Creation Surface Back dans Init_rotoblit", "info", MB_OK); goto error; }

	///**/MessageBox(NULL,"pass2","info",MB_OK);

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth= nb_x; ddsd.dwHeight= nb_y;
	ddrval = lpDD->CreateSurface(&ddsd, &(rotoblitstruct.SurfDest), NULL);
	if(ddrval != DD_OK) { MessageBox(NULL, "Error: Creation Surface Dest dans Init_rotoblit", "info", MB_OK); goto error; }

	///**/MessageBox(NULL,"pass3","info",MB_OK);

	return ddrval;

error:
	end_rotoblit();
	return ddrval;
}


void Space::end_rotoblit() {
	if(rotoblitstruct.SurfBack) { rotoblitstruct.SurfBack->Release(); rotoblitstruct.SurfBack = NULL; }
	if(rotoblitstruct.SurfDest) { rotoblitstruct.SurfDest->Release(); rotoblitstruct.SurfDest = NULL; }
}


void Space::rotoblit(USHORT* tabdata, USHORT* tabdataback, int rotation, int zoom, int accel, int beat, int nb_x, int nb_y) {
	static ULONG angle = 0;
	DDSURFACEDESC2 ddsd;
	HRESULT ddrval = DD_OK;
	int mi_x = (nb_x>>1), mi_y = (nb_y>>1);//, i;
	DDBLTFX ddbltfx;
	RECT rectsrc, rectdest;
	int minzoom = 1;	// zoom minimum
	static int sens = 1;
	static int sens2 = 1;
	/**/static int old_accel = 0;
	int Flags, coefzoom = 350;//500;

	/**/if( (Mirror == HORMIRROR)||(Mirror == DOUMIRROR) ) ddsd.dwHeight = mi_y >>= 1;
	/**/if( (Mirror == VERMIRROR)||(Mirror == DOUMIRROR) ) ddsd.dwHeight = mi_x >>= 1;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );	
	ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.lpSurface = (void*)tabdataback;	// normalement suffisament alloué
	ddsd.lPitch = ((nb_x>>2)<<3);
	ddsd.dwHeight = nb_y;
	ddsd.dwWidth = nb_x;
//#ifdef _DEBUG
//	if(debugfile) fprintf(debugfile, "SetSurfaceDesc surf back dans Rotoblit (%d X %d, %d)", nb_x, nb_y, ((nb_x>>2)<<3));
//#endif
	if((ddrval = rotoblitstruct.SurfBack->SetSurfaceDesc(&ddsd, 0)) != DD_OK) {
#ifdef _DEBUG
		print_log("Error: SetSurfaceDesc sur Surface Back dans Rotoblit");
#endif
		goto error;
	}
	
	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof( ddsd );	
	ddsd.dwFlags = DDSD_LPSURFACE | DDSD_PITCH | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.lPitch = ((nb_x>>2)<<3);
	ddsd.dwHeight = nb_y;
	ddsd.dwWidth = nb_x;
	ddsd.lpSurface = tabdata;	// on peut faire directos le blit dans le tab. data dest...
	if((ddrval = rotoblitstruct.SurfDest->SetSurfaceDesc(&ddsd, 0)) != DD_OK) {
#ifdef _DEBUG
		print_log("Error: SetSurfaceDesc sur Surface Dest dans Rotoblit");
#endif
		goto error;
	}
	
	// gestion angle de rotation et zoom
	angle = (accel<<3);
	if(rotation == BLITROTATERIGHT) angle = 36000 - int(angle);	// angle opposé
	else if(rotation == BLITROTATEBEAT) { 
		if(beat) sens = -sens;
		if(sens == -1) angle = 36000 - int(angle);
	}
	
	accel = (accel>128)?128:accel;
	rectsrc.left = 0;
	rectsrc.top = 0;
	/**/
	if( (Mirror == NOMIRROR)||(Mirror == HORMIRROR) ) rectsrc.right = nb_x;
	else rectsrc.right = (nb_x>>1) + 1;
	/**/
	/**/
	if( (Mirror == NOMIRROR)||(Mirror == VERMIRROR) ) rectsrc.bottom = nb_y;
	else rectsrc.bottom = (nb_y>>1) + 1;
	/**/
	rectdest.left = 0;
	rectdest.top = 0;
	/**/
	if( (Mirror == NOMIRROR)||(Mirror == HORMIRROR) ) rectdest.right = nb_x;
	else rectdest.right = (nb_x>>1) + 1;
	/**/
	/**/
	if( (Mirror == NOMIRROR)||(Mirror == VERMIRROR) ) rectdest.bottom = nb_y;
	else rectdest.bottom = (nb_y>>1) + 1;
	/**/
	
	if(zoom == BLITZOOMIN) sens2 = 1;
	else if(zoom == BLITZOOMOUT) sens2 = -1;
	else if( (zoom == BLITZOOMBEAT)&&(beat) ) sens2 = -sens2;
		
	///**/ accel += old_accel;
	///**/ old_accel = accel - old_accel;
	///**/ accel >>= 1;
	
	if(zoom != BLITNOZOOM) {
		if(sens2 == 1) {
			rectsrc.left += minzoom + (mi_x*accel/coefzoom);
			/**/
			if( (Mirror == NOMIRROR)||(Mirror == HORMIRROR) )
				rectsrc.right -= minzoom + (mi_x*accel/coefzoom);
			/**/
			rectsrc.top += minzoom + (mi_y*accel/coefzoom);
			/**/
			if( (Mirror == NOMIRROR)||(Mirror == VERMIRROR) )
				rectsrc.bottom -= minzoom + (mi_y*accel/coefzoom);
			/**/
		}
		else {
			rectdest.left += minzoom + (mi_x*accel/coefzoom);
			/**/
			if( (Mirror == NOMIRROR)||(Mirror == HORMIRROR) )
				rectdest.right -= minzoom + (mi_x*accel/coefzoom);
			/**/
			rectdest.top += minzoom + (mi_y*accel/coefzoom);
			/**/
			if( (Mirror == NOMIRROR)||(Mirror == VERMIRROR) )
				rectdest.bottom -= minzoom + (mi_y*accel/coefzoom);
			/**/
		}
	}
	

	ZeroMemory(&ddbltfx, sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwRotationAngle = angle;
	
	if(rotation != BLITNOROTATE) Flags = DDBLT_WAIT | DDBLT_ROTATIONANGLE;
	else Flags = DDBLT_WAIT;
	
	//ddrval = rotoblitstruct.SurfDest->Blt(&rectdest, rotoblitstruct.SurfBack, &rectsrc, Flags, NULL); 
	ddrval = rotoblitstruct.SurfDest->Blt(&rectdest, rotoblitstruct.SurfBack, &rectsrc, Flags, &ddbltfx); 
	
	if (ddrval == DDERR_INVALIDPARAMS) affiche("Error: Blit Surface Dest: Paramètres invalides");
	else if(ddrval == DDERR_NOROTATIONHW) { affiche("Error Blit: Désolé, votre carte vidéo ne supporte pas la rotation"); blitrotation = BLITNOROTATE; } 
	else if(ddrval != DD_OK) Abandon(ddrval, "Error: Blit Surface Dest dans rotoblit", TRUE);

	
error:;
}



// uniquement pour le mode windowed
//void Space::SetWindowSize() {
void Space::SetSizeRectWnd(int width, int height, int style, int exstyle, int *dec) {
	RECT r;
	POINT p;
	int my_width = WidthWin, my_height = HeightWin;
	int my_style = style;
	int my_exstyle = exstyle;

	// windowed mode uniquement: WidthMode et HeightMode sont corrects
	// mode dock: on passe les params
	if(width) my_width = width;
	if(height) my_height = height;
	
	/**/
	if(blitrotozoom) {
		int sav_width = my_width;
		
		if(ResMode == AUTORES) my_width = ((my_width>>2)<<2);
		else if(ResMode == AUTODEMIRES) my_width = ((my_width>>3)<<3);
		else my_width = ( ( my_width >= ((DefXRes>>2)<<2) )?my_width:((my_width>>2)<<2) );
	
		if(!width) WidthWin = my_width;
		else *dec = sav_width - my_width;
	}
	/**/

	p.x = p.y = 0;
	ClientToScreen(hMainWnd, &p);
	r.left = p.x; r.top = p.y;
	r.right = p.x + my_width - 1; r.bottom = p.y + my_height - 1;
	AdjustWindowRectEx(&r, my_style, NULL, my_exstyle);
	MoveWindow(hMainWnd, r.left, r.top, (r.right - r.left + 1), (r.bottom - r.top + 1), TRUE);
	ShowWindow(hMainWnd, SW_SHOW);
}

