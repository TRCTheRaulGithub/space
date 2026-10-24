#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <malloc.h>
#include "log.h"

#include <windows.h>	/********/ //pour le messageBox dans point_taille

#define FLEUR 0
#define SPIRALE 1
#define ONDULATION 2
#define CERCLEVAGUE 3

#define PT 0
#define LINE 1
#define FILL 2

typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef unsigned int UINT;

#define NO 0
#define YES 1

#define FALSE 0
#define TRUE 1

//table cosinus
const int tabcos[512]= 
{ 65535, 65530, 65515, 65490, 65456, 65411, 65357, 65293, 65219, 65135, 65042, 64938, 
64825, 64702, 64570, 64427, 64275, 64114, 63942, 63761, 63571, 63370, 63161, 
62941, 62713, 62475, 62227, 61970, 61704, 61428, 61143, 60849, 60546, 60234, 
59913, 59582, 59243, 58895, 58537, 58171, 57797, 57413, 57021, 56620, 56211, 
55794, 55368, 54933, 54490, 54040, 53581, 53114, 52638, 52155, 51665, 51166, 
50660, 50146, 49624, 49095, 48559, 48015, 47464, 46906, 46341, 45769, 45190, 
44604, 44011, 43412, 42806, 42194, 41576, 40951, 40320, 39683, 39040, 38391, 
37737, 37076, 36410, 35739, 35062, 34380, 33693, 33001, 32303, 31601, 30894, 
30183, 29467, 28746, 28021, 27292, 26559, 25822, 25081, 24336, 23587, 22835, 
22080, 21321, 20559, 19794, 19026, 18255, 17481, 16705, 15926, 15144, 14361, 
13575, 12787, 11998, 11206, 10413, 9618, 8822, 8024, 7226, 6426, 5625, 
4823, 4021, 3218, 2415, 1611, 807, 3, -801, -1605, -2409, -3212, 
-4015, -4817, -5619, -6420, -7220, -8018, -8816, -9612, -10407, -11200, -11992, 
-12781, -13569, -14355, -15139, -15920, -16699, -17475, -18249, -19020, -19788, -20553, 
-21315, -22074, -22830, -23582, -24330, -25075, -25816, -26553, -27287, -28016, -28741, 
-29461, -30177, -30889, -31596, -32298, -32995, -33688, -34375, -35057, -35734, -36405, 
-37071, -37732, -38386, -39035, -39678, -40315, -40946, -41571, -42190, -42802, -43408, 
-44007, -44599, -45185, -45764, -46337, -46902, -47460, -48011, -48555, -49091, -49620, 
-50142, -50656, -51162, -51661, -52152, -52635, -53110, -53577, -54036, -54487, -54930, 
-55364, -55790, -56208, -56617, -57018, -57410, -57794, -58169, -58535, -58892, -59240, 
-59580, -59910, -60232, -60544, -60847, -61141, -61426, -61702, -61968, -62225, -62473, 
-62711, -62940, -63159, -63369, -63569, -63760, -63941, -64112, -64274, -64426, -64569, 
-64701, -64824, -64938, -65041, -65135, -65218, -65292, -65356, -65411, -65455, -65490, 
-65515, -65529, -65534, -65530, -65515, -65490, -65456, -65412, -65357, -65293, -65220, 
-65136, -65042, -64939, -64826, -64703, -64571, -64428, -64277, -64115, -63944, -63763, 
-63572, -63372, -63162, -62943, -62715, -62476, -62229, -61972, -61706, -61430, -61146, 
-60852, -60549, -60236, -59915, -59585, -59245, -58897, -58540, -58174, -57800, -57416, 
-57024, -56624, -56214, -55797, -55371, -54936, -54494, -54043, -53584, -53117, -52642, 
-52159, -51668, -51170, -50663, -50150, -49628, -49099, -48563, -48019, -47468, -46910, 
-46345, -45773, -45194, -44608, -44016, -43417, -42811, -42199, -41580, -40956, -40325, 
-39688, -39045, -38396, -37742, -37081, -36415, -35744, -35067, -34385, -33698, -33006, 
-32309, -31607, -30900, -30188, -29472, -28752, -28027, -27298, -26565, -25827, -25086, 
-24342, -23593, -22841, -22086, -21327, -20565, -19800, -19031, -18261, -17487, -16710, 
-15932, -15150, -14367, -13581, -12793, -12004, -11212, -10419, -9624, -8828, -8030, 
-7232, -6432, -5631, -4829, -4027, -3224, -2421, -1617, -813, -9, 795, 
1599, 2402, 3206, 4009, 4811, 5613, 6414, 7214, 8012, 8810, 9606, 
10401, 11194, 11986, 12775, 13563, 14349, 15133, 15914, 16693, 17469, 18243, 
19014, 19782, 20547, 21309, 22068, 22824, 23576, 24325, 25070, 25811, 26548, 
27281, 28010, 28735, 29456, 30172, 30884, 31591, 32293, 32990, 33683, 34370, 
35052, 35729, 36400, 37066, 37727, 38381, 39030, 39673, 40311, 40942, 41566, 
42185, 42797, 43403, 44002, 44595, 45181, 45760, 46332, 46897, 47456, 48007, 
48551, 49087, 49616, 50138, 50652, 51158, 51657, 52148, 52631, 53106, 53574, 
54033, 54484, 54927, 55361, 55787, 56205, 56614, 57015, 57407, 57791, 58166, 
58532, 58889, 59238, 59577, 59908, 60229, 60542, 60845, 61139, 61424, 61700, 
61966, 62223, 62471, 62709, 62938, 63157, 63367, 63568, 63758, 63940, 64111, 
64273, 64425, 64568, 64700, 64823, 64937, 65040, 65134, 65218, 65292, 65356, 
65410, 65455, 65490, 65514, 65529 };


#define cosinus(x)(int)  tabcos[(x&511)]
#define sinus(x)(int) tabcos[((x+128)&511)]
#define absolue(x)(int) ((x<0)?-x:x)
#define fabsolue(x)(float) ((x<0)?-x:x)
#define maximum(x, y)(int) ((x > y)?x:y)
#define fmaximum(x, y)(float) ((x > y)?x:y)
#define minimum(x, y)(int) ((x < y)?x:y)
#define fminimum(x, y)(float) ((x < y)?x:y)

//trace un point (nb_x est la longeur en x de ecran)
inline void point(unsigned short *ecran, int x, int y, unsigned short coul, int nb_x)
{
	__asm {
		push eax
		push edi
		push edx
		push ecx

		mov eax, y
		mov edi, ecran
		mul nb_x
		add eax, x
		add edi, eax
		add edi, eax
		mov cx, coul
		mov [edi], cx
		
		pop ecx
		pop edx
		pop edi
		pop eax
	}
}


inline void not_point(unsigned short *ecran, int x, int y, int nb_x, unsigned short mask)
{
	__asm {
		push eax
		push edi
		push edx
		push ebx

		xor ebx, ebx
		mov eax, y
		mov edi, ecran
		mul nb_x
		add eax, x
		add edi, eax
		add edi, eax
		add bx, [edi]
		jz suit
		xor bx, mask
		mov [edi], bx
suit:		
		pop ebx
		pop edx
		pop edi
		pop eax
	}
}


//trace une ligne verticale
inline void vertical_ligne(unsigned short *ecran, int x, int y1, int y2, unsigned short coul, int nb_x)
{

	__asm {
		pushad

		mov ebx, x
		mov ecx, y2
		mov eax, y1
		mov edi, ecran
		cmp ecx, eax
		ja y1supy2
		xchg eax, ecx
y1supy2:
		add edi, ebx
		sub ecx, eax
		add edi, ebx
		mul nb_x 
		inc ecx
		mov ebx, nb_x
		add edi, eax
		add ebx, ebx
		add edi, eax
		mov dx, coul
		ALIGN 16
lop:	mov [edi], dx
		add edi, ebx
		dec ecx
		jnz lop

		popad
	}
}


//trace une ligne horizontale (2 pixels à la fois)
inline void horizontal_ligne(unsigned short *ecran, int y, int x1, int x2, unsigned short coul, int nb_x)
//inline void horizontal_ligne(unsigned short *ecran, int y, int x1, int x2, unsigned int coul, int nb_x)
{
	__asm {
		pushad

		mov ebx, x1
		mov ecx, x2
		mov eax, y
		mov edi, ecran
		cmp ecx, ebx
		ja x2supx1
		xchg ebx, ecx
x2supx1:
		lea edi, [edi + ebx * 2]
		mul nb_x 
		sub ecx, ebx
		lea edi, [edi + eax * 2]
		inc ecx
		mov ax, coul
		mov bx, ax
		shl eax, 16
		mov ax,bx
/**
		mov eax, coul
		mov ebx, eax
		shl eax, 16
		or eax, ebx
/**/
		shr ecx,1
		jnc pair
		mov [edi], ax
		add edi, 2
		test ecx,ecx
		jz fin
		ALIGN 16
pair:
		mov [edi], eax
		add edi, 4
		dec ecx
		jnz pair

fin:
		popad
	}
}


inline void point_taille(unsigned short *screen, int x, int y, int taille, unsigned short coul, int nb_x, int nb_y) {
///****/return;	
	// réajustement coordonnées pour qu'elles ne dépassent l'écran
	//x = maximum(x, 0); x = minimum(nb_x - 1, x);
	//y = maximum(y, 0); y = minimum(nb_y - 1, y);
	if( (x<0)||(x>=nb_x)||(y<0)||(y>=nb_y) ) return;

	int j;

	//taille = maximum(1, taille);
	if( (x + taille) > nb_x ) taille = nb_x - x;
	if( (y + taille) > nb_y ) taille = nb_y - y;

	switch(taille) {
	case 1: point(screen, x, y, coul, nb_x); break;
	case 2:
		point(screen, x, y, coul, nb_x);
		point(screen, x, y + 1, coul, nb_x);
		point(screen, x + 1, y, coul, nb_x);
		point(screen, x + 1, y + 1, coul, nb_x);
		break;
	default:
		for(j = 0; j < taille; j++)
			horizontal_ligne(screen, y + j, x, x + taille - 1, coul, nb_x);
	case 0:;
	}
}


//trace une ligne
void ligne(unsigned short *ecran, int x1, int y1, int x2, int y2, unsigned short coul, int width, int height);

//trace une ligne transparente (16 bits)
void ligne_trans(unsigned short *ecran, int x1, int y1, int x2, int y2, unsigned short coul, int width, int height, int mode555);

// trace un graphe sur une ligne basé sur le data du wave
void ligne_graphe(unsigned short *screen, int x, int y1, int x2, int y2, unsigned short coul, unsigned int width, unsigned int height, unsigned char *wavedata, int type);

// trace un graphe mathématique dont le style est passé en param
void SuperGraphe(unsigned short *screen, int x, int y, int diam, int taillept, int nbpoint, int ang, int param, unsigned short coul, unsigned int nb_x, unsigned int nb_y, int style);

#endif  // _GRAPH_H


