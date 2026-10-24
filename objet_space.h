#ifndef _SPACE_H_
#define _SPACE_H_

//#define DIRECTDRAW_VERSION         0x0300

#include <windows.h>
#include "ddraw3.h"
//#include <ddraw.h>
#include "vis.h"
#include "objet_effet.h"
#include "objet_transformation.h"
#include "resource.h"

#ifdef  _DEBUG
#include "log.h"				// pour le debug	
#endif

// on doit avoir des valeurs différentes à chaque foir, car elles sont utilisées pour le menu
#define FPS			0x9
#define FLASHIS		0x17

#define FULLSCREEN	0x7		// définitions pour les modes de vue
#define DOCKED		0x8	
#define WINDOWED	0xE
#define BACKGROUND	0xB

#define AUTORES		0x2
#define AUTODEMIRES	0x3
#define DEFINEDRES	0x4

/*
#define SLOWSPEED		0x28	// 40 ms soit 25 FPS max
#define NORMALSPEED		0xC		// 12 ms soit 90 FPS max env
#define FASTSPEED		0x1		// 1 ms !!
#define REALTIMESPEED	0x6		// peut pas mettre 0 car un menu ne retourne cette valeur que si ça foire
#define LIMITFPS60		0x96
#define LIMITFPS30		0x9B
*/
#define SPEED20		0x28	// 25 FPS max
#define SPEED30		0xC		// 30 FPS max 
#define SPEED45		0x1		// 45 FPS max
#define SPEED60		0x6		// 60 FPS max
#define SPEEDMAX	0xA		// maximun de FPS


#define CLOSEPLUGIN 0x5

#define ADDWAVEFORM 0xD
#define REMOVEWAVEFORM 0x14
#define CLEARWAVEFORM 0x16
#define ALLWAVEFORM 0x1C

#define ADDCERCLEWAVE 0x1D
#define REMOVECERCLEWAVE 0x1E
#define CLEARCERCLEWAVE 0x1F
#define ALLCERCLEWAVE 0x20

#define ADDCERCLEBASS 0x22
#define REMOVECERCLEBASS 0x23
#define CLEARCERCLEBASS 0x24
#define ALLCERCLEBASS 0x25

#define ADDCONFETTIS 0x26
#define REMOVECONFETTIS 0x27
#define CLEARCONFETTIS 0x29
#define ALLCONFETTIS 0x30

#define ADDMATHSHAPE 0x74
#define REMOVEMATHSHAPE 0x75
#define CLEARMATHSHAPE 0x76
#define ALLMATHSHAPE 0x77

#define ADDROTALINE 0x18
#define REMOVEROTALINE 0x19
#define CLEARROTALINE 0x1A
#define ALLROTALINE 0x1B

#define ADDLITTLEWAVE 0x97
#define REMOVELITTLEWAVE 0x82
#define CLEARLITTLEWAVE 0x99
#define ALLLITTLEWAVE 0x9A

#define ADDLITTLEMATH 0x10C
#define REMOVELITTLEMATH 0x10D
#define CLEARLITTLEMATH 0x110
#define ALLLITTLEMATH 0x111

#define ADDGEOGRAPH 0x112
#define REMOVEGEOGRAPH 0x113
#define CLEARGEOGRAPH 0x114
#define ALLGEOGRAPH 0x115

#define MODE555 0x15

#define NORENDER	0x40
#define CLEARSCREEN 0x41
#define FADEOUT		0x42
#define QBLUR		0x43
#define NBLUR		0x45
#define SBLUR		0x47

//les transfomations
#define MATHENABLED	0x49
#define MATHNORANDOM 0x4A 
#define MATHROTOZOOMIN 0x4C
#define MATHZOOMIN 0x4D
#define MATHZOOMOUT 0x116
#define MATHROTATION 0x4E
#define MATHPETALES 0x4F
#define MATHZARB 0x50
#define MATHTUNNEL 0x51
#define MATHTRANSBEFORE 0x6D
#define MATHNEXT 0x52
#define MATHSLIDE 0x6F
#define MATHRANDOM 0x70
#define MATHNOISE 0x71
#define MATHNOMOVE 0x7C
#define MATHCYCLONE 0x7D
#define MATHHORIZOOM 0x7F
#define MATHVERTIZOOM 0x80
#define MATHRAYSUN 0x81
#define MATHBIROTOZOOM 0x83
#define MATHBIZOOM 0x84
#define MATHROTOSUN 0x85
#define MATHBOX 0x86
#define MATHROTOTUNNEL 0x87
#define MATHWATERZOOM 0x93
#define MATHGALAXY 0x94
#define MATHVERTIPERSP 0x106
#define MATHHORIPERSP 0x107
#define MATHCYCLOFLOWER 0x108
#define MATHILLUSION 0x109
#define MATHSWIRLOUT 0x10A
#define MATHIMPLOSE 0x10B
#define MATHSHADOWIN 0x117
#define MATHROTOZOOMOUT 0x118

#define MATHTRANSFAST 0x7E

//les shifts
#define MATHNOSHIFT 0x88
#define MATHBASSSHIFT 0x89
#define MATHTOPLEFTSHIFT 0x8A
#define MATHTOPSHIFT 0x8B
#define MATHTOPRIGHTSHIFT 0x8C
#define MATHRIGHTSHIFT 0x8D
#define MATHBOTTOMRIGHTSHIFT 0x8E
#define MATHBOTTOMSHIFT 0x8F
#define MATHBOTTOMLEFTSHIFT 0x90
#define MATHLEFTSHIFT 0x91

#define LOADRANDOM 0x92
#define MATHMOVING 0x95

#define BLEND 0x6E

#define RANDOMWAVEFORM 0x53 
#define RANDOMCERCLEWAVE 0x54 
#define RANDOMCONFETTIS 0x55 
#define RANDOMCERCLEBASS 0x56 
#define RANDOMMATHSHAPE 0x73 
#define RANDOMROTALINE 0x21 
#define RANDOMLITTLEWAVE 0x98 
#define RANDOMGEOGRAPH 0x10E 
#define RANDOMLITTLEMATH 0x10F 

#define NOMIRROR 0x78
#define HORMIRROR 0x79
#define VERMIRROR 0x7A
#define DOUMIRROR 0x7B

#define BG160 0x57
#define BG200 0x58
#define BG240 0x59
#define BG400 0x5A
#define BG512 0x5B
#define BG640 0x5C
#define BG800 0x5D
#define BG1024 0x5E

#define BOTTOMDOCK 0x5F
#define TOPDOCK 0x60
#define LEFTDOCK 0x61
#define RIGHTDOCK 0x62
#define LITTLEDOCK 0x63

#define WIN320 0x64
#define WIN400 0x65
#define WIN512 0x66
#define WIN640 0x67
#define WINDEFINED 0x6A

#define RANDOMMIRROR 0x4B 

//#define ONBASSCHANGE 0x9B
#define BEATEFFECT 0x9B
#define BEATTRANS 0x9C


#define LOADCONF 0x1000
#define SAVECONF 0x2000

#define BLITNOZOOM 0x9D
#define BLITZOOMIN 0x9E
#define BLITZOOMOUT 0x9F
#define BLITZOOMBEAT 0x100
#define BLITNOROTATE 0x101
#define BLITROTATELEFT 0x102
#define BLITROTATERIGHT 0x103
#define BLITROTATEBEAT 0x104
#define BLITENABLED 0x105

#define FS 0x3000
// le reste ne fait pas partie des élément du menu


#define MAX_WIDTH 1024
#define MAX_HEIGHT 768

extern char szAppName[];
extern winampVisModule module;

// procédure de traitement de la fenêtre de saisie de résolution
LRESULT CALLBACK DefinedResDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
// procédure de traitement de la fenêtre de sauvegarde de configuration
UINT CALLBACK ConfigNameDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

// crée la liste des modes (mis dans ModeSupport[30][2]
HRESULT WINAPI MakeSupportListe(LPDDSURFACEDESC2, LPVOID);


#define affiche(x) MessageBox(NULL, x, "info", MB_OK)
 
// struct pour roto blitter
typedef struct _rotoblit {
	LPDIRECTDRAWSURFACE4 SurfBack, SurfDest;
	int SurfBackPitch, SurfDestPitch;
} ROTOBLITDATA;


typedef struct context1 {
	UINT i, retmenu;
	struct Space* spc;
} cont1;

//class Space {
struct Space {

//données
	// données systèmes
	LPDIRECTDRAW            lpDD1;			// objet DirectDraw 
	LPDIRECTDRAW4           lpDD;			// objet DirectDraw3 
	LPDIRECTDRAWSURFACE4    lpDDSPrimary;	// surface primaire DirectDraw (celle qui représente l'écran)
	LPDIRECTDRAWSURFACE4    lpDDSBack;		// surface secondaire DirectDraw (celle qui représente l'écran)
	LPDIRECTDRAWCLIPPER     lpDDClipper;    // Clippeur DirectDraw (celle qui représente l'écran)
	DDCAPS CapsDrv;							// Capacités directdraw de la carte vidéo
	void *SurfaceBackPointer;	// Pointeur sur la surface "Back"
	void *SurfacePrimPointer;	// Pointeur sur la surface Primary
	long SurfacePrimaryPitch;	// Pitch (longueur ligne en octet) Primary Surface
	long SurfaceBackPitch;		// Pitch (longueur ligne en octet) Back Surface
	DDPIXELFORMAT SurfacePrimaryPixFormat;	// Format des pixels de la primary surface (utile pour le mode background)

	USHORT *Screen, *ScreenBack;			// tableaux de calcul des images (rendu)

	// données configuration
	int ModeFounded;		// TRUE si on a crée la liste des modes
	int ModeSupport[30][2];		// tableau de tous les modes (width et height) supportés
	int WidthMode;		// largeur et hauteur du mode fullscreen ou actuel
	int HeightMode;
	int DepthMode;		// Nombre de bits par pixel (16, 24, 32)
	int WidthBack;		// largeur et hauteur du mode background
	int HeightBack;
	UINT DockMode;		// spécifie le mode d'affichage dock
	int Speed;			// vitesse du plugin (temps en ms entre 2 appels du rendu)
	int Mirror;			// Spécifie le type mirroir actuel
	int Mode555;		// Placé à 1 (TRUE) si le mode 16 bits doit être vue comme un mode 15 bits
	int Render;			// définie le rendu en cours
	int Fade;			// TRUE si on a un fade, FALSE sinon
	int Blend;			// TRUE si on fait les blur avant les effets
	//int LimitFPS;		// 1 si le nombre de FPS est limité à 60, 2 pour 30 fps, 0 si pas de limite
	int TransBefore;	// TRUE si on fait les trans avant les effets (donne mieux pour le kaléidoscope)
	int RandomWaveForm;	// TRUE si on doit changer les effets 'aléatoirement' automatiquement sinon FALSE
	int RandomCercleWave;			
	int RandomCercleBass;			
	int RandomConfettis;			
	int RandomMathShape;			
	int RandomRotaLine;			
	int RandomLittleWave;			
	int RandomLittleMath;			
	int RandomGeoGraph;			
	int SupportMMX;		// TRUE si le processeur supporte les instructions MMX, sinon FALSE
	int DispModeHasChanged;	//TRUE si le mode d'affichage a été changé lors de l'initialisation du plugin
	int RandomMirror;	// TRUE si les mirros sont selectionnés au hasard			
	int LoadRandom;		// TRUE si les load sont selectionés au hasard
	//bool OnBassChange;	// TRUE si les effets réagissent aux beats
	bool BeatEffect;	// TRUE si les effets réagissent aux beats
	bool BeatTrans;		// TRUE si les trans réagissent aux beats
	bool blitrotozoom;	// TRUE si le rotozoom par blit est activé, sinon FALSE
	int blitrotation;	// = BLITNOROTATE, BLITROTATELEFT, BLITROTATERIGHT ou BLITROTATEBEAT
	int blitzoom;		// = BLITNOZOOM, BLITZOOMIN, BLITZOOMOUT ou BLITZOOMBEAT
	bool Beat;			// TRUE si un battement a été détecté (dépend de BassDetected, voir à arranger ...)
	ROTOBLITDATA rotoblitstruct;


	// données relatives à la detection des basses
	UCHAR BassDetected;		// mis à 1 ou plus si une basse a été detectée sinon à 0

	static char* Version;
	HWND hMainWnd; 			// Handle de la fenêtre principale
	int XWin;				// Coordonnées de la fenêtre
	int YWin;
	int WidthWin;			// largeur et hauteur de la fenêtre pricipale
	int HeightWin;
	int XRes;				// Render Resolution
	int YRes;
	int DefXRes;			// XRes et YRes définie par l'utilisateur
	int DefYRes;

	int ChildWndDisplayed;	// Placé à 1 si une fenêtre fils est affichée (nécessaire pour le mode fullscreen)
	int ViewMode;			// mode actuel : windowed, dock, fullscreen
	int ResMode;			// 1: auto, 2: auto-demi, 3:defined
	int NoDisplayTransform;	// Placé à 1 lorsque l'on ne doit pas calculer les transformations (lors du sizing...)
	int ShowFPS;			// TRUE si le nombre de frames / secondes doit être affiché
	int Flashis;			// TRUE si on choisit des couleurs plus primaires (flashis ??)
	int Accel;				// valeur d'accélération (en fonction du spectrum) utilisé par les effets
	
	unsigned char WallPaperWindow[MAX_PATH];
	//unsigned char WallPaperWindowStyle[2];
	/**/unsigned long BackgroundColor;

//fonctions
	Space();							// initialisation
	~Space();							// destruction
	void GetIniConf(char *file);		// donne le nom complet du fichier .ini pour la configuration
	int InitDDraw(int NewMode);			// initialise un nouveau mode directdraw (les tableaux sont initialisés en même temps)
	int InitWnd(int NewMode);			// Initialise (crée si besoin) la fenêtre correspondante au nouveau mode
	void KillDDraw();					// détruit l'objet directdraw courant
	void AfficheMainWnd(RECT NewRect);	// Affiche la fenêtre aux nouvelles coordonées
	//RECT GetRectDock(int View);			// retourne les coordonnées du dock (correspondant au mode donnée, retourne { 0, 0, 0, 0} si View est invalide, fait un appel à CheckRes
	RECT SetRectDock(int View, bool init = FALSE);	// update les coordonnées de la fenêtre en mode dock (init est à un si update forcé)
	void KillEffects();					// Désalloue les effets
	void SetConf(char*);				// Lit la configuration (correspondant à la section) et initialise les données en fonction (utilise checkconf)
	void CheckConf();					// Contrôle la configuration
	UCHAR DetectBass();					// Retourne l'amplitude de la basse detectée (0 si pas de basse)
	void ChangeEffects();				// Change les effets courants selon la transformation
	char** GetPresets();				// retourne toutes les presets (fichiers *.spc)
	int init(char*);							// initialisation du plugin avec une conf donnée
	int ChangeViewMode(int NewMode);	// Passe d'un mode de vue à un autre
	int CalculRender();					// rendu sur les tableaux
	int SetRes(int NM, int RX, int RY);	// Définie la nouvelle résolution
	int DisplayRender();				// Affiche le rendu dans le la surface primary
	void ResizeMainWnd();				// Resize la fenêtre principale
	int Abandon(LONG error, char *s, int DDError = FALSE);	// Affiche le message d'erreur correspondant et retourne 1 (DDError à 1 s'il s'agit d'une erreur directdraw)
	int CheckRes(int NewMode, int w = 1);	// Adapte si nécessaire XRes et YRes selon le mode de résolution actuel et le mode d'affichage spécifié
	int InitViewMode(int NewMode);		// initialise un nouveau mode de vue
	HMENU MakeMenuConfig();				// construit et retourne le menu de config
	int RunMenuConfigSel(UINT Choix);   // Exécute la selection du menu config
	int DDActive();						// Retourne 1 (TRUE) si le directdraw est actif
	void SaveConf(char*);				// Sauvegarde la configuration sous une section donnée en paramètre
	void KillViewMode(int Mode);		// détruit le mode de vue spécifié (normalement le courant)
	// réajuste la taille de la zone cliente de la fenêtre (par défaut avec WidthWin et HeightWin)
	void SetSizeRectWnd(int width = 0, int height = 0, int style = WS_THICKFRAME | WS_SYSMENU | WS_BORDER | WS_CAPTION, int exstyle = WS_EX_TOOLWINDOW, int *dec = NULL);
	
	/**/
	int init_rotoblit(int nb_x, int nb_y);
	void rotoblit(USHORT* tabdata, USHORT* tabdataback, int Rotation, int zoom, int accel, int beat, int nb_x, int nb_y);
	void end_rotoblit();

	
	// transformation : un unique objet
	MathTrans Trans;										// diverses transformations

	/*********** INFO:
	Pour chaque nouvel effet, il ne faut pas oublier de les mettre dans le constructeur
	(Space) pour les randoms, de les initialiser (SetConf), de les Afficher (CalculRender),
	de les détruire (KillEffects), de les sauvegarder (SaveConf), de les mettre dans le menu
	(MakeMenuConfig) et de les traiter	dans le menu (RunMenuConfig): cela comprend le 'random',
	'add', 'all', 'remove', 'clear' et le 'load new config' pour le RunMenuConfig enfin il
	faut déclarer une variable 'random<nomeffet>' et le traiter dans (ChangeEffects)
	************/

	//effets
	CercleBassEff *CercleBass[MAX_CERCLEBASS_EFFECTS];		// cercles concentriques sur les basses 
	WaveFormEff *WaveForm[MAX_WAVEFORM_EFFECTS];			// le waveform classique
	CercleWaveEff *CercleWave[MAX_CERCLEWAVE_EFFECTS];		// le waveform en forme de cercle
	ConfettisEff *Confettis[MAX_CONFETTIS_EFFECTS];			// des confettis de couleurs unis 
	MathShapeEff *MathShape[MAX_MATHSHAPE_EFFECTS];
	RotaLineEff *RotaLine[MAX_ROTALINE_EFFECTS];			// ligne en rotation en dégradé depuis le centre vers l'extérieur
	LittleWaveEff *LittleWave[MAX_LITTLEWAVE_EFFECTS];		// petits waveforms se déplaçant sur l'écran
	LittleMathEff *LittleMath[MAX_LITTLEMATH_EFFECTS];		// petits formes mathématiques apparaissant au hasard sur l'écran
	GeoGraphEff *GeoGraph[MAX_GEOGRAPH_EFFECTS];			// Graphs géométriques
	
	/*
	StarFieldEff *StarField[MAX_STARFIELD_EFFECTS];
	*/


};


#endif