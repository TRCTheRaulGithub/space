#include "objet_transformation.h"


//le fade
void FadeOut(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int supportMMX)       
{
	if(supportMMX)
	if(!(mode555))
		_asm
		{
			pushad

			mov eax, nb_y
			mov esi, screen
			mul nb_x
			mov ebp, eax 
			shr ebp, 2 // ebp = nombre de pixel à rendre / 4 (car 4 pixels d'un coup)

			pxor mm1, mm1
			pxor mm3, mm3
			pxor mm4, mm4
			pxor mm5, mm5
			pxor mm6, mm6
			pxor mm7, mm7
			
			mov ebx, 0x08000800	//masque de décrementation composante rouge
			movd mm1, ebx
			
			por mm3, mm1
			psllq mm3, 32
			por mm3, mm1
			
			mov ebx, 0x00010001	//masque de décrementation composante bleue
			movd mm1, ebx
			
			por mm4, mm1
			psllq mm4, 32
			por mm4, mm1

			mov ebx, 0xF800F800	//masque de récuperation composante rouge
			movd mm1, ebx

			por mm5, mm1
			psllq mm5, 32
			por mm5, mm1

			mov ebx, 0x07C007C0	//masque de récuperation composante verte
			movd mm1, ebx

			por mm6, mm1
			psllq mm6, 32
			por mm6, mm1

			mov ebx, 0x001F001F	//masque de récuperation composante bleue
			movd mm1, ebx

			por mm7, mm1
			psllq mm7, 32
			por mm7, mm1

			ALIGN 16
fadeout22:
			
			movq mm0, [esi]
			movq mm1, mm0
			movq mm2, mm0
			pand mm0, mm5	// composante rouge
			pand mm1, mm6	// composante verte
			pand mm2, mm7	// et la composante bleue
			psllw mm1, 5	// décalage de 5 pour avoir le même alignement des bits que la composante rouge
			psubusw mm0, mm3	// on soustrait 1 à chaque composante avec saturation
			psubusw mm1, mm3
			psubusw mm2, mm4
			psrlw mm1, 5	// on restore la composante verte
			paddw mm0, mm2	// et on fait la somme des composante 
			add esi,8
			paddw mm0, mm1
		
			dec ebp
			movq [esi - 8], mm0
			jnz fadeout22

			emms
			
			popad
	}
	else
		_asm
		{
			pushad

			mov eax, nb_y
			mov esi, screen
			mul nb_x
			mov ebp, eax 
			shr ebp, 2 // ebp = nombre de pixel à rendre / 4 (car 4 pixels d'un coup)
			pxor mm1, mm1
			pxor mm3, mm3
			pxor mm4, mm4
			pxor mm5, mm5
			pxor mm6, mm6
			pxor mm7, mm7
			
			mov ebx, 0x04000400	//masque de décrementation composante rouge
			movd mm1, ebx
			
			por mm3, mm1
			psllq mm3, 32
			por mm3, mm1
			
			mov ebx, 0x00010001	//masque de décrementation composante bleue
			movd mm1, ebx
			
			por mm4, mm1
			psllq mm4, 32
			por mm4, mm1

			mov ebx, 0x7C007C00	//masque de récuperation composante rouge
			movd mm1, ebx

			por mm5, mm1
			psllq mm5, 32
			por mm5, mm1

			mov ebx, 0x03E003E0	//masque de récuperation composante verte
			movd mm1, ebx

			por mm6, mm1
			psllq mm6, 32
			por mm6, mm1

			mov ebx, 0x001F001F	//masque de récuperation composante bleue
			movd mm1, ebx

			por mm7, mm1
			psllq mm7, 32
			por mm7, mm1

			ALIGN 16
fadeout32:
			
			movq mm0, [esi]
			movq mm1, mm0
			movq mm2, mm0
			pand mm0, mm5	// composante rouge
			pand mm1, mm6	// composante verte
			pand mm2, mm7	// et la composante bleue
			psllw mm1, 5	// décalage de 5 pour avoir le même alignement des bits que la composante rouge
			psubusw mm0, mm3	// on soustrait 1 à chaque composante avec saturation
			psubusw mm1, mm3
			psubusw mm2, mm4
			psrlw mm1, 5	// on restore la composante verte
			paddw mm0, mm2	// et on fait la somme des composante 
			add esi,8
			paddw mm0, mm1

			dec ebp
			movq [esi - 8], mm0
			jnz fadeout32

			emms
			
			popad
	}
	else
	if(!(mode555))
		_asm
		{
			pushad

			mov eax, nb_y
			mov esi, screen
			mul nb_x
			mov ebp, eax  // ebp = nombre de pixel à rendre

			xor eax, eax
			xor ebx, ebx

			ALIGN 16
fadeout2:

			mov ax, [esi]

			mov bx, ax
			and ax, 0xF81F
			jz bleu_0_1	
			test ah, ah
			jz rouge_0_1
			sub ah, 0x8
			test al, al
			jz bleu_0_1
rouge_0_1:
			dec al
bleu_0_1:
			and bx, 0x7C0
			jz vert_0_1
			sub bx, 0x40
vert_0_1:
			add esi, 2
			or ax, bx
		

			dec ebp
			mov [esi - 2], ax
			jnz fadeout2

			popad
	}
	else
		_asm
		{
			pushad

			mov eax, nb_y
			mov esi, screen
			mul nb_x
			mov ecx, eax  

			xor eax, eax
			xor ebx, ebx

			ALIGN 16
fadeout3:
			mov ax, [esi]

			mov bx, ax
			and ax, 0x7C1F
			jz others_0_12
			test ah, ah
			jz rouge_0_12
			sub ax, 0x400
rouge_0_12:
			dec ax
others_0_12:
			and bx, 0x3E0
			jz vert_0_12
			sub bx, 0x40
vert_0_12:
			add esi, 2
			or ax, bx
			
			dec ecx
			mov [esi - 2], ax
			jnz fadeout3

			popad
	}
}



//le blur ...
void Quick(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int supportMMX)
{
if(supportMMX)
if(mode555)
__asm {
	pushad
	
	mov esi, screen
	mov eax, nb_y
	xor edi, edi
	mul nb_x
	xor edx, edx
	mov ecx, eax
	shr ecx, 2	// nombre de pix 16 bits à rendre en blur / 4 (car 4 pixs rendus d'un coup)

	pxor mm1, mm1
	pxor mm6, mm6
	pxor mm7, mm7
			
	mov ebx, 0x7C1F7C1F	//masque de récuperation composante rouge et bleue
	movd mm1, ebx

	por mm6, mm1
	psllq mm6, 32
	por mm6, mm1

	mov ebx, 0x03E003E0	//masque de récuperation composante verte
	movd mm1, ebx

	por mm7, mm1
	psllq mm7, 32
	por mm7, mm1


	ALIGN 16
blurmore3:


	movq mm0, [esi]
	movq mm1, mm0
	movq mm2, [esi + 2]
	movq mm3, mm2

	pand mm0, mm6
	pand mm1, mm7
	pand mm2, mm6
	pand mm3, mm7

	psrlw mm0, 1
	psrlw mm1, 1
	psrlw mm2, 1
	psrlw mm3, 1

	paddusw mm0, mm2
	paddusw mm1, mm3

	pand mm0, mm6
	pand mm1, mm7

	add esi, 8
	paddw mm0, mm1

	dec ecx

	movq [esi - 8], mm0
	jnz blurmore3

	emms
	
	popad
	}
else
__asm {
	pushad
	
	mov esi, screen
	mov eax, nb_y
	xor edi, edi
	mul nb_x
	xor edx, edx
	mov ecx, eax
	shr ecx, 2	// nombre de pix 16 bits à rendre en blur / 4 (car 4 pixs rendus d'un coup)

	pxor mm1, mm1
	pxor mm6, mm6
	pxor mm7, mm7
			
	mov ebx, 0xF81FF81F	//masque de récuperation composante rouge et bleue
	movd mm1, ebx

	por mm6, mm1
	psllq mm6, 32
	por mm6, mm1

	mov ebx, 0x07C007C0	//masque de récuperation composante verte
	movd mm1, ebx

	por mm7, mm1
	psllq mm7, 32
	por mm7, mm1


	ALIGN 16
blurmore4:


	movq mm0, [esi]		// les 4 premiers pixels
	movq mm1, mm0
	movq mm2, [esi + 2]	// les 4 pixels (se situant un pixel plus loin)
	movq mm3, mm2

	pand mm0, mm6		// les composantes bleues et rouges
	pand mm1, mm7		// les composantes vertes
	pand mm2, mm6
	pand mm3, mm7

	psrlw mm0, 1
	psrlw mm1, 1
	psrlw mm2, 1
	psrlw mm3, 1

	paddusw mm0, mm2
	paddusw mm1, mm3

	pand mm0, mm6
	pand mm1, mm7

	add esi, 8
	paddw mm0, mm1

	dec ecx

	movq [esi - 8], mm0
	jnz blurmore4

	emms
	
	popad
	}
else
if(mode555)
__asm {
	pushad
	
	mov esi, screen
	mov eax, nb_y
	xor edi, edi
	mul nb_x
	xor edx, edx
	mov ecx, eax		; nombre de pix 16 bits à rendre en blur

	ALIGN 16
blurmore:
	mov ax, [esi]
	add esi, 2
	shr ax, 1
	mov bx, ax
	and ax, 0x3C0F
	and bx, 0x01E0
	add di, ax
	add dx, bx
	add dx, di
	dec ecx
	mov [esi - 2], dx
	mov di, ax
	mov dx, bx
	jnz blurmore

	popad
	}
else
__asm {
	pushad
	
	mov esi, screen
	mov eax, nb_y
	xor edi, edi
	mul nb_x
	xor edx, edx
	mov ecx, eax		; nombre de pix 16 bits à rendre en blur

	ALIGN 16
blurmore2:
	mov ax, [esi]
	add esi, 2
	shr ax, 1
	mov bx, ax
	and ax, 0x780F
	and bx, 0x03E0
	add di, ax
	add dx, bx
	add dx, di
	dec ecx
	mov [esi - 2], dx
	mov di, ax
	mov dx, bx
	jnz blurmore2

	popad
	}
}


void Normal(USHORT* ecran_courant, USHORT* ecran_precedent, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix) {
	if(supportMMX)
	if(!(mode555))
		__asm {
			pushad

			mov eax, nb_y
			mov esi, ecran_precedent
			sub eax, 2
			mov ecx, nb_x
			mov edi, ecran_courant
			mul ecx
			sub eax, 2
			mov edx, ecx
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero
			mov eax, nb_pix
nb_pix_zero:

			mov ecx, eax  // ecx contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			pxor mm6, mm6
			pxor mm7, mm7
			pxor mm0, mm0
			pxor mm1, mm1
			
			mov ebx, 0xF81F
			movd mm0, ebx
			mov ebx, 0x7C0
			movd mm1, ebx

			por mm6, mm0
			psllq mm6, 32
			por mm6, mm0

			por mm7, mm1
			psllq mm7, 32
			por mm7, mm1
			
			ALIGN 16
boucleblur:

			movq mm0, [esi]
			movq mm2, [esi + 2*edx]
			movq mm1, mm0
			movq mm3, mm2
			pand mm0, mm6	// on garde les composantes RB et RB des pixels 1 et 3 (dans les 48 bits de poids faibles) du haut
			pand mm1, mm7	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts) du haut
			pand mm2, mm6	// on garde les composantes RB et RB des pixels 1 et 3 (dans les 48 bits de poids faibles) du bas
			pand mm3, mm7	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts) du bas

			paddd mm0, mm2	//on additionne les blocs de composantes (16 bits) entre eux
			paddd mm1, mm3	//normalement on n'a pas de saturation ...

			movd eax, mm0	//on charge les premières composantes RB dans eax (pixels gauches)
			movd ebx, mm1	//on charge les premières composantes V dans ebx (pixels gauches)
			psrlq mm0, 32
			psrlq mm1, 32  
			
			movd ebp, mm0
			add edi, 2
			add eax, ebp	//et on ajoute les autres composantes RB
			add esi, 2		// on passe au pixel suivant
			movd ebp, mm1
			shr eax, 2		// moyenne des 4
			add ebx, ebp	//et on ajoute les autres composantes V
			
			and eax, 0xF81F
			
			//pas pairable :(
			shr ebx, 2
			and ebx, 0x7C0
			or eax, ebx
			
			dec ecx
			mov [edi + edx], ax  
			jnz boucleblur
			
			emms

			popad
	}
	else
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero2
			mov eax, nb_pix
nb_pix_zero2:

			mov ecx, eax  // ecx contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			pxor mm6, mm6
			pxor mm7, mm7
			pxor mm0, mm0
			pxor mm1, mm1
			
			mov ebx, 0x7C1F
			movd mm0, ebx
			mov ebx, 0x3E0
			movd mm1, ebx

			por mm6, mm0
			psllq mm6, 32
			por mm6, mm0

			por mm7, mm1
			psllq mm7, 32
			por mm7, mm1
			
			ALIGN 16
boucleblur2:

			movq mm0, [esi]
			movq mm1, mm0
			pand mm0, mm6	// on garde les composantes RB et RB des pixels 1 et 3 (dans les 48 bits de poids faibles) du haut
			pand mm1, mm7	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts) du haut
			
			movq mm2, [esi + 2*edx]
			movq mm3, mm2
			pand mm2, mm6	// on garde les composantes RB et RB des pixels 1 et 3 (dans les 48 bits de poids faibles) du bas
			pand mm3, mm7	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts) du bas

			paddd mm0, mm2	//on additionne les blocs de composantes (16 bits) entre eux
			paddd mm1, mm3	//normalement on n'a pas de saturation ...

			movd eax, mm0	//on charge les premières composantes RB dans eax (pixels gauches)
			movd ebx, mm1	//on charge les premières composantes V dans ebx (pixels gauches)
			psrlq mm0, 32
			psrlq mm1, 32  

			movd ebp, mm0
			add edi, 2
			add eax, ebp	//et on ajoute les autres composantes RB
			add esi, 2  // on passe au pixel suivant
			movd ebp, mm1
			shr eax, 2  // moyenne des 4
			add ebx, ebp	//et on ajoute les autres composantes V
			and eax, 0x7C1F
			
			//pas pairable :(
			shr ebx, 2
			and ebx, 0x3E0
			or eax, ebx
			
			dec ecx
			mov [edi + edx], ax  
			jnz boucleblur2
		
			emms

			popad
	}
	else
	if(!(mode555))
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub esp,4  // on s'alloue un emplacement pile
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero3
			mov eax, nb_pix
nb_pix_zero3:

			mov [esp], eax  // [esp] contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			xor eax, eax
			xor ecx, ecx
			xor ebp, ebp
			xor ebx, ebx

			ALIGN 16
boucleblur3:
			mov ax, [esi]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouge et bleue (en mode 555 c 0x7C1F)
			and bx, 0x7C0  // on isole les 5 bits de poids fort de la composante verte (0x3C0 en mode 555)
			mov cx, ax
			mov bp, bx

			mov ax, [esi + 4]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx]
			mov bx, ax
			and ax, 0xF81F
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 4]
			mov bx, ax
			and ax, 0xF81F
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			shr ecx, 2  // moyenne des 4
			shr ebp, 2

			and ecx, 0xF81F
			and ebp, 0x7C0

			add edi, 2
			or ecx, ebp
			add esi, 2  // on passe au pixel suivant

			dec dword ptr [esp]  //mettre le dec juste avant le jnz si ça foire
			mov [edi + edx], cx  
			jnz boucleblur3
		
			add esp, 4  // on libère l'emplacement pile
			popad
	}
	else
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub esp,4  // on s'alloue un emplacement pile
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero4
			mov eax, nb_pix
nb_pix_zero4:

			mov [esp], eax  // [esp] contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			xor eax, eax
			xor ebx, ebx
			xor ecx, ecx
			xor ebp, ebp

			ALIGN 16
boucleblur4:
			mov ax, [esi]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			mov cx, ax
			mov bp, bx

			mov ax, [esi + 4]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 4]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			shr ecx, 2  
			shr ebp, 2

			and ecx, 0x7C1F
			and ebp, 0x3C0

			add edi, 2
			or ecx, ebp
			add esi, 2  
			

			dec dword ptr [esp]
			mov [edi + edx], cx  
			jnz boucleblur4
		
			add esp, 4 
			popad
	}

	if(!nb_pix) {
		ZeroMemory(ecran_courant + (nb_x * (nb_y - 2)) + (nb_x - 1), (nb_x + 1) * sizeof(short));
		ZeroMemory(ecran_courant, (nb_x + 1) * sizeof(short));
	}
}


void Slow(USHORT *ecran_courant, USHORT *ecran_precedent, UINT nb_x, UINT nb_y, int mode555, int supportMMX, int nb_pix)
{
	if(supportMMX)
	if(!(mode555))
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero
			mov eax, nb_pix
nb_pix_zero:

			mov ecx, eax  // ecx contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2
			
			pxor mm0, mm0
			pxor mm1, mm1
			pxor mm2, mm2
			pxor mm3, mm3
			pxor mm4, mm4
			pxor mm5, mm5
			pxor mm6, mm6
			pxor mm7, mm7

			mov ebx, 0x07C0F81F
			movd mm0, ebx
			movd mm3, ebx
			mov ebx, 0x07C00000
			movd mm1, ebx

			mov ebx, 0x0000F81F
			movd mm2, ebx
		
			por mm4, mm0
			psllq mm4, 32
			por mm4, mm1

			por mm5, mm2
			psllq mm5, 32
			por mm5, mm3

			pxor mm0, mm0
		
			por mm6, mm1
			psllq mm6, 32
			por mm6, mm1

			por mm7, mm2
			psllq mm7, 32
			por mm7, mm2
			
			ALIGN 16

boucleblur:
			movq mm0, [esi]
			movq mm1, mm0
			psllq mm0,16
			pand mm1, mm5	// on garde les composantes RB, V et RB des 3 pixels (dans les 48 bits de poids faibles)
			pand mm0, mm4	// on garde les composantes V , RB et V des 3 pixels (dans les 48 bits de poids forts)
			
			movq mm2, [esi + edx]
			movq mm3, mm2
			psllq mm2,16
			pand mm3, mm7	// on garde les composantes RB, RB des pixels 1 et 3 (dans les 48 bits de poids faibles)
			pand mm2, mm6	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts)

			paddd mm1, mm3	// normalement on n'a pas de saturation ...
			paddd mm0, mm2	// on additionne les blocs de composantes (16 bits) entre eux

			movq mm2, [esi + edx * 2]
			movq mm3, mm2
			psllq mm2,16
			pand mm3, mm5	// on garde les composantes RB, V et RB des 3 pixels
			pand mm2, mm4	// on garde les composantes V , RB et V des 3 pixels
			
			paddd mm1, mm3
			paddd mm0, mm2	// on additionne les blocs de composantes (16 bits) entre eux

			psrlq mm0, 16

			movq mm3, mm1
			movq mm2, mm0
			psrlq mm3, 16
			psrlq mm2, 16
			paddd mm0, mm3
			paddd mm1, mm2
			psrlq mm3, 16
			psrlq mm2, 16
			paddd mm1, mm3
			paddd mm0, mm2
			psrlq mm1, 3
			psrlq mm0, 3

			movd eax, mm1
			movd ebx, mm0
			and eax, 0xF81F
			and ebx, 0x7C0
			add edi, 2
			add esi, 2		// on passe au pixel suivant
			add eax, ebx	// on mélange les composantes
			
			dec ecx
			mov [edi + edx], ax

			jnz boucleblur
		
			emms
			popad
		}
	else
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero2
			mov eax, nb_pix
nb_pix_zero2:

			mov ecx, eax  // ecx contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2
			
			pxor mm0, mm0
			pxor mm1, mm1
			pxor mm2, mm2
			pxor mm3, mm3
			pxor mm4, mm4
			pxor mm5, mm5
			pxor mm6, mm6
			pxor mm7, mm7

			mov ebx, 0x03E07C1F
			movd mm0, ebx
			movd mm3, ebx
			mov ebx, 0x03E00000
			movd mm1, ebx

			mov ebx, 0x00007C1F
			movd mm2, ebx
		
			por mm4, mm0
			psllq mm4, 32
			por mm4, mm1

			por mm5, mm2
			psllq mm5, 32
			por mm5, mm3

			pxor mm0, mm0
		
			por mm6, mm1
			psllq mm6, 32
			por mm6, mm1

			por mm7, mm2
			psllq mm7, 32
			por mm7, mm2
			
			ALIGN 16

boucleblur2:
			movq mm0, [esi]
			movq mm1, mm0
			psllq mm0,16
			pand mm1, mm5	// on garde les composantes RB, V et RB des 3 pixels (dans les 48 bits de poids faibles)
			pand mm0, mm4	// on garde les composantes V , RB et V des 3 pixels (dans les 48 bits de poids forts)
			
			movq mm2, [esi + edx]
			movq mm3, mm2
			psllq mm2,16
			pand mm3, mm7	// on garde les composantes RB, RB des pixels 1 et 3 (dans les 48 bits de poids faibles)
			pand mm2, mm6	// on garde les composantes V et V des pixels 1 et 3 (dans les 48 bits de poids forts)

			paddd mm1, mm3	// normalement on n'a pas de saturation ...
			paddd mm0, mm2	// on additionne les blocs de composantes (16 bits) entre eux

			movq mm2, [esi + edx * 2]
			movq mm3, mm2
			psllq mm2,16
			pand mm3, mm5	// on garde les composantes RB, V et RB des 3 pixels
			pand mm2, mm4	// on garde les composantes V , RB et V des 3 pixels
			
			paddd mm1, mm3
			paddd mm0, mm2	// on additionne les blocs de composantes (16 bits) entre eux

			psrlq mm0, 16

			movq mm3, mm1
			movq mm2, mm0
			psrlq mm3, 16
			psrlq mm2, 16
			paddd mm0, mm3
			paddd mm1, mm2
			psrlq mm3, 16
			psrlq mm2, 16
			paddd mm1, mm3
			paddd mm0, mm2
			psrlq mm1, 3
			psrlq mm0, 3

			movd eax, mm1
			movd ebx, mm0
			and eax, 0x7C1F
			and ebx, 0x3E0
			add edi, 2
			add esi, 2		// on passe au pixel suivant
			add eax, ebx	// on mélange les composantes
			
			
			dec ecx
			mov [edi + edx], ax

			jnz boucleblur2
		
			emms
			popad
		}
	else
	if(!(mode555))
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub esp,4  // on s'alloue un emplacement pile
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero3
			mov eax, nb_pix
nb_pix_zero3:

			mov [esp], eax  // [esp] contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			xor eax, eax
			xor ebx, ebx
			xor ecx, ecx
			xor ebp, ebp

			ALIGN 16
boucleblur23:
			mov ax, [esi]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouge et bleue (en mode 555 c 0x7C1F)
			and bx, 0x7C0  // on isole les 5 bits de poids fort de la composante verte (0x3C0 en mode 555)
			mov cx, ax
			mov bp, bx

			mov ax, [esi + 2]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 4]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + edx]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + edx + 4]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx]
			mov bx, ax
			and ax, 0xF81F
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 2]
			mov bx, ax
			and ax, 0xF81F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 4]
			mov bx, ax
			and ax, 0xF81F
			and bx, 0x7C0 
			add ecx, eax
			add ebp, ebx

			shr ecx, 3  // moyenne des 8
			shr ebp, 3

			and ecx, 0xF81F
			and ebp, 0x7C0

			add edi, 2
			or ecx, ebp
			add esi, 2  // on passe au pixel suivant
			

			dec dword ptr [esp]  //mettre le dec juste avant le jnz si ça foire
			mov [edi + edx], cx  
			jnz boucleblur23
		
			add esp, 4  // on libère l'emplacement pile
			popad
	}
	else
		__asm {
			pushad

			mov esi, ecran_precedent
			mov edi, ecran_courant
			mov ecx, nb_x
			mov eax, nb_y
			sub esp,4  // on s'alloue un emplacement pile
			sub eax, 2
			mul ecx
			mov edx, ecx
			sub eax, 2
			test nb_pix, 0xFFFFFFFF
			jz nb_pix_zero4
			mov eax, nb_pix
nb_pix_zero4:

			mov [esp], eax  // [esp] contient le nombre de pixel à rendre
			add edx, edx  // edx contient le nb de pixel / ligne * 2

			xor eax, eax
			xor ebx, ebx
			xor ecx, ecx
			xor ebp, ebp

			ALIGN 16
boucleblur22:
			mov ax, [esi]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			mov cx, ax
			mov bp, bx

			mov ax, [esi + 2]
			mov bx, ax
			and ax, 0x7C1F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx


			mov ax, [esi + 4]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + edx]
			mov bx, ax
			and ax, 0x7C1F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + edx + 4]
			mov bx, ax
			and ax, 0x7C1F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 2]
			mov bx, ax
			and ax, 0x7C1F // on isole les composantes rouges et bleu (en mode 555 c 0x7C1F)
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			mov ax, [esi + 2 * edx + 4]
			mov bx, ax
			and ax, 0x7C1F
			and bx, 0x3C0 
			add ecx, eax
			add ebp, ebx

			shr ecx, 3  
			shr ebp, 3

			and ecx, 0x7C1F
			and ebp, 0x3C0

			add edi, 2
			or ecx, ebp
			add esi, 2  
			

			dec dword ptr [esp]
			mov [edi + edx], cx  
			jnz boucleblur22
		
			add esp, 4 
			popad
	}

	if(!nb_pix) {
		ZeroMemory(ecran_courant + (nb_x * (nb_y - 2)) + (nb_x - 1), (nb_x + 1) * sizeof(short));
		ZeroMemory(ecran_courant, (nb_x + 1) * sizeof(short));
	}
}



// Transformations
int MathTrans::enabled = FALSE;
int MathTrans::newtrans = TRUE;
UINT MathTrans::type = NOMOVE;

MathTrans::MathTrans() : old_nb_x(0), old_nb_y(0), sourcecoordtable(NULL), presourcecoordtable(NULL), /**/n_x(MAX_WIDTH * 1.1), n_y(MAX_HEIGHT * 1.1) {
	float dist;

	n_x = n_x + (n_x)%2; n_y = n_y + (n_y)%2;

	distance_angle[0] = (float*)malloc(n_x*n_y*sizeof(float));
	distance_angle[1] = (float*)malloc(n_x*n_y*sizeof(float));

	for(int j = -(n_y>>1); j < (n_y>>1); j++) {
		for(int i = -(n_x>>1); i < (n_x>>1); i++) {
			dist = sqrt(i*i + j*j);  // distance de notre point / centre
			distance_angle[0][(i+(n_x>>1)) + (j+(n_y>>1))*n_x] = dist;
			if(dist != 0) distance_angle[1][(i+(n_x>>1)) + (j+(n_y>>1))*n_x] = (j<0)?-acos(i/dist):acos(i/dist);	// angle de notre point / centre 
			else distance_angle[1][(i+(n_x>>1)) + (j+(n_y>>1))*n_x] = 0;
			//else *((__m128 *) &MathTrans.distance_angle[1][(i+(n_x>>1)) + (j+(n_y>>1))*n_x]) = (__m128) _mm_set_ps1(0.00000000e+00);

		}
	}
}

MathTrans::~MathTrans() {
	if(sourcecoordtable) free(sourcecoordtable);
	if(presourcecoordtable) free(presourcecoordtable);
	free(distance_angle[0]); free(distance_angle[1]); 
}


//void MathTrans::Render(USHORT **ecran_courant, USHORT **ecran_precedent, UINT nb_x, UINT nb_y, int supportMMX, int move, int calc_x, int calc_y)
void MathTrans::Render(USHORT **ecran_courant, USHORT **ecran_precedent, UINT nb_x, UINT nb_y, int supportMMX, int move, int calc_x, int calc_y, bool bassdetected)
{	// fô corriger les trans, elles sont pas centrées et y manque 1 ligne ou colonne si nb impairs de pix ...
	// voir les lignes precédés de //**/, mais pour l'instant ça crashe avec les modifs...
	static int time= 1;
	//static int deplacement_x = 0;
	static int compt_dep = 0;
	static int val = 0, val2 = 0;
	static float ang1 = 3.141592653/2, ang2 = 0;

	if( (old_nb_x != nb_x)||(old_nb_y != nb_y)||(newtrans) ) { 
		coef_x = center_x = old_nb_x = nb_x; coef_y = center_y = old_nb_y = nb_y;
		coef_x2 = center_x2 = old_nb_x2 = nb_x + (nb_x>>4); coef_y2 = center_y2 = old_nb_y2 = nb_y + (nb_y>>4);
		taille = old_nb_x * old_nb_y; taille2 = old_nb_x2 * old_nb_y2;
		///**/center_x ++; center_y++; center_x2++; center_y2++;
		center_x >>= 1; center_y >>= 1;
		center_x2 >>= 1; center_y2 >>= 1;
		distancemax = sqrt(pow(old_nb_x, 2) + pow(old_nb_y,2));
		distancemax2 = sqrt(pow(old_nb_x2, 2) + pow(old_nb_y2,2));
		deplacement_x = deplacement_y = ang1 = ang2 = 0;
		time2 = 0;

		if(random) {
			UINT tmp = type; while( tmp == (type = rand()%27) );
			while(type == (nexttype = rand()%27) ); 
			if(slide) { coef_y /= 200; coef_x /= 200; coef_y2 /= 200; coef_x2 /= 200; }
			else  { coef_y /= 400; coef_x /= 400; coef_y2 /= 400; coef_x2 /= 400; }
			pos = 0;
			if(!slide) {
				if(presourcecoordtable) free(presourcecoordtable);
				if(shift) {
					presourcecoordtable = (int*)malloc(taille2 * sizeof(int) + 4);
					int i = 0; while(i<=taille2) presourcecoordtable[i++] = taille;
					//(int*)memset((int*)presourcecoordtable, taille, taille2 + 1);
				}
				else {
					presourcecoordtable = (int*)malloc(taille * sizeof(int) + 4);
					int i = 0; while(i<=taille) presourcecoordtable[i++] = taille;
					//(int*)memset((int*)presourcecoordtable, taille, taille + 1);
				}
			}
		}
		
		Recalculate();
		newtrans = FALSE; // jamais besoin de recalculer une transformation si il y en a déjà une
	} //puis le precalcul éventuel

	if(random) {
		if(slide) {
			if(++time2 >= 350) { 
				if(time2 == 350) {
					int tmp = val;
					while(tmp == (val = rand()%4));
				}
			Recalculate(TRUE, val);
			}
		}
		else Recalculate(FALSE, 0, TRUE);
	}

	int max = taille; 
	int* tmp = sourcecoordtable;

	USHORT* scr_cour = *ecran_courant;
	USHORT* scr_pred = *ecran_precedent;
	
	*(scr_pred + max) = 0;

	if(shift) {	// deplacement transformation
		delta_nbx = ((old_nb_x2 - old_nb_x)>>1) + ((old_nb_x2 - old_nb_x)%2);
		delta_nby = ((old_nb_y2 - old_nb_y)>>1) + ((old_nb_y2 - old_nb_y)%2);
		int delta_nbx2 = deplacement_x = (delta_nbx)?delta_nbx - 1:0;
		int delta_nby2 = deplacement_y = (delta_nby)?delta_nby - 1:0;
		if( (--compt_dep < 0)&&(bassdetected) ) {
			compt_dep = 10;
			ang1 += move / (120.0 + float(rand()%90));
			ang2 += move / (150.0 + float(rand()%100));
		}
		else {
			ang1 += move / (750.0 + float(rand()%650));
			ang2 += move / (600.0 + float(rand()%550));
		}
		switch(shift) {
			case BASSSHIFT: 
				deplacement_x += delta_nbx2 * cos(ang1);
				deplacement_y += delta_nby2 * sin(ang2);
			 break;
			case TOPSHIFT:
				deplacement_y += abs(delta_nby2 * cos(ang1));
				//deplacement_y += delta_nby2;
				deplacement_x = delta_nbx;
				break;
			case TOPRIGHTSHIFT:
				deplacement_x -= abs(delta_nbx2 * cos(ang1));
				deplacement_y += abs(delta_nby2 * cos(ang1));
				//deplacement_x -= delta_nbx2;
				//deplacement_y += delta_nby2;
				break;
			case TOPLEFTSHIFT:
				deplacement_x += abs(delta_nbx2 * cos(ang1));
				deplacement_y += abs(delta_nby2 * cos(ang1));
				//deplacement_x += delta_nbx2;
				//deplacement_y += delta_nby2;
				break;
			case RIGHTSHIFT:
				deplacement_x -= abs(delta_nbx2 * cos(ang1));
				//deplacement_x -= delta_nbx2;
				deplacement_y = delta_nby;
				break;
			case LEFTSHIFT:
				deplacement_x += abs(delta_nbx2 * cos(ang1));
				//deplacement_x += delta_nbx2;
				deplacement_y = delta_nby;
				break;
			case BOTTOMSHIFT:
				deplacement_y -= abs(delta_nby2 * cos(ang1));
				//deplacement_y -= delta_nby2;
				deplacement_x = delta_nbx;
				break;
			case BOTTOMRIGHTSHIFT:
				ang1 += move / (750.0 + float(rand()%650));
				deplacement_x -= abs(delta_nbx2 * cos(ang1));
				deplacement_y -= abs(delta_nby2 * cos(ang1));
				//deplacement_x -= delta_nbx2;
				//deplacement_y -= delta_nby2;
				break;
			case BOTTOMLEFTSHIFT:
				ang1 += move / (750.0 + float(rand()%650));
				deplacement_x += abs(delta_nbx2 * cos(ang1));
				deplacement_y -= abs(delta_nby2 * cos(ang1));
				//deplacement_x += delta_nbx2;
				//deplacement_y -= delta_nby2;
		}
		tmp += deplacement_x;
		tmp += deplacement_y * old_nb_x2;
	} else deplacement_x = deplacement_y = 0; 


	if( ((!calc_x)||(calc_x == old_nb_x))&&(!shift) ) {	// !shift <=> shift = NOMOVE
		//if(calc_x == old_nb_x) max = (max>>1) + (old_nb_x);
		max = (calc_x == old_nb_x)?(max>>1) + (old_nb_x):max;
		
		/*	//bof pas de fast, sert à rien ...
		if(fast) {	//?? le fast déconne, manque 1 ligne en bas quand nb_y impair ...
			if(supportMMX) 
				_asm {
				pushad
				mov ecx, max
				mov edx, tmp
				mov edi, scr_cour
				mov esi, scr_pred
				shl ecx, 1
				sub ecx, 16
				xor ebp, ebp

				ALIGN 16
boucle:
				
				mov eax, [edx + 2 * ebp]
				mov ebx, [edx + 2 * ebp + 16]
				add ebp, 16
				movq mm0, [esi + eax * 2]
				cmp ebp, ecx
				movq mm1, [esi + ebx * 2]
				movq [edi + ebp - 16], mm0
				movq [edi + ebp - 8], mm1
			
				jb boucle

				emms
				popad
				}
			else
				_asm {
				pushad
				mov ecx, max
				mov edx, tmp
				mov edi, scr_cour
				mov esi, scr_pred
				shl ecx, 1
				sub ecx, 16
				xor ebp, ebp

				ALIGN 16
boucle2:

				mov eax, [edx + 2 * ebp]
				mov ebx, [edx + 2 * ebp + 8]
				mov eax, [esi + eax * 2]
				add ebp, 16
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp - 16], eax
				mov [edi + ebp - 12], ebx
			
				mov eax, [edx + 2 * ebp - 16]
				mov ebx, [edx + 2 * ebp - 8]
				mov eax, [esi + eax * 2]
				cmp ebp, ecx
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp - 8], eax
				mov [edi + ebp - 4], ebx
				jb boucle2

				popad
				}
		
		}
		else {	// Trans normale (pas rapide)
			*/
			//while(max--) scr_cour[max] = scr_pred[tmp[max]];
			_asm {
				pushad
				mov ecx, max
				mov edx, tmp
				mov edi, scr_cour
				mov esi, scr_pred
				shl ecx, 1
				sub ecx, 16
				xor ebp, ebp

				ALIGN 16
boucle3:
				//rien de pairable malheureusement à cause des transfer mémoire à chaque ligne
				mov eax, [edx + 2 * ebp]
				mov ebx, [edx + 2 * ebp + 4]
				mov eax, [esi + eax * 2]
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp], ax
				mov [edi + ebp + 2], bx
				//mov [edi + ebp], eax
				//mov [edi + ebp + 2], ebx

				mov eax, [edx + 2 * ebp + 8]
				mov ebx, [edx + 2 * ebp + 12]
				mov eax, [esi + eax * 2]
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp + 4], ax
				mov [edi + ebp + 6], bx
				//mov [edi + ebp + 4], eax
				//mov [edi + ebp + 6], ebx

				mov eax, [edx + 2 * ebp + 16]
				mov ebx, [edx + 2 * ebp + 20]
				mov eax, [esi + eax * 2]
				add ebp, 16
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp - 8], ax
				mov [edi + ebp - 6], bx
				//mov [edi + ebp - 8], eax
				//mov [edi + ebp - 6], ebx
			
				mov eax, [edx + 2 * ebp - 8]
				mov ebx, [edx + 2 * ebp - 4]
				mov eax, [esi + eax * 2]
				cmp ebp, ecx
				mov ebx, [esi + ebx * 2]
				mov [edi + ebp - 4], ax
				mov [edi + ebp - 2], bx
				//mov [edi + ebp - 4], eax
				//mov [edi + ebp - 2], ebx
				jb boucle3
			
				popad
			}
		//}	//paske plus de fast
	} // trans par ligne ...
	else { 
		if(shift) {
			if((!calc_x)||(calc_x == old_nb_x)) {
				int l_y = (calc_y)?calc_y:old_nb_y;
				int tmp2 = old_nb_x;
				/*
				if(fast) {
					int old_nb_x_2 = (old_nb_x<<1);		
					int old_nb_x2_2 = (old_nb_x2<<1);		
					l_y >>= 1;
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_cour[tmp2 + old_nb_x] = scr_pred[tmp[--tmp2]];
						scr_cour += old_nb_x_2; tmp2 = old_nb_x; tmp += old_nb_x2_2;
						l_y--;
					}
				}
				else
				*/
					//while(l_y--) {
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_pred[tmp[--tmp2]];
						scr_cour += tmp2 = old_nb_x; tmp += old_nb_x2;
						l_y--;
					}
			}
			else {
				int l_y = (calc_y)?calc_y:old_nb_y;
				int tmp2 = --calc_x;
				/*
				if(fast) {
					int old_nb_x_2 = (old_nb_x<<1);		
					int old_nb_x2_2 = (old_nb_x2<<1);		
					l_y >>= 1;
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_cour[tmp2 + old_nb_x] = scr_pred[tmp[--tmp2]];
						scr_cour += old_nb_x_2; tmp2 = calc_x; tmp += old_nb_x2_2;
						l_y--;
					}
				}
				else
				*/
					//while(l_y--) {
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_pred[tmp[--tmp2]];
						scr_cour += old_nb_x; tmp2 = calc_x; tmp += old_nb_x2;
						l_y--;
					}
			}
		}
		else {
			int l_y = (calc_y)?calc_y:old_nb_y;
			int tmp2 = --calc_x;
				/*
				if(fast) {
					int old_nb_x_2 = (old_nb_x<<1);		
					l_y >>= 1;
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_cour[tmp2 + old_nb_x] = scr_pred[tmp[--tmp2]];
						scr_cour += old_nb_x_2; tmp2 = calc_x; tmp += old_nb_x_2;
						l_y--;
					}
				}
				else
				*/
					//while(l_y--) {
					while(l_y) {
						while(tmp2) scr_cour[tmp2] = scr_pred[tmp[--tmp2]];
						scr_cour += old_nb_x; tmp2 = calc_x; tmp += old_nb_x;
						l_y--;
					}
		}
			


		/*
		calc_y = (calc_y == MAX_HEIGHT)?MAX_HEIGHT - 1:calc_y;  // il faut aussi décrémenter calc_y sinon ça foire quand = 768 ??
		_asm {	
			pushad

			mov edx, tmp
			mov edi, scr_cour
			mov eax, calc_y
			mov ebx, nb_x
			mov esi, scr_pred
			sub edi, ebx
			sub edi, ebx
			sub edx, ebx
			sub edx, ebx
			sub edx, ebx
			sub edx, ebx
			sub esp, 4			// on s'alloue 4 octets
			mov [esp], eax
			dec [esp]

boucle33:				
			lea edi, [edi + ebx * 2] // ebx = nb_x
			mov ecx, calc_x
			lea edx, [edx + ebx * 4] 
	
			ALIGN 16
boucle22:
			mov eax, [edx + 4 * ecx]
			mov ax, [esi + eax * 2]
			dec ecx
			mov [edi + 2 * ecx + 2], ax
			
			jns boucle22
			
			
			dec dword ptr [esp]
			jns boucle33

			add esp,4	// on libère les 4 octets
				
			popad
			}*/
	}
	
	USHORT* tmp2 = *ecran_courant;	// et on inverse les pointeurs car screenback et en fait screen
	*ecran_courant = *ecran_precedent; *ecran_precedent = tmp2;
}

void MathTrans::Recalculate(int sliding, int num, int pre) {
	float old_x, old_y, angle, distance;
	static float zs1, zs2, ra, ra2;
	//int  depy = 0, arry = 0, depx = 0, arrx = 0, py, px;
	/**/static int  depy = 0, arry = 0, depx = 0, arrx = 0, py = 1, px = 1;
	int new_center_x = center_x, new_center_y = center_y, typetrans, x, y;
	float new_coef_x = coef_x, new_coef_y = coef_y, new_distancemax = distancemax;
	UINT new_taille = taille, new_old_nb_x = old_nb_x, new_old_nb_y = old_nb_y, debtrans;
	int delta_x = 0, delta_y = 0;
	static int val1 = 0, val2 = 0;
	int* coordtable;
	
	if(shift) {
		new_center_x = center_x2; new_center_y = center_y2;
		new_distancemax = distancemax2;
		new_coef_x = coef_x2; new_coef_y = coef_y2; new_taille = taille2;
		new_old_nb_x = old_nb_x2; new_old_nb_y = old_nb_y2;
		delta_x = (old_nb_x2 - old_nb_x)/2; delta_y = (old_nb_y2 - old_nb_y)/2;
	}
	
	if(sliding) {
		if(time2 == 350) { 
			pos = 0;
			type = nexttype;
			while(type == (nexttype = rand()%27) ); 
		
			switch(num) {
			default: case 0: depy = -new_center_y; arry = depy + new_coef_y; depx = -new_center_x; arrx = new_center_x + new_old_nb_x%2; py = px = 1; break;
			case 1: depy = new_center_y - 1 + new_old_nb_y%2; arry = depy - new_coef_y; depx = -new_center_x; arrx = new_center_x + new_old_nb_x%2; py = -1; px = 1; break;
			case 2: depy = -new_center_y ; arry = new_center_y + new_old_nb_y%2; depx = -new_center_x; arrx = depx + new_coef_x; py = px = 1; break;
			case 3: depy = -new_center_y ; arry = new_center_y + new_old_nb_y%2; depx = new_center_x - 1 + new_old_nb_x%2; arrx = depx - new_coef_x; py = 1; px = -1;
			}
		}
		else {
			switch(num) {
			default: case 0: depy = arry; if(arry <= new_center_y - new_coef_y + new_old_nb_y%2) arry += new_coef_y; break;
			case 1: depy = arry; if(arry > -new_center_y) arry -= new_coef_y; break;
			case 2: depx = arrx; if(arrx <= new_center_x - new_coef_x + new_old_nb_x%2) arrx += new_coef_x; break;
			case 3: depx = arrx; if(arrx > -new_center_x) arrx -= new_coef_x;
			}
		}
		
		// hack pour terminer le sliding ... pb dû aux flottants (pour les new_pos_x et y) 
		if(pos == 199) { 
			if(px>0) arrx = new_center_x + new_old_nb_x%2;
			else  arrx = -new_center_x - 1; 
			if(py>0) arry = new_center_y + new_old_nb_y%2;
			else arry = -new_center_y - 1;
		}
		
		// pos varie de 0 à 200, new_coef_y = new_old_nb_y/200
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
				sourcecoordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = taille;
	}
	else if(!pre) {
		if(sourcecoordtable) free(sourcecoordtable);
		sourcecoordtable = (int*)malloc(new_taille * sizeof(int) + 4);
		int i = 0; while(i <= new_taille) sourcecoordtable[i++] = taille;
		//(int*)memset((int*)sourcecoordtable, taille, new_taille + 1);
		
		depy = -new_center_y; arry = new_center_y + new_old_nb_y%2;
		depx = -new_center_x; arrx = new_center_x + new_old_nb_x%2;
		px = py = 1;
	}

	if(pre) {	//donc pas slide
		coordtable = presourcecoordtable;
		typetrans = nexttype;
		int old_depy = depy, old_arry = arry;
		depy = -new_center_y + (pos * new_coef_y); depx = -new_center_x;
		arry = -new_center_y + ((pos+1) * new_coef_y);
		arrx = new_center_x + new_old_nb_x%2;
		py = px = 1;
		if(depy == old_depy) depy = old_arry;
	} else {
		coordtable = sourcecoordtable;
		typetrans = type;
	}

	debtrans = ( ((!sliding)&(!pre)) | (((sliding)|(pre))&(!pos)) );
		
///**/typetrans = 20;
///**/typetrans = WATERZOOM;
	switch (typetrans) {
	case VERTIPERSP:
		if(debtrans) {
			zs1 = .025 + (rand()%75)/1000.0;
			val1 = (rand()%2)?-1:1;
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = distance * (1 + cos(angle)*zs1*val1);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case HORIPERSP:
		if(debtrans) {
			zs1 = .025 + (rand()%75)/1000.0;
			val1 = (rand()%2)?-1:1;
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = distance * (1 + sin(angle)*zs1*val1);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case CYCLOFLOWER: case ILLUSION:
		if(debtrans) {
			if(typetrans == CYCLOFLOWER) {
				zs1 = 5 + rand()%8;		// profondeur de la fleur ( = nb cyclones)
				val1 = 10 + rand()%20;	// force cyclone (+petie val = +grande)
				val2 = 20 + rand()%30;	// profondeur2 de la fleur ( = nb cyclones)
				zs2 = (.05 + (rand()%5)/100.0) * ((rand()%2)?1:-1);
			}
			else if(typetrans == ILLUSION) {
				zs1 = 1000 + rand()%1000;	// "finesse grain"
				val1 = 10 + rand()%20;	// vitesse zoom (+ petite val = + grande)
				val2 = 2 + rand()%6;	// "longueur oscillation"
				zs2 = .08 - (rand()%16)/100.0;
			}
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = angle + .01 + sin(distance/new_distancemax*val2)/val1;
				distance = distance / (1 + (sin(angle * zs1) + .7)/val1);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case SWIRLOUT:
		if(debtrans) {
			zs1 = 1 - (rand()%6)/100.0;	// coef zoom
			val1 = 10 + rand()%30;	// ceof diviseur de vitesse rotation
			val2 = 10 + rand()%30 ;	// nb oscillation (10 =~3)
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = angle + sin(distance/new_distancemax*val2)/val1;
				distance = distance * zs1;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	
	case IMPLOSE:
		if(debtrans) {
			val1 = 2 + rand()%10;	// coef diviseur du "zoom"
			zs1 = .3 + (rand()%40)/100.0;	// coef "d'extension" de l'implosion
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = distance * (1 + (distance/new_distancemax - zs1)/val1);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case ROTOZOOMIN:
		if(debtrans) {
			zs1 = 0.88 + (float(rand()%10)/100.0);
			ra = 0;
			while( (ra > -.05)&&(ra < .05) ) ra = - 0.12 + (float(rand()%24)/100.0);
			int tmp1 = (new_center_x>>4), tmp2 = (new_center_y>>4); tmp1 = (tmp1)?tmp1:1; tmp2 = (tmp2)?tmp2:1; 
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle += ra;
				distance *= zs1;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case ROTOZOOMOUT:
		if(debtrans) {
			zs1 = 1.02 + (float(rand()%10)/100.0);
			ra = 0;
			while( (ra > -.05)&&(ra < .05) ) ra = - 0.12 + (float(rand()%24)/100.0);
			int tmp1 = (new_center_x>>4), tmp2 = (new_center_y>>4); tmp1 = (tmp1)?tmp1:1; tmp2 = (tmp2)?tmp2:1; 
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle += ra;
				distance *= zs1;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case ZOOMIN:
		if(debtrans) {
			zs1 = .88 + (float(rand()%10)/100.0);
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = zs1 * x + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;
				old_y = zs1 * y + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case ZOOMOUT:
		if(debtrans) {
			zs1 = 1.02 + (float(rand()%10)/100.0);
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = zs1 * x + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;
				old_y = zs1 * y + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case ROTATION:
		if(debtrans) {
			ra = 0;
			while( (ra > -.05)&&(ra < .05) ) ra = - 0.12 + (float(rand()%24)/100.0);
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle += ra;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case PETALES:
		if(debtrans) {
			val1 = 1 + (sqrt(new_taille))/((rand()%30) + 30); val1 = (val1&0x1)?val1:val1|0x1;
			val2 = (val1>>1);
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				old_x = (distance * cos(angle)) + new_center_x + rand()%val1 - val2 + .5;
				old_y = (distance * sin(angle)) + new_center_y + rand()%val1 - val2 + .5;
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case ZARB:
		if(debtrans) {
			zs1 = 10 + 2* (rand() % 7);
			val1 = (rand() % 4) + 1; val2 = (rand() % 4) + 1;
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				old_x = (distance * cos(angle) + val1 * sin(angle*zs1)) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = (distance * sin(angle) + val2 * cos(angle*zs1)) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case TUNNEL:
		if(debtrans) {
			zs2 = ((.95 + (float(rand()%10)/100.0)) / new_distancemax);
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				zs1 = 1 - (distance*zs2);
				zs1 *= ( distance - (((absolue(int(angle*100)%3)) == 1)?rand()%5:0) );
				old_x = zs1 * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = zs1 * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
				
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case ROTOTUNNEL:
		if(debtrans) {
			ra = 0;
			while( (ra > -.05)&&(ra < .05) ) ra = - 0.14 + (float(rand()%30)/100.0);
			zs2 = ((.95 + (float(rand()%10)/100.0)) / new_distancemax);
			//zs2 = .95 + (rand()%4)/100.0;
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				zs1 = 1 - (distance*zs2);
				distance *= zs1;
				angle += ra;
				///**/distance = /*zs2 * */atan(distance*1.57/new_distancemax)*new_distancemax/1.62;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
				
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case WATERZOOM:
		if(debtrans) {
			//zs1 = 1;
			//while( (zs1 > .98)&&(zs1 < 1.02) ) zs1 = 0.88 + (float(rand()%24)/100.0);
			zs1 = 0.94 + (float(rand()%5)/100.0);
			val1 = 2 + rand()%6; // nombre de "vagues"
			val2 = 2 + rand()%6; // "hauteur" des "vagues"
			/**/ra = 1/new_distancemax;
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = zs1 * ( distance + val2 * sin(distance * (6,24*ra*val1)) );
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
				
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case CYCLONE:
		if(debtrans) {
			zs1 = .01 + ((rand()%50)/1000.0);
			zs2 = .01 + ((rand()%50)/1000.0);
			val1 = 5 + rand()%20; val2 = 5 + rand()%20;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = x + val1 * sin( zs1 * y ) + new_center_x + .5;
				old_y = y + val2 * sin( zs2 * x ) + new_center_y + .5;

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case HORIZOOM:
		if(debtrans) {
			zs1 = 1;
			while( (zs1 > .98)&&(zs1 < 1.02) ) zs1 = 0.84 + (float(rand()%32)/100.0);
			int tmp1 = (new_center_x>>4); tmp1 = (tmp1)?tmp1:1;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = x + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = y * zs1 + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case VERTIZOOM:
		if(debtrans) {
			zs1 = 1;
			while( (zs1 > .98)&&(zs1 < 1.02) ) zs1 = 0.84 + (float(rand()%32)/100.0);
			int tmp1 = (new_center_x>>4); tmp1 = (tmp1)?tmp1:1;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = x * zs1 + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = y + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case RAYSUN:
		if(debtrans) {
		zs1 = int(8 + rand()%5) * 3.141592653;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = ( ( 1 + sin(zs1 * angle) ) * .5 * .06 + .92) * distance;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;
					
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case BIROTOZOOM:
		if(debtrans) {
			zs1 = 1; ra = 0;
			while( (zs1 > .98)&&(zs1 < 1.02) ) zs1 = 0.85 + (float(rand()%30)/100.0);
			if(zs1 > 1) zs2 = .985 - (rand()%14)/100.0; else zs2 = 1.015 + (rand()%14)/100.0;
			int tmp1 = (new_center_x>>4), tmp2 = (new_center_y>>4); tmp1 = (tmp1)?tmp1:1; tmp2 = (tmp2)?tmp2:1; 
			while( (ra > -.04)&&(ra < .04) ) ra = - 0.11 + (float(rand()%22)/100.0);
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle += ra;
				old_x = zs1 * distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = zs2 * distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		}
		break;
	case BIZOOM:
		if(debtrans) {
			zs1 = 1;
			while( (zs1 > .98)&&(zs1 < 1.02) ) zs1 = 0.88 + (float(rand()%24)/100.0);
			if(zs1 > 1) zs2 = .985 - (rand()%10)/100.0; else zs2 = 1.015 + (rand()%10)/100.0;
			int tmp1 = (new_center_x>>4), tmp2 = (new_center_y>>4); tmp1 = (tmp1)?tmp1:1; tmp2 = (tmp2)?tmp2:1; 
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = zs1 * float(x) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = zs2 * float(y) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case ROTOSUN:
		if(debtrans) {
		zs1 = int(1 + (rand()%21)/10.0) * 3.141592653;
		zs2 =  - .05 + (rand()%100)/1000.0;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				distance = ( ( 1 + cos(zs1 * angle) ) * .05 + .85) * distance;
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
					
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case BOX:
		if(debtrans) {
		val1 = 2 * (4 + rand()%16);
		zs1 = (new_center_x) / (40 + rand()%60);
		zs2 = (new_center_y) / (40 + rand()%60);
		ra = 1.0/new_center_x;
		ra2 = 1.0/new_center_y;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				old_x = x + zs1 * cos(val1 * .8 * ra * x) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = y + zs2 * cos(val1 * .8 * ra2 * y) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
					
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case GALAXY:
		if(debtrans) {
			zs1 = (rand()%314)/100.0;
			zs2 = .01 * (.5 + (rand()%10)/10.0);
			ra = .1 * (.5 + (rand()%10)/10.0);
		}
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				ra2 = 2 * angle + 6.283185306*angle + zs1;
				angle = angle + ra * sin(ra2);
				distance = distance + zs2 * cos(ra2);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	case SHADOWIN:
		if(debtrans) {
			zs1 = .88 + (float(rand()%10)/100.0);
		}

		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				//distance *= .94*(cos(angle) + 4.3)/5.0;
				//distance = zs1*distance + ((rand()%21) - 10);
				distance = zs1*distance - (rand()%10);
				distance = max(0, distance);
				old_x = distance * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;

	default: case NOMOVE: // on ne change pas les coordonnées (mais on peut y ajouter le noise et le deplacement)
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
					old_x = x + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
					old_y = y + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
		
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
				}
			} 
/*
	case ROTOBRICK:
		//if( ((!sliding)&&(!pre)) || (((sliding)||(pre))&&(!pos)) ) {
		if(debtrans) {
			zs1 = -.05 + ((rand()%10)/100.0);
			zs2 = 1 + ((rand()%1)/1000.0);
			val1 = 10 + rand()%15;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				old_x = (zs2 * int(distance/val1)*val1) * cos(angle + zs1) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = (zs2 * int(distance/val1)*val1) * sin(angle + zs1) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	

				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
	case WATER:
		//if( ((!sliding)&&(!pre)) || (((sliding)||(pre))&&(!pos)) ) {
		if(debtrans) {
		zs1 = 0.02 + (rand()%6)/100.0;
		zs2 = 1/new_distancemax;
		}
		
		for(y = depy; (py > 0)?(y < arry):(y > arry); y+=py)
		{
			for(x = depx; (px > 0)?(x < arrx):(x > arrx); x+=px)
			{
				distance = distance_angle[0][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				angle = distance_angle[1][(x+(n_x>>1)) + (y+(n_y>>1))*n_x];
				float distance2 = distance * sin(distance * zs1);
				//angle -= ((distance*float(1.14/new_distancemax)) - .42) * ((rand()%9)/10.0);
				angle -= ((distance*float(1.14*zs2)) - .42) * ((rand()%9)/10.0);
				old_x = distance2 * cos(angle) + new_center_x + ((noise)?(rand()%5) - 2:0) + .5;	
				old_y = distance2 * sin(angle) + new_center_y + ((noise)?(rand()%5) - 2:0) + .5;	
					
				if( (old_y < old_nb_y + delta_y) && (old_y >= delta_y) && (old_x < old_nb_x + delta_x) && (old_x >= delta_x) )
					coordtable[(x + int(new_center_x)) + (new_old_nb_x * (y + int(new_center_y)))] = (old_x - delta_x) + (int(old_y - delta_y) * old_nb_x);
			}
		} break;
*/
		} // fin du switch

	if(sliding) { if(++pos >= 200) time2 = 0;}	//on arrête le rendu là
	else if(pre) {
		if(++pos >= 400) { 
			pos = 0;
			type = nexttype;
			while(type == (nexttype = rand()%27) ); 
			if(sourcecoordtable) { free(sourcecoordtable); sourcecoordtable = NULL; }
			sourcecoordtable = presourcecoordtable;
			presourcecoordtable = (int*)malloc(new_taille * sizeof(int) + 4);
			int i = 0; while(i <= new_taille) presourcecoordtable[i++] = taille;
			//(int*)memset((int*)presourcecoordtable, taille, new_taille + 1);
		}
	}
}


// les "mirroirs":
void Horizontal(USHORT *dest, USHORT *src, UINT XRes, UINT YRes, int pitch) {
	int nb_y = (YRes>>1) + 1, /*reste,*/ taille, nb_x = (XRes<<1), i;

	nb_y <<= 1;
	// premiere moitie (horizontal)
	if(pitch == XRes) {
		taille = XRes * nb_y;
		memcpy(dest, src, taille);
		dest += (taille>>1); src += (taille>>1);
	}
	else 
		for(i = 0; i < nb_y ; i += 2)
		{ 
			memcpy(dest, src, nb_x); 
			dest += pitch; //fô que sa valeur soit correcte et non égale à 0
			src += XRes; 
		} 

	// seconde moitie
	src -= XRes;
	for(i = 0; i < nb_y - 1; i += 2)
	{ 
		memcpy(dest, src, nb_x); 
		dest += pitch; //fô que sa valeur soit correcte et non égale à 0
		src -= XRes; 
	} 
}

void Vertical(USHORT *dest, USHORT *src, UINT XRes, UINT YRes, int pitch) {
	int nb_x = (XRes&0xFFFE) + 2, i;
	USHORT* d, *s;
	
	//première moitié (verticale)
	for(d = dest, s = src, i = 0; i < YRes ; i ++, d += pitch, s += XRes)
		memcpy(d, s, nb_x); 
		
	//seconde moitié
	int taille = (XRes<<1) - nb_x;
	for(d = dest + (nb_x>>1), s = src, i = 0; i < YRes ; i ++, d += pitch, s += XRes)
		MemCopyInv(d, s, taille); 
}


void Double(USHORT *dest, USHORT *src, UINT XRes, UINT YRes, int pitch) {
	int nb_x = (XRes&0xFFFE) + 2, nb_y = (YRes>>1) + 1, i;
	USHORT* d, *s;
	
	//première moitié (verticale)
	for(d = dest, s = src, i = 0; i < nb_y ; i ++, d += pitch, s += XRes)
		memcpy(d, s, nb_x); 

	for(d = dest + (nb_y) * pitch, s = src + (nb_y - 1) * XRes, i = 0; i < nb_y - 1; i++, d += pitch, s -= XRes)
		memcpy(d, s, nb_x); 

	//seconde moitié
	int taille = (XRes<<1) - nb_x;
	for(d = dest + (nb_x>>1), s = src, i = 0; i < nb_y ; i++, d += pitch, s += XRes)
		MemCopyInv(d, s, taille); 

	for(d = dest + (nb_x>>1) + (nb_y) * pitch, s = src + (nb_y - 1) * XRes, i = 0; i < nb_y - 1; i++, d += pitch, s -= XRes)
		MemCopyInv(d, s, taille); 
}




