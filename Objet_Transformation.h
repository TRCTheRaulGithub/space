#ifndef _TRANS_H_
#define _TRANS_H_

//#pragma vector

#include <windows.h>
#include "objet_effet.h"		// pour connaitre éventuellement les effets en cours
#include "graph.h"
#include <math.h>
#include <assert.h>


#define NOMOVE 0		// définition des différentes transformations
#define ZOOMIN 1
#define ROTATION 2
#define ROTOZOOMIN 3
#define PETALES 4
#define ZARB 5
#define TUNNEL 6
#define CYCLONE 7
//#define ROTOBRICK 8
#define HORIZOOM 9
#define VERTIZOOM 10
#define RAYSUN 11
//#define WATER 12
#define BIROTOZOOM 13
#define BIZOOM 14
#define ROTOSUN 15
#define BOX 16
#define WATERZOOM 8
#define GALAXY 12
#define ROTOTUNNEL 17
#define VERTIPERSP 18
#define HORIPERSP 19
#define CYCLOFLOWER 20
#define ILLUSION 21
#define SWIRLOUT 22
#define IMPLOSE 23
#define ZOOMOUT 24
#define SHADOWIN 25
#define ROTOZOOMOUT 26

//les shifts
#define NOSHIFT 0
#define BASSSHIFT 1
#define TOPLEFTSHIFT 2
#define TOPSHIFT 3
#define TOPRIGHTSHIFT 4
#define RIGHTSHIFT 5
#define BOTTOMRIGHTSHIFT 6
#define BOTTOMSHIFT 7
#define BOTTOMLEFTSHIFT 8
#define LEFTSHIFT 9

#define MAX_WIDTH 1024
#define MAX_HEIGHT 768

typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef unsigned int UINT;

//copie de mémoire (à 8 pixel près) en octets

/*
inline void FastMemCopy(void* dest, void* source, UINT nb, int supportMMX) {
	if(supportMMX)
	__asm {
		push eax
		push ecx
		push esi
		push edi

		mov ecx, nb  
		mov esi, source
		sub ecx, 16
		mov edi, dest
		ALIGN 16

boucleclr:	
		movq mm0, [esi + ecx]
		movq mm1, [esi + ecx + 8]
		movq [edi + ecx], mm0
		movq [edi + ecx + 8], mm1

		sub ecx, 16
		jns boucleclr

		emms

		pop edi
		pop esi
		pop ecx
		pop eax
	}
	else
	__asm {
		pushad

		mov ecx, nb  
		mov esi, source
		mov edi, dest
		sub ecx, 16
		ALIGN 16

boucleclr2:	
		mov eax, dword ptr [esi + ecx]
		mov ebx, dword ptr [esi + 4 + ecx]
		mov dword ptr [edi + ecx], eax
		mov dword ptr [edi + 4 + ecx], ebx
		mov eax, dword ptr [esi + 8 + ecx]
		mov ebx, dword ptr [esi + 12 + ecx]
		mov dword ptr [edi + 8 + ecx], eax
		mov dword ptr [edi + 12 + ecx], ebx
		
		sub ecx, 16
		jns boucleclr2

		popad

	}
}
*/

//copie de mémoire inversée (copie depuis source + nb vers source) en octets
inline void MemCopyInv(void *dest, void *source, UINT nb) {
	__asm {
		pushad

		mov esi, [source]
		mov ecx, nb
		mov edi, [dest]
		sub ecx, 2
		xor edx, edx
		
boucle:
		add edx, 2
		mov ax, [esi + ecx]
		sub ecx, 2
		mov [edi + edx - 2], ax
		jns boucle

		popad
	}
}

// les différents renders
void FadeOut(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int supportMMX);
void Quick(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int supportMMX);
void Normal(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix = 0);
void Slow(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix = 0);
void FastWater(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix = 0);
void NormalWater(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix = 0);


//les transformations mathématiques
struct MathTrans {
	static int enabled;				// TRUE si les transformations sont activées
	static UINT type;				// correspond au type de la transformation courante
	static int newtrans;			// TRUE si une nouvelle transformation doit être affichée
	UINT nexttype;					// correspond au type de la transformation courante
	int random;						// TRUE si les transformations sont choisies au hasard
	int noise;						// TRUE si on ajoute un noise sur les transformations
	int shift;						// indique le "shift" appliqué
	int slide;						// TRUE si les transformations apparaissent en slide
	//int fast;						// TRUE si transformations rapide (+ pixelisées)
	int time2;						// compteur pour le slide
	int* sourcecoordtable;			// tableau contenant les coordonnées sources de l'image (transformation)
	int n_x, n_y;					// dimension tableau precalcul distance et angle
	UINT old_nb_x, old_nb_y;		// par rapport à la taille écran
	UINT old_nb_x2, old_nb_y2;		// par rapport à la taille du tableau de transformation (mode moving)
	int* presourcecoordtable;		// second tableau précalculé
	float* distance_angle[2];		// tableau de précalcul des distances et angles (plus de 6 mégas !!!)
	UINT taille;					// ces variables permettent de ne pas répéter les calculs
	UINT taille2;					// les variables se terminant par '2' se rapportent à la transformation "moving"
	/**/
	int deplacement_x, deplacement_y;
	int delta_nbx, delta_nby;		// permet au cercle wave de se centrer sur le tunnel
	/**/
	int center_x, center_y;			
	int center_x2, center_y2;		
	int old_pos, pos;		;		// positions dans le précalcul dde la transformation
	float coef_x, coef_y;
	float distancemax;
	float coef_x2, coef_y2;
	float distancemax2;
	MathTrans();
	~MathTrans();
	void Recalculate(int sliding = FALSE, int num = 0, int pre = FALSE);	// calcul d'une nouvelle transformation (en sliding, pre ou normal)
	//void Render(USHORT **ecran_courant, USHORT **ecran_precedent, UINT nb_x, UINT nb_y, int supportMMX, int accel, int calc_x, int calc_y);
	void Render(USHORT **ecran_courant, USHORT **ecran_precedent, UINT nb_x, UINT nb_y, int supportMMX, int accel, int calc_x, int calc_y, bool bassdetected);
};

// les différents mirroirs
void Horizontal(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int pitch);
void Vertical(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int pitch);
void Double(USHORT *screen, USHORT *screenback, UINT nb_x, UINT nb_y, int pitch);


#endif