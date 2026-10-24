#include "graph.h"
#include "log.h"

void ligne(unsigned short *ecran, int x1, int y1, int x2, int y2, unsigned short coul, int nb_x, int nb_y)
{
	//pas la peine de tracer la ligne si tout est en dehors
	//if( ((x1 < 0)&&(x2 < 0))||((x1 >= nb_x)&&(x2 >= nb_x))||((y1 < 0)&&(y2 < 0))||((y1 >= nb_y)&&(y2 >= nb_y)) )
	//	return ;
	

	int p1x = 1, p1y = 1, p2x = 1, p2y = 1;
	int so, add1, add2, x, deltax, deltay;

	deltax = x2 - x1;
	deltay	= y2 - y1;
 	
	// gestion cliping
	//if(x1 < 0) { x1 = 0; int rapx= x2 / fabsolue(deltax); y1 = y2 + deltay * rapx; }
	//if(x2 < 0) { x2 = 0; int rapx= x1 / fabsolue(deltax); y2 = y1 + deltay * rapx; }
	//if(y1 < 0) { y1 = 0; int rapy= y2 / fabsolue(deltay); x1 = x2 + deltax * rapy; }
	//if(y2 < 0) { y2 = 0; int rapy= y1 / fabsolue(deltay); x2 = x1 + deltax * rapy; }
	//if(x1 >= nb_x) { x1 = nb_x - 1; int difx = nb_x - 1 - x2; float rapx = difx / fabsolue(deltax); y1 = y2 + deltay * rapx; }
	//if(x2 >= nb_x) { x2 = nb_x - 1; int difx = nb_x - 1 - x1; float rapx = difx / fabsolue(deltax); y2 = y1 + deltay * rapx; }
	//if(y1 >= nb_y) { y1 = nb_y - 1; int dify = nb_y - 1 - y2; float rapy = dify / fabsolue(deltay); x1 = x2 + deltax * rapy; }
	//if(y2 >= nb_y) { y2 = nb_y - 1; int dify = nb_y - 1 - y2; float rapy = dify / fabsolue(deltay); x1 = x2 + deltax * rapy; }

	if (deltax < 0)
	{
		deltax =- deltax;
		p1x =- 1;
	}
 
	if (deltay < 0)
	{
		deltay =- deltay;
		p1y =- 1;
	}
	
	p2x = p1x;
	p2y = p1y;

	if (deltax < deltay)
	{
		x = deltax;
		deltax = deltay;
		deltay = x;
		p2x = 0;
	}
	else p2y = 0;

	deltay += deltay;
	add1 =- deltay;
	so = deltay - deltax;
	add2 =- (so - deltax);
	so = -so;

	__asm {
		pushad

		add esp, 4
		mov eax, p1y
		mul nb_x
		mov ecx, eax
		mov esi, deltax
		mov [esp], esi
		mov eax, p2y
		mov edi, ecran
		mov ebx, edi
		and ebx, 1
		shr edi, 1
		add edi, ebx
		mul nb_x
		mov ebx, eax
		mov eax, y1
		mov esi, so
		mul nb_x
		add eax, x1
		add edi , eax
		mov edx, deltax	
		mov ax, coul

restepix:

		mov [edi * 2], ax

		test esi, 0x80000000
		jz suite
		add edi, p1x
		add esi, add2
		add edi, ecx
		dec edx
		jns  restepix
		jmp fin
suite:
		add edi, p2x
		add esi, add1
		add edi, ebx
		dec edx
		jns  restepix

fin:
		sub esp, 4
		
		popad
	}
}



void ligne_trans(unsigned short *ecran, int x1, int y1, int x2, int y2, unsigned short coul, int nb_x, int nb_y, int mode555)
{
	int p1x = 1, p1y = 1, p2x = 1, p2y = 1;
	int so, add1, add2, x, deltax, deltay;

	deltax = x2 - x1;
 
	if (deltax < 0)
	{
		deltax =- deltax;
		p1x =- 1;
	}

	deltay	= y2 - y1;
 
	if (deltay < 0)
	{
		deltay =- deltay;
		p1y =- 1;
	}
 
	p2x = p1x;
	p2y = p1y;

	if (deltax < deltay)
	{
		x = deltax;
		deltax = deltay;
		deltay = x;
		p2x = 0;
	}
	else
		p2y = 0;

	deltay += deltay;
	add1 =- deltay;
	so = deltay - deltax;
	add2 =- (so - deltax);
	so = -so;

/*	if(mode555) {
restepix:
		__asm {
			pushad
		
			mov eax, y1
			mov edi, ecran
			mul nb_x
			add eax, x1
			add edi, eax
			add edi, eax
			mov dx, coul

			mov cx, [edi]
			/**
			mov bx, dx
			mov ax, cx

			and dx, 0x7C1F
			and	cx, 0x7C1F
			and bx, 0x3E0
			and ax, 0x3E0
			add dx, cx
			add bx, ax
			//shr dx, 1
			//shr bx, 1
			and dx, 0x7C1F
			and bx, 0x3E0
		
			or dx, bx
			/**/
/*			or dx, cx

			mov [edi], dx
			
			popad
	    }

		if (so < 0)
		{
			x1 += p1x;
			y1 += p1y;
			so += add2;
 
			if (deltax-- >= 0)
				goto restepix;
 
			goto fin;
		}
		else
		{
			x1 += p2x;
			y1 += p2y;
			so += add1;
 
			if (deltax-- > 0)
				goto restepix;
		}
fin:	;
	}
	else {
*/
restepix2:
		__asm {
			pushad
		
			mov eax, y1
			mov edi, ecran
			mul nb_x
			add eax, x1
			add edi, eax
			add edi, eax
			mov dx, coul
		
			mov cx, [edi]
			/**
			mov bx, dx
			mov ax, cx
			
			and dx, 0xF81F
			and	cx, 0xF81F
			and bx, 0x7E0
			and ax, 0x7E0
			add dx, cx
			add bx, ax
			shr dx, 1
			shr bx, 1
			and dx, 0xF81F
			and bx, 0x7E0
			
			or dx, bx
			/**/
			or dx, cx
			
			mov [edi], dx
		
			popad
	    }

		if (so < 0)
		{
			x1 += p1x;
			y1 += p1y;
			so += add2;
	
			if (deltax-- >= 0)
				goto restepix2;
 
			goto fin2;
		}
		else
		{
			x1 += p2x;
			y1 += p2y;
			so += add1;
 
			if (deltax-- > 0)
				goto restepix2;
		}
fin2:	;
//		}
}



void SuperGraphe(unsigned short *screen, int x, int y, int diam, int taillept, int nbpoint, int ang, int param, unsigned short coul, unsigned int nb_x, unsigned int nb_y, int style) {
// nbpoint est une valeur entre 1 et 5 correpondant à la densité du nb de point

	int angle, angle2, diam_cour, decompt, incangle, incangle2, anglemax;
	int d1, d2, x1, y1;
	
	// réajustement coordonnées pour qu'elles ne dépassent l'écran
	x = maximum(x, 0); x = minimum(nb_x - 1, x);
	y = maximum(y, 0); y = minimum(nb_y - 1, y);

	// réajustement diamètre pour qu'il ne dépasse pas de l'écran
	//if( (x+diam+taillept)>=nb_x) diam = nb_x - x - taillept - 1;
	//if( (y+diam+taillept)>=nb_y) diam = nb_y - y - taillept - 1;
	//if( (x-diam)<0) diam = x - 1;
	//if( (y-diam)<0) diam = y - 1;

	if(diam <= taillept) {	// si 1 seul point, on le fait pis cassos
		point_taille(screen, x, y, taillept, coul, nb_x, nb_y);
		return;
	}

///**/style = SPIRALE; 

	switch(style) { 
	case SPIRALE:	// param ~= nb "fermetures cycliques" de la spirale
		decompt = 0;
		diam = maximum(1 + (minimum(nb_x, nb_y)>>5), diam);
		for(angle = ang, diam_cour = taillept; diam_cour < diam; ) {
			point_taille(screen, x + ((diam_cour * cosinus(angle))>>16), y + ((diam_cour * sinus(angle))>>16), taillept, coul, nb_x, nb_y);
			
			if(--decompt < 0) {
				//if(diam < (minimum(nb_x, nb_y)>>4)) decompt = (diam_cour+( nbpoint - (diam_cour>>1)))/taillept;
				//else decompt = (nbpoint*diam)/( ((diam - diam_cour)<<2) + 1);
				//decompt = ((nbpoint>>1)*diam)/( ((diam - (diam_cour>>2))) + 1);
				decompt = 5* ((10 + diam_cour*(nbpoint>>1))/(diam+1));

				diam_cour++;
			}
			//incangle = ((1+(taillept>>1))*512)/((diam_cour+diam)*param);
			incangle = (param + 1)*3/(1+(diam_cour>>4));
			angle += (incangle)?incangle:1;
		}
		break;
	case FLEUR:
		anglemax = ang + 512*param;
		incangle = (anglemax - ang)/(diam*5*((nbpoint>>2)+1));
		incangle = (incangle)?incangle:1;
	
		for(angle = ang; angle < anglemax; angle += incangle) {
			x1 = x + ( (((diam * cosinus(angle))>>16) * sinus(angle*param))>>16 );
			y1 = y + ( (((diam * sinus(angle))>>16) * sinus(angle*param))>>16 );
			
			point_taille(screen, x1, y1, taillept, coul, nb_x, nb_y);
		}
		break;
	case ONDULATION:
		anglemax = ang + 512;
		incangle = 512/(diam*nbpoint);
		incangle = (incangle)?incangle:1;
		incangle2 = incangle * param;
		d1 = (diam>>1) + (diam>>2);
		d2 = (diam>>2);

		for(angle = ang, angle2 = 0; angle < anglemax; angle += incangle, angle2 += incangle2) {
			x1 = x + ( (d1 * cosinus(angle))>>16 ) + ( (d2 * sinus(angle2))>>16 );
			y1 = y + ( (d1 * sinus(angle))>>16 ) + ( (d2 * cosinus(angle2))>>16 );
			point_taille(screen, x1, y1, taillept, coul, nb_x, nb_y);
		}
		break;
	case CERCLEVAGUE:
		anglemax = ang + 512;
		incangle = (512/diam)/nbpoint;
		incangle = (incangle)?incangle:1;
		incangle2 = incangle * param;
		d1 = (diam>>1) + (diam>>2);
		d2 = (diam>>2);

		for(angle = ang, angle2 = 0; angle < anglemax; angle += incangle, angle2 += incangle2) {
			x1 = x + ( (d1 * cosinus(angle))>>16 ) + ( (d2 * cosinus(angle2))>>16 );
			y1 = y + ( (d1 * sinus(angle))>>16 ) + ( (d2 * sinus(angle2))>>16 );
				
			point_taille(screen, x1, y1, taillept, coul, nb_x, nb_y);
		}
	}
}


void ligne_graphe(unsigned short *ecran, int x1, int y1, int x2, int y2, unsigned short coul, unsigned int nb_x, unsigned int nb_y, unsigned char *wavedata, int type) {

	UINT deltax, deltay;
	int* tabdecy;
	bool negy = FALSE, negx = FALSE;
	int balancexy, incj;
	int i, x, j, oldy, y, ycentre, data;	//variables 

	// clipping
	x1 = (x1<0)?0:((x1>=nb_x)?nb_x - 1:x1);
	x2 = (x2<0)?0:(x2>=nb_x)?nb_x - 1:x2;
	y1 = (y1<0)?0:(y1>=nb_y)?nb_y - 1:y1;
	y2 = (y2<0)?0:((y2>=nb_y)?nb_y - 1:y2);


	// maj de negy et negx
	if(x1>x2) negx = TRUE;
	if(y1>y2) negy = TRUE;

	// calcul des deltas
	deltax = (negx)?(x1-x2):(x2-x1);
	deltay = (negy)?(y1-y2):(y2-y1);
	
	// premier point toujours à (x1, y1)
	point(ecran, x1, y1, coul, nb_x);
	
	// si un point à faire, on a fini
	if( (!deltax)||(deltax == 1) ) return;

	
	// allocation tab de décalage des y
	tabdecy = (int*)malloc(deltax*sizeof(int));
	
	// initialisation balance
	balancexy = int(deltax) - int(deltay);
	
#ifdef _DEBUG
	if(debugfile) fprintf(debugfile, "deltax = %u\n", deltax);
#endif

	// calcul du tableau de décalage des x par rapport à x
	tabdecy[0] = 0;
	for(i = 1; i<deltax; i++) {
		tabdecy[i] = 0;
		while(balancexy < 0) {
			balancexy += deltax;
			if(negy) tabdecy[i]--;
			else tabdecy[i]++;
		}
		balancexy -= deltay;
	}

	// initialisation pour premier point
	oldy = y = ycentre = y1;
	incj = ((576/deltax)<<2);
	int incx = (negx)?-1:1;

	switch(type) {
		
	case PT:
		//for(x = x1 + 1, i = 1, j = 0; x < x2; x++, i++, j+=incj) {
		for(x = x1 + incx, i = 1, j = 0; ((negx)?(x > x2):(x < x2)); x += incx, i++, j+=incj) {
		
			ycentre += tabdecy[i];
			data = unsigned(wavedata[(j>>2)]^128);
			data *= nb_y;
			data >>= 11;
			data -= (nb_y >> 4);	// pour le recentrer / l'axe
			y = ycentre + data;
			y = (y < 0)?0:( (y>=nb_y)?nb_y - 1:y );
			point(ecran, x, y, coul, nb_x);
		}
		break;
	case LINE:
		//for(x = x1 + 1, i = 1, j = 0; x < x2; x++, i++, j+=incj) {
		for(x = x1 + incx, i = 1, j = 0; ((negx)?(x > x2):(x < x2)); x += incx, i++, j+=incj) {
		
			ycentre += tabdecy[i];
			data = unsigned(wavedata[(j>>2)]^128);
			data *= nb_y;
			data >>= 11;
			data -= (nb_y >> 4);	// pour le recentrer / l'axe
			y = ycentre + data;
			y = (y < 0)?0:( (y>=nb_y)?nb_y - 1:y );
			//ligne(ecran, x - 1, oldy, x, y, coul, nb_x, nb_y);
			ligne(ecran, x - incx, oldy, x, y, coul, nb_x, nb_y);
			oldy = y; 
		}
		break;
	case FILL:
		//for(x = x1 + 1, i = 1, j = 0; x < x2; x++, i++, j+=incj) {
		for(x = x1 + incx, i = 1, j = 0; ((negx)?(x > x2):(x < x2)); x += incx, i++, j+=incj) {

			ycentre += tabdecy[i];
			data = unsigned(wavedata[(j>>2)]^128);
			data *= nb_y;
			data >>= 11;
			data -= (nb_y >> 4);	// pour le recentrer / l'axe
			y = ycentre + data;
			ycentre = (ycentre < 0)?0:( (ycentre>=nb_y)?nb_y - 1:ycentre );
			y = (y < 0)?0:( (y>=nb_y)?nb_y - 1:y );
		
			//ligne(ecran, x - 1, oldy, x, ycentre, coul, nb_x, nb_y);
			ligne(ecran, x - incx, oldy, x, ycentre, coul, nb_x, nb_y);
			vertical_ligne(ecran, x, ycentre, y, coul, nb_x);
			oldy = ycentre; 
		}
	}

	free(tabdecy);
}



