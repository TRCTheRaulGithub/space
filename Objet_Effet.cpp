#include "objet_effet.h"

/*****/
#include <windows.h>

UINT GetCoul(int number, int mode555, int primary) {

	int rouge = (mode555)?0x7C00:0xF800;
	int vert = (mode555)?0x3E0:0x7E0;
	int bleu = 0x1F;

	if(primary)
		switch(number) {
		case 0: return rouge; 
		case 1: return vert;  
		case 2: return bleu;  
		case 3: return vert + rouge;
		case 4: return bleu + rouge;
		case 5: return vert + bleu;
		default: case 7: return vert + bleu + rouge;
		case 6: return vert + (bleu>>1);
		case 15: return ((vert>>1)&vert) + bleu;
		case 8: return (bleu>>1) + rouge;
		case 9: return bleu + ((rouge>>1)&rouge);
		case 10: return ((vert>>1)&vert) + rouge;
		case 11: return vert + ((rouge>>1)&rouge);
		case 12: return ((vert>>1)&vert) + bleu + rouge;
		case 13: return vert + (bleu>>1) + rouge;
		case 14: return vert + bleu + ((rouge>>1)&rouge);
		case 16: return vert + (bleu>>1) + ((rouge>>1)&rouge);
		case 17: return ((vert>>1)&vert) + (bleu>>1) + rouge;
		case 18: return ((vert>>1)&vert) + bleu + ((rouge>>1)&rouge);
		}
	else
	{
		int dr = (mode555)?10:11;
		int dv = (mode555)?5:6;

		switch(number) {
		case 0: return rouge + ((vert>>1)&vert) + (bleu>>3); 
		case 1: return vert + (bleu>>2);  
		case 2: return bleu + ((vert>>2)&vert);  
		case 3: return (23<<dr) + (30<<dv) + 12;
		case 4: return bleu + ((rouge>>2)&rouge);
		case 5: return ((vert>>1)&vert) + (bleu>>1) + ((rouge>>1)&rouge);
		default: case 7: return (14<<dr) + (12<<dv) + 3;
		case 6: return (19<<dr) + (30<<dv) + 25;
		case 15: return (29<<dr) + (24<<dv) + 18;
		case 8: return (2<<dr) + (20<<dv) + 19;
		case 9: return (30<<dr) + (21<<dv) + 25;
		case 10: return (10<<dr) + (29 << dv) + 16;
		case 11: return (22<<dr) + (19<<dv) + 30;
		case 12: return (1<<dr) + (22<<dv) + 31;
		case 13: return (24<<dr) + (18<<dv) + 30;
		case 14: return (30<<dr) + (20<<dv) + 26;
		case 16: return (20<<dr) + (20<<dv) + 30;
		case 17: return (1<<dr) + (21<<dv) + 20;
		case 18: return (28<<dr) + (25<<dv) + 1;
		}
	}
}


UINT GetInterpoledCoul(UINT coul, UINT destcoul, int mode555, int inc) {
	if( (coul&0xFFFF)<12 ) return (coul + inc);
	else {
		USHORT rougecoul = (mode555)?(coul>>26):(coul>>27);
		USHORT vertcoul = (mode555)?(coul>>21)&0x1F:(coul>>21)&0x3F;
		USHORT bleucoul = (coul>>16)&0x1F;
		USHORT rougedestcoul = (mode555)?(destcoul>>10):(destcoul>>11);
		USHORT vertdestcoul = (mode555)?(destcoul>>5)&0x1F:(destcoul>>5)&0x3F;
		USHORT bleudestcoul = destcoul&0x1F;
		
		if(rougecoul < rougedestcoul) rougecoul++;
		else if(rougecoul > rougedestcoul) rougecoul--;
		if(vertcoul < vertdestcoul) vertcoul++;
		else if(vertcoul > vertdestcoul) vertcoul--;
		if(bleucoul < bleudestcoul) bleucoul++;
		else if(bleucoul > bleudestcoul) bleucoul--;
		
		coul = (mode555)?(rougecoul<<26):(rougecoul<<27);
		coul += (vertcoul<<21);
		coul += (bleucoul<<16);

		return coul;
	}
}


//Objet GeoGraph : des grandes figures mathématiques
int GeoGraphEff::nombre = 0;

GeoGraphEff::GeoGraphEff(GeoGraphEff *GeoGraph[MAX_GEOGRAPH_EFFECTS]) {
	nombre++;

	style = rand()%4;	// FLEUR, SPIRALE, ONDULATION, CERCLEVAGUE
	
	basecoul = rand() % 19;
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);

	if(nombre == 1) // première création, le cercle est au centre de l'écran
		decal = NO;
	else			// sinon il est décalé d'un rayon de cercle par rapport au centre
		decal = YES;
	
	if(nombre > 1)
	{
		int angdep = 0, incang = 512 / nombre;

		for(int i = 0;  i < nombre - 1; i++)
		{ GeoGraph[i]->anglecentre = angdep; angdep += incang; }
		
		GeoGraph[0]->decal = YES;
		GeoGraph[0]->anglecentre = 0; anglecentre = angdep;
	}
	else
		anglecentre = 0;

	angleeff = rand()%512;
	gauche = TRUE;
	counter = 0;
	counter2 = GetTickCount();
	old_accel = 64;
}

GeoGraphEff::~GeoGraphEff() { nombre--; }

void GeoGraphEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat) {
	
	int centrex = (nb_x>>1);
	int centrey = (nb_y>>1);

	static int oldmode555 = 0;
	int decalx = (centrex>>1), decaly = (centrey>>1);
	
	accel = min(128, accel);

	int diam = (min(nb_x, nb_y) >> 5) * accel;
	int taillept = 1;
	int incangle =  1 + (accel>>3);
	int incangle2 =  1 + (accel>>3);  
	/**/int nbpoint = 1;
	/**/nbpoint += (accel>>4);	// 1 <= nbpoint <= 15

	diam >>= (decal)?4:3;
	diam *= (Beat)?1.2:1;

	taillept += (accel>>5);
	taillept *= (Beat)?((nb_x+nb_y)/400.0):((nb_x+nb_y)/600.0);
	taillept = max(1, taillept);
	taillept = min((nb_x+nb_y)/100, taillept);

	if(Beat) counter++;
	if(counter >= 2) {	// on change le sens de rotation toutes les 2 basses
		gauche = (gauche)?FALSE:TRUE;
		counter = 0;
	}


	if(GetTickCount() - counter2 > 2500) {	// si 1,5 secondes sont passés, on change le style
		counter2 = GetTickCount();
		style = rand()%4;
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, 1); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, 1);
	}


	angleeff += (gauche)?incangle2:-incangle2;
	
	if(oldmode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	} else {
		while ((basecoul>>16) == nextcoul) 
			nextcoul = GetCoul(rand() % 19, mode555, primary);
	
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>2)+1);
	}
	
	oldmode555 = mode555;

	int x = centrex;
	int y = centrey;
	//decalx += (centrex>>2) + (old_accel - accel);
	//decaly += (centrey>>2) + (old_accel - accel);
	decalx *= float(accel/50.0);
	decaly *= float(accel/50.0);
	decalx = min(centrex, decalx);
	decaly = min(centrey, decaly);
	decalx = max(0, decalx);
	decaly = max(0, decaly);
	
	if(decal) {
		anglecentre = (anglecentre + incangle)&511;
		x += ( (decalx*cosinus(anglecentre))>>16 );
		y += ( (decaly*sinus(anglecentre))>>16 );
	}
	
	old_accel = accel;

	int param = 1;
	switch(style) {
	case FLEUR: param += (accel>>4); break;
	case SPIRALE: param += (accel>>4); break;
	default: param += (accel>>3);
	}

	SuperGraphe(screen, x, y, diam, taillept, nbpoint, angleeff, param, (basecoul>>16), nb_x, nb_y, style);
}



//Objet LittleMath : des ptits figures mathématiques
int LittleMathEff::nombre = 0;

LittleMathEff::LittleMathEff() {
	nombre++;

	style = rand()%4;	// FLEUR, SPIRALE, ONDULATION, CERCLEVAGUE
	
	basecoul = rand() % 19;
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);

	x = y = 0;
	counter = GetTickCount();
}

LittleMathEff::~LittleMathEff() { nombre--; }

void LittleMathEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat) {

	if( (accel < 15) &&( (rand()%(accel+1))<=8 ) ) return;	// on n'affiche rien si pas de assez de "mouvement"

	static int oldmode555 = 0;
	
	accel = min(128, accel);
	//counter++;

	int diam = min(nb_x, nb_y) * (10 + accel);
	float taillepoint = 1;
	int taillept;
	int nbpoint = 1;
	nbpoint += rand()%5;	// 1 <= nbpoint <=5

	diam >>= 10;	// divise par 128 * 8 pour environ 1/8 de min(nb_x, nb_y)
	diam <<= (Beat)?1:0;
	//diam = max(1, diam);
	diam = max( (min(nb_x, nb_y)>>4), diam);

	taillepoint += (accel>>5);
	taillepoint *= (Beat)?2:1;
	taillepoint *= float((nb_x+nb_y)/600);
	taillepoint = min((nb_x+nb_y)/150, taillepoint);
	taillepoint = max(2, taillepoint);
	taillept = (int)taillepoint;
	

	/*
	x = (nb_x>>1);
	y = (nb_y>>1);
	int decx = rand()%((x*(60 + ((Beat)?(accel<<1):accel)))/188 - diam);
	int decy = rand()%((y*(60 + ((Beat)?(accel<<1):accel)))/188 - diam);
	x += (rand()%2)?decx:-decx;
	y += (rand()%2)?decy:-decy;
	*/
	x = rand()%nb_x;
	y = rand()%nb_y;

	if(GetTickCount() - counter > 2500) {	// si 1,5 secondes sont passés, on change le style
		counter = GetTickCount();
		style = rand()%4;
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, 1); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, 1);
	}
	
	if(oldmode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	} else {
		while ((basecoul>>16) == nextcoul) 
			nextcoul = GetCoul(rand() % 19, mode555, primary);
	
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>2)+1);
	}
	
	oldmode555 = mode555;
	int param;

	switch(style) {
	case CERCLEVAGUE: param = 1 + (accel>>4); break;
	case SPIRALE: param = 2 + rand()%3 + (accel>>6); break;
	case FLEUR: nbpoint = 4 + rand()%2; param = 1 + rand()%6; break;
	case ONDULATION: param = 1 + (nbpoint>>1) + rand()%5;
	}

	SuperGraphe(screen, x, y, diam, taillept, nbpoint, rand()%512, param, (basecoul>>16), nb_x, nb_y, style);
}


//Objet LittleWave : des ptits waves (plusieurs types: genre explosion, ressort, ...) qui bougent
int LittleWaveEff::nombre = 0;

LittleWaveEff::LittleWaveEff() {
	nombre++;

	type = rand()%3;	// FILL, LINE ou PT
	
	basecoul = rand() % 19;
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);

	angle1 = rand()%512;
	angle2 = rand()%512;
	angle3 = rand()%512;
	angle4 = rand()%512;

	xx2 = xy2 = x1 = x2 = y1 = y2 = 0;
}

LittleWaveEff::~LittleWaveEff() { nombre--; }


void LittleWaveEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat) {

	static int oldmode555 = 0;
	int new_type = type;

	int center_x = (nb_x>>1), center_y = (nb_y>>1);
	int rayonx = (nb_x>>2), rayony = (nb_y>>2);//, rayonx2, rayony2;	// rayon = 1/4 écran
	
	if(oldmode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	} else {
		while ((basecoul>>16) == nextcoul) 
			nextcoul = GetCoul(rand() % 19, mode555, primary);
	
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>2)+1);
	}
	
	oldmode555 = mode555;

	int coul = (basecoul>>16), acc, dr, bleu, rouge, vert;

	if(Beat) {	// on eclairci les couleurs si beat
		acc = (accel>>1);
		dr = ((mode555)?10:11);
		bleu = coul&0x1F;
		vert = ( (mode555)?((coul>>5)&0x1F):((coul>>5)&0x3F) );
		rouge = (coul>>dr);
		bleu += acc;
		vert += ((mode555)?acc:(acc<<1));
		rouge += acc;
		bleu = ((bleu>0x1F)?0x1F:bleu);
		vert = ( (mode555)?((vert>0x1F)?0x1F:vert):((vert>0x3F)?0x3F:vert) );
		rouge = ((rouge>0x1F)?0x1F:rouge);
		coul = bleu|(vert<<5)|(rouge<<dr);
	}
	else { // sinon au contraire on assombri les couleurs
		acc = (int(accel/3)?int(accel/3):1);
		dr = ((mode555)?10:11);
		bleu = coul&0x1F;
		vert = ( (mode555)?((coul>>5)&0x1F):((coul>>5)&0x3F) );
		rouge = (coul>>dr);
		bleu -= bleu/acc;
		vert -= vert/acc;
		rouge -= rouge/acc;
		bleu = ((bleu<0)?0:bleu);
		vert = ((vert<0)?0:vert);
		rouge = ((rouge<0)?0:rouge);
		coul = bleu|(vert<<5)|(rouge<<dr);
	}

	if(!coul) return;	// si couleur = noir on ne dessine rien car ça fait plutôt moche...


	// calcul des coordonées
	x1 = center_x + ((rayonx * cosinus(angle1))>>16) + ((rayonx * cosinus(angle2))>>16);
	y1 = center_y + ((rayony * sinus(angle3))>>16) + ((rayony * sinus(angle4))>>16);

	// calcul des nouveaux angles
	angle1 += (accel>>3) + rand()%(1 + (accel>>4)); //(vitesse déplacement, min = 1)
	angle2 += (accel>>2) + rand()%(1 + (accel>>3));
	angle3 += (accel>>3) + rand()%(1 + (accel>>4));
	angle4 += (accel>>2) + rand()%(1 + (accel>>3));
	angle1 &= 511; angle2 &= 511; angle3 &= 511; angle4 &= 511;

	int decx = (nb_x/12), decy = (nb_y/12);
	

	x2 = x1 + ((decx * cosinus(angle2))>>16);
	y2 = y1 + ((decx * sinus(angle4))>>16);

	
	if( (nombre>12)&&(nombre<30)&&(type == FILL) ) new_type = LINE;
	else if( (nombre>=30)&&(type != PT) ) new_type = PT;
	if( (MathTrans::enabled)&&( (MathTrans::type == ZOOMIN)||(MathTrans::type == ROTOZOOMIN)||(MathTrans::type == TUNNEL) )&&(new_type != FILL)&&(nombre < 5) ) new_type = FILL;

	ligne_graphe(screen, x1, y1, x2, y2, coul, nb_x, nb_y, wavedata, new_type);
}


// Objet CercleBass
int CercleBassEff::nombre = 0;

CercleBassEff::CercleBassEff() {
	nombre++;
	taille = petite;
	switch(nombre) {
	case 1: coefx = coefy = 0.5; taille = grande; break;
	case 2: coefx = 0.5; coefy = 0.25; break;
	case 3: coefx = 0.5; coefy = 0.74; break;
	case 4: coefx = 0.25; coefy = 0.5; break;
	case 5: coefx = 0.74; coefy = 0.5; break;
	case 6: coefx = 0.25; coefy = 0.25; break;
	case 7: coefx = 0.25; coefy = 0.74; break;
	case 8: coefx = 0.74; coefy = 0.25; break;
	default: case 9: coefx = 0.74; coefy = 0.74; break;
	}
};

CercleBassEff::~CercleBassEff() { nombre--; };

void CercleBassEff::Render(USHORT *screen, UCHAR bass, UINT nb_x, UINT nb_y, int mode555, int primary) {
	bass = (bass>128)?128:bass;

	UINT largeurpoint = (nb_x / 100);
	UINT hauteurpoint = (nb_y / 100);
	UINT ray_max_x = (nb_x  >> 2); ray_max_x *= bass; 
	if(taille != grande) ray_max_x >>= 7; else ray_max_x /= 70; ray_max_x = (ray_max_x)?ray_max_x:1;
	UINT ray_max_y = (nb_y  >> 2); ray_max_y *= bass;
	if(taille != grande) ray_max_y >>= 7; else ray_max_y /= 70; ray_max_y = (ray_max_y)?ray_max_y:1;
	UINT inca = 512; inca /= (( nb_x + nb_y ) >> 1); inca = (inca<6)?6:inca;
	UINT nbcercle = (bass >> 4); nbcercle = (nbcercle)?nbcercle:1;
	UINT centre_x = nb_x * coefx;
	UINT centre_y = nb_y * coefy;
	if((nbcercle * largeurpoint > ray_max_x) || (nbcercle * hauteurpoint > ray_max_y)) nbcercle = maximum(ray_max_x, ray_max_y);
	UINT inc_ray_x = ray_max_x / nbcercle; inc_ray_x = (inc_ray_x)?inc_ray_x:1;
	UINT inc_ray_y = ray_max_y / nbcercle; inc_ray_y = (inc_ray_y)?inc_ray_y:1;
	UINT couleur;

	
	int basecoul, nextcoul;

	basecoul = GetCoul(rand()%8, mode555, primary);
	while (basecoul == (nextcoul = GetCoul(8,mode555, primary)));
	basecoul <<= 16;
	
	//for (int angle = inca; angle < 512; angle += inca)
	for (int angle = inca; angle < 256; angle += inca)
	{  /// à débugger car dépassements mémoire lors de passage shade winamp en winamp en mode little docks !!!
			while ((basecoul>>16) == nextcoul) nextcoul = GetCoul(rand()%8, mode555, primary);
			// interpolage de la couleur de base avec la prochaine:
			basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, 12);
			couleur = (basecoul>>16);
		
		for(int x, y, rayon_x = inc_ray_x, rayon_y = inc_ray_y; (rayon_x < ray_max_x)&&(rayon_y < ray_max_y); rayon_x += inc_ray_x, rayon_y += inc_ray_y)
		{
			x = centre_x + ((rayon_x * cosinus(angle))>>16);
			y = centre_y + ((rayon_y * sinus(angle))>>16);
			for(UINT j = 0; j <= hauteurpoint; j++)
				horizontal_ligne(screen, y + j, x, x + largeurpoint, couleur, nb_x);
			y = centre_y - ((rayon_y * sinus(angle))>>16);
			for(j = 0; j <= hauteurpoint; j++)
				horizontal_ligne(screen, y + j, x, x + largeurpoint, couleur, nb_x);
		}
	}

}


//Objet Waveform
int WaveFormEff::nombre = 0;

WaveFormEff::WaveFormEff(WaveFormEff *WaveForm[MAX_WAVEFORM_EFFECTS])
{ 
	nombre++;
	
	
	switch(rand() % 2) {
	case 0: direction = VERTICALE; break;
	case 1:
	default: direction = HORIZONTALE; break;
	}

	int angver = 0; int anghor = 0;
	
	if(WaveFormEff::nombre > 1)
	{
		int nbhor = 0, nbver = 0, incangver = 0, incanghor = 0;

		for(int i = 0; i < WaveFormEff::nombre - 1; i++)
			if(WaveForm[i]->direction == HORIZONTALE) nbhor++;
			else nbver++;
		
		if(direction == VERTICALE) incangver = 512 / (nbver + 1);
		else if(nbver) incangver = 512 / nbver;
		if(direction == HORIZONTALE) incanghor = 512 / (nbhor + 1);
		else if(nbhor) incanghor = 512 / nbhor;

		for(i = 0;  i < WaveFormEff::nombre - 1; i++)
		{
			if(WaveForm[i]->direction == VERTICALE)
			{ WaveForm[i]->angle = angver; angver += incangver;}
			else
			{ WaveForm[i]->angle = anghor; anghor += incanghor;}

		}
	}

	if(direction == VERTICALE) angle = angver;
	else angle = anghor;

	switch(rand() % 3) {
	case 0: type = PT; break;
	case 1: type = LINE; break;
	case 2:
	default: type = FILL; break;
	}

	basecoul = rand() % 19;
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);
}


WaveFormEff::~WaveFormEff() { nombre--; }


void WaveFormEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat)
{
	UINT decalage;
	int temp, incangle;
	static oldmode555 = 0;

	if(oldmode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	} else {
		while ((basecoul>>16) == nextcoul) 
			nextcoul = GetCoul(rand() % 19, mode555, primary);
	
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>3)+1);//((accel- 5)<1)?1:(accel - 5));
	}
	
	oldmode555 = mode555;

	int coul = (basecoul>>16), bleu, rouge, vert, dr, acc;

	if(Beat) {	// on eclairci les couleurs si beat
		acc = (accel>>1);
		dr = ((mode555)?10:11);
		bleu = coul&0x1F;
		vert = ( (mode555)?((coul>>5)&0x1F):((coul>>5)&0x3F) );
		rouge = (coul>>dr);
		bleu += acc;
		vert += ((mode555)?acc:(acc<<1));
		rouge += acc;
		bleu = ((bleu>0x1F)?0x1F:bleu);
		vert = ( (mode555)?((vert>0x1F)?0x1F:vert):((vert>0x3F)?0x3F:vert) );
		rouge = ((rouge>0x1F)?0x1F:rouge);
		coul = bleu|(vert<<5)|(rouge<<dr);
	}
	else { // sinon au contraire on assombri les couleurs
		acc = (int(accel/3)?int(accel/3):1);
		dr = ((mode555)?10:11);
		bleu = coul&0x1F;
		vert = ( (mode555)?((coul>>5)&0x1F):((coul>>5)&0x3F) );
		rouge = (coul>>dr);
		bleu -= bleu/acc;
		vert -= vert/acc;
		rouge -= rouge/acc;
		bleu = ((bleu<0)?0:bleu);
		vert = ((vert<0)?0:vert);
		rouge = ((rouge<0)?0:rouge);
		coul = bleu|(vert<<5)|(rouge<<dr);
	}
	
	// si couleur = noir, rien à afficher ...
	if(!coul) return;

	
	//incangle =  1 + (accel>>3) + 416 / ((nb_x + nb_y) >> 1);  // vitesse deplacement selon resol (min = 6)
	incangle =  1 + (accel>>3);  // vitesse deplacement selon resol (min = 6)
	angle = (angle + incangle)&511;


	// le graphe fait le quart de la largeur (ou de la hauteur) de screen
	if(direction == VERTICALE)
	{
		temp = ((3 * nb_x)>>3);
		decalage = (temp<<16) + tabcos[angle] * temp; decalage >>= 16;
		VerticalRender(screen, nb_x, nb_y, decalage, wavedata, coul);
	}
	else
	{
		temp = ((3 * nb_y)>>3);
		decalage = (temp<<16) + tabcos[angle] * temp; decalage >>= 16;
		HorizontalRender(screen, nb_x, nb_y, decalage, wavedata, coul);
	}
}



void WaveFormEff::VerticalRender(USHORT *screen, UINT nb_x, UINT nb_y, UINT decalage, unsigned char *wavedata, int coul)
{
	float y, incy;
	unsigned int data;//, coul;
	int i, old_x;
	int axe = decalage + (nb_x >> 3); //axe = mileu du graphe
	int new_type = type;
	
	if( (nombre>8)&&(nombre<14)&&(type == FILL) ) new_type = LINE;
	else if( (nombre>=14)&&(type != PT) ) new_type = PT;
	if( (MathTrans::enabled)&&( (MathTrans::type == ZOOMIN)||(MathTrans::type == ROTOZOOMIN)||(MathTrans::type == TUNNEL) )&&(new_type != FILL)&&(nombre < 5) ) new_type = FILL;
	
	incy = 512; incy /= nb_y;
	old_x = axe; 
	//int coul = (basecoul>>16);
	
	for (i = 0, y = incy; y < 512; y += incy, i++)
	{   
		data = (unsigned)(wavedata[(int)y]^128);
		data *= nb_x; data >>= 10; data += decalage;
		
		switch(new_type) { 
		case PT : point(screen, data, i, coul, nb_x); break;
		case FILL : horizontal_ligne(screen, i, axe, data, coul, nb_x); break;
		case LINE :
		default: {horizontal_ligne(screen, i, old_x, data, coul, nb_x); old_x = data; }; break;
		}
	}
}


void WaveFormEff::HorizontalRender(USHORT *screen, UINT nb_x, UINT nb_y, UINT decalage, unsigned char *wavedata, int coul)
{
	float x, incx;
	unsigned int data;//, coul;
	int i, old_y;
	int axe = decalage + (nb_y >> 3); //axe = mileu du graphe
	int new_type = type;
	
	if( (nombre>8)&&(nombre<14)&&(type == FILL) ) new_type = LINE;
	else if( (nombre>=14)&&(type != PT) ) new_type = PT;
	if( (MathTrans::enabled)&&( (MathTrans::type == ZOOMIN)||(MathTrans::type == ROTOZOOMIN)||(MathTrans::type == TUNNEL) )&&(new_type != FILL)&&(nombre < 5) ) new_type = FILL;
	
	incx = 512; incx /= nb_x;
	old_y = axe; 
	//int coul = (basecoul>>16);

	for (i = 0, x = incx; x < 512; x += incx, i++)
	{   
		data = (unsigned)(wavedata[(int)x]^128);
		data *= nb_y; data >>= 10; data += decalage;
			
		switch(new_type) {
		case PT : point(screen, i, data, coul, nb_x); break;
		case FILL : vertical_ligne(screen, i, axe, data, coul, nb_x); break;
		case LINE :
		default: { vertical_ligne(screen, i, old_y, data, coul, nb_x); old_y = data; }; break;
		}
	}
}

int CercleWaveEff::nombre = 0;
int CercleWaveEff::sens = 0;

CercleWaveEff::CercleWaveEff(CercleWaveEff *Cercle[MAX_CERCLEWAVE_EFFECTS])
{
	nombre++;
	
	pos = nombre;
	if(nombre == 1) // première création, le cercle est au centre de l'écran
		decal = NO;
	else			// sinon il est décalé d'un rayon de cercle par rapport au centre
		decal = YES;
	
	if(nombre > 1)
	{
		int angdep = 0, incang = 512 / (nombre - 1);

		for(int i = 1;  i < nombre - 1; i++)
		{ Cercle[i]->angle = angdep; angdep += incang; }
		
		Cercle[0]->angle = 0; angle = angdep;
	}
	else
		angle = 0;
		

	switch(rand() % 3) {
	case 0: type = PT; break;
	case 1: type = LINE; break;
	case 2:
	default: type = FILL; break;
	}

	basecoul = rand() % 19;
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);
}


CercleWaveEff::~CercleWaveEff() { nombre--; }

void CercleWaveEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat)
{// foire quand au moins 3 cercles (2 sur la periph) et ResY = 9 
	int incangle;
	int new_type = ( (nombre == 1)&&( (MathTrans::enabled)&&(MathTrans::type != ROTOTUNNEL)&&(MathTrans::type != TUNNEL) ) )?FILL:type;
	static int compte_sens = 20;
	static int oldmode555 = 0;

	if(oldmode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	} else {
		while ((basecoul>>16) == nextcoul) 
			nextcoul = GetCoul(rand() % 19, mode555, primary);
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>3)+1);//((accel- 5)<1)?1:(accel - 5));
	}
	
	oldmode555 = mode555;

	int mi_y = (nb_y >> 1), mi_x = (nb_x >> 1), centre_x, centre_y;
	
	incangle =  1 + (accel>>4);  // vitesse deplacement selon resol (min = 6)
	
	if(pos==1) {
		if( (Beat)&&/*((rand()%3) > 0)&&*/(compte_sens-- <= 0) ) { 
			sens = ((sens)?0:1);
			compte_sens = 20;
		}
	}

	if(sens == 0) angle = (angle + incangle)&511;
	else angle = ((angle >= incangle)?(angle - incangle):angle + (512-incangle));
	
	centre_x = mi_x; centre_y = mi_y;
	if(decal)
	{
		centre_x += ( (((int)nb_x/3) * (cosinus(angle))) >> 16);
		centre_y += ( (((int)nb_y/3) * (sinus(angle))) >> 16);
	}
	
	if(accel) accel = (accel < 5)?10:( (accel>64)?128:(accel<<1) );	
	else /*accel = 1;*/ return;
	float i, inci, a = angle;
	///**/int i, inci, aa = (angle<<6), a = (angle<<6);
	int x, y, data;
	int hx = nb_x * accel, hy = nb_y * accel, hhx, hhy;
	hx /= 1024; hy /= 1024;	// doit être au moins > 7 * 128  sinon ça peut foirer

	if( (!hx)||(!hy) ) return;
	inci = 512;
	//inci = (512<<2);
	///**/inci = (512<<6);
	inci /= 3 * (hx + hy);

	int taille_cercle = (decal)?24:23; // 24 oblige sinon foire quand au moins 2 cercles (1 sur la periph) et ResY < 16 
	int decal_cercle = (decal)?17:16;
	//int taille_cercle = (decal)?24:( (nombre == 1)?22:23 ); // 24 oblige sinon foire quand au moins 2 cercles (1 sur la periph) et ResY < 16 
	//int decal_cercle = (decal)?17:( (nombre == 1)?15:16 );
	int coul = (basecoul>>16);
	int old_x = centre_x + ((cosinus((int)a) * hx * ((UCHAR)(wavedata[0]^128))) >> taille_cercle);
	int old_y = centre_y + ((sinus((int)a) * hy * ((UCHAR)(wavedata[0]^128))) >> taille_cercle);

	for (i = inci; i < 512; i += inci, a += inci)
	///**/for (i = inci; i<32768; i += inci, aa += inci)
	{   	
		data = (UCHAR)(wavedata[(int)i]^128);
		///**/data = (UCHAR)(wavedata[(i>>6)]^128);
		//data = 256;
		hhx = hx * data;
		hhy = hy * data;
		///**/a = (aa>>6);
		x = centre_x + ((cosinus((int)a) * hhx) >> taille_cercle);
		y = centre_y + ((sinus((int)a) * hhy) >> taille_cercle);
		///**/x = centre_x + ((cosinus(a) * hhx) >> taille_cercle);
		///**/y = centre_y + ((sinus(a) * hhy) >> taille_cercle);
		switch(new_type) {
		case PT : point(screen, x, y, coul, nb_x); break;
		case FILL :	ligne(screen, centre_x + ((cosinus((int)a) * hx) >> decal_cercle), centre_y + ((sinus((int)a) * hy) >> decal_cercle), x, y, coul, nb_x, nb_y); break;
		///**/case FILL :	ligne(screen, centre_x + ((cosinus(a) * hx) >> decal_cercle), centre_y + ((sinus((int)a) * hy) >> decal_cercle), x, y, coul, nb_x, nb_y); break;
		case LINE :	default: ligne(screen, old_x, old_y, x, y, coul, nb_x, nb_y); old_x = x; old_y = y;
		}
	}
}


int ConfettisEff::nombre = 0;

ConfettisEff::ConfettisEff()
{
	nombre++;
	randomcoul = rand() % 19;
}


ConfettisEff::~ConfettisEff() { nombre--; }

void ConfettisEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat)
{
	int taille = ((nb_x + nb_y))/70, taille_conf;
	int type = CROIX;
	int coord_x, coord_y;//, val;
	USHORT sav_couleur, r_coul, v_coul, b_coul, couleur;

	nb_y -= (taille + 2) ; //pour ne pas avoir de problème de débordement de confettis

	sav_couleur = GetCoul(randomcoul, mode555, primary);

	for(int i = 0; i < (accel>>2); i++)
	{
		int randomval = rand()%8;

		//calcul
		coord_x = rand() % nb_x;
		coord_y = rand() % nb_y;

		switch(randomval % 3) {
		case 2: type = CARRE; break;
		case 1: type = ETOILE; break;
		case 0: type = CROIX; 
		}

		taille_conf = taille / ((rand()%3) + 1);
		//taille_conf = (taille_conf<1)?1:taille_conf;
		taille_conf = (taille_conf<0)?0:taille_conf;
		int val = taille_conf/4, val2 = taille_conf/12;
		
		//gestion couleur
		r_coul = (mode555)?(sav_couleur&0x7C00):(sav_couleur&0xF800);
		v_coul = (mode555)?(sav_couleur&0x3E0):(sav_couleur&0x7E0);
		b_coul = (sav_couleur&0x1F);
		r_coul = r_coul ^ ( rand()%15 << ((mode555)?10:11) );
		v_coul = v_coul ^ ( rand()%15 << 5 );
		b_coul = b_coul ^ rand()%15;
		couleur = r_coul + b_coul + v_coul;

		// gestion selon accel
		if(Beat) {
			int bleu, rouge, vert, dr, acc = (accel>>1);
			dr = ((mode555)?10:11);
			bleu = couleur&0x1F;
			vert = ( (mode555)?((couleur>>5)&0x1F):((couleur>>5)&0x3F) );
			rouge = (couleur>>dr);
			bleu += acc;
			vert += ((mode555)?acc:(acc<<1));
			rouge += acc;
			bleu = ((bleu>0x1F)?0x1F:bleu);
			vert = ( (mode555)?((vert>0x1F)?0x1F:vert):((vert>0x3F)?0x3F:vert) );
			rouge = ((rouge>0x1F)?0x1F:rouge);
			couleur = bleu|(vert<<5)|(rouge<<dr);
			if(acc) if((rand()%acc)>4) taille_conf = taille;	// + de grandes étoiles tirant vers le blanc
		}

		//affichage		
		switch(taille_conf) {
		case 0:	point(screen, coord_x, coord_y, couleur, nb_x); break;  // un point !
		case 1:	point(screen, coord_x, coord_y, couleur, nb_x);	// un carre de 4
			point(screen, coord_x + 1, coord_y, couleur, nb_x);
			point(screen, coord_x, coord_y + 1, couleur, nb_x);
			point(screen, coord_x + 1, coord_y + 1, couleur, nb_x);
			break;
		case 2:	if(type != CROIX) { // 3 points de longueur 
			horizontal_ligne(screen, coord_y, coord_x, coord_x + 2, couleur, nb_x);
			horizontal_ligne(screen, coord_y + 1, coord_x, coord_x + 2, couleur, nb_x);
			horizontal_ligne(screen, coord_y + 2, coord_x, coord_x + 2, couleur, nb_x);
				}
			else {
			horizontal_ligne(screen, coord_y + 1, coord_x, coord_x + 2, couleur, nb_x);
			vertical_ligne(screen, coord_x + 1, coord_y, coord_y + 2, couleur, nb_x);
			}; break;
		default: switch(type) { 
			case CARRE:
				{
					for(int j = 0; j < taille_conf/2; j++)
						horizontal_ligne(screen, coord_y + j, coord_x + (taille_conf / 2) - j, coord_x + j, couleur, nb_x);
					for(int k = taille_conf/2; k <= taille_conf; k++)
						horizontal_ligne(screen, coord_y + k, coord_x + k - (taille_conf / 2), coord_x + taille_conf - k, couleur, nb_x);
				} break;
			case ETOILE:{ int k;
					for(k = -val2; k <= val2; k++)
						ligne(screen, coord_x + val + k, coord_y + val, coord_x + taille_conf - val + k, coord_y + (taille_conf - 1 - val), couleur, nb_x, nb_y);
					for(k = -val2; k <= val2; k++)
						ligne(screen, coord_x - val + k + taille_conf, coord_y + val, coord_x + val + k, coord_y + (taille_conf - val - 1), couleur, nb_x, nb_y); 
						}
			case CROIX: { int k;
					for(k = -val2; k <= val2; k++)
						horizontal_ligne(screen, coord_y + (taille_conf>>1) + k, coord_x , coord_x + taille_conf, couleur, nb_x);
					for(k = -val2; k <= val2; k++)
						vertical_ligne(screen, coord_x + (taille_conf>>1) + k, coord_y, coord_y + taille_conf, couleur, nb_x);
						}
				 }
		}
	}

}


int MathShapeEff::nombre = 0;

MathShapeEff::MathShapeEff() : pts(NULL), angle(0), angle2(0), incang2(1), incang(0), /*type(rand()%4),*/ angle_deb(0), angle2_deb(0), diam_x(0), diam_y(0), coef_a(0), coef_b(0), inc_a(0), inc_b(0), old_nb_x(0), old_nb_y(0), old_mode555(0), inc_d(0), compteur(0) {
	nombre++;

	basecoul = rand() % 19;
	//nb_coul = basecoul = (rand()%19);
	do nextcoul = rand() % 19; while(basecoul == nextcoul);
	basecoul = GetCoul(basecoul, 0, 1); basecoul <<= 16;
	nextcoul = GetCoul(nextcoul, 0, 1);
	//nextcoul = GetCoul((nb_coul + 1)%19, 0);
}


MathShapeEff::~MathShapeEff() { 
	nombre--;
	delete[] pts;
}
	
void MathShapeEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat) {
	static int center_x = 0, center_y = 0;
	static int coef_x = 0, coef_y = 0;
	static UINT nb_point = 0, taille_point = 0;
	UINT i;
	int coul;
	

	// avant ça crashait si cété pas multiplié par 2  à cause de taille_point apparement
	//accel <<= 1;
	/**/accel *= 1.5;

	accel = ((accel>128)?128:accel);
	
	//gestion couleur
	if(old_mode555 != mode555)  {
		basecoul = rand() % 19;
		do nextcoul = rand() % 19; while(basecoul == nextcoul);
		basecoul = GetCoul(basecoul, mode555, primary); basecoul <<= 16;
		nextcoul = GetCoul(nextcoul, mode555, primary);
	}
	else {
		while ((basecoul>>16) == nextcoul) nextcoul = GetCoul(rand() % 19, mode555, primary);
		
		// interpolage de la couleur de base avec la prochaine:
		basecoul = GetInterpoledCoul(basecoul, nextcoul, mode555, (accel>>3)+1);
	}
	old_mode555 = mode555;

	coul = (basecoul>>16);
		
	if(Beat) {	// on eclairci les couleurs si beat
		int bleu, rouge, vert, dr, acc = (accel>>2);
		dr = ((mode555)?10:11);
		bleu = coul&0x1F;
		vert = ( (mode555)?((coul>>5)&0x1F):((coul>>5)&0x3F) );
		rouge = (coul>>dr);
		bleu += acc;
		vert += ((mode555)?acc:(acc<<1));
		rouge += acc;
		bleu = ((bleu>0x1F)?0x1F:bleu);
		vert = ( (mode555)?((vert>0x1F)?0x1F:vert):((vert>0x3F)?0x3F:vert) );
		rouge = ((rouge>0x1F)?0x1F:rouge);
		coul = bleu|(vert<<5)|(rouge<<dr);
	}

	//initialisation
	if( (old_nb_x != nb_x)||(old_nb_y != nb_y)||(compteur > 750) ) {
		create = FALSE;
		type = rand()%4;
		compteur = 0;
		// en dessous de 200 ça peut crasher ??? (à 175 ca crashe ...)
		//taille_point = 1 + ( ((nb_x + nb_y)>>2) + ((nb_x + nb_y)>>2) )/100;
		taille_point = 1 + ( ((nb_x + nb_y)>>1) )/100;
		//taille_point = 1 + ( ((nb_x + nb_y)>>2) + ((nb_x + nb_y)>>2) )/200;
		//taille_point = 1;
		center_x = (nb_x >> 1) - taille_point;
		center_y = (nb_y >> 1) - taille_point;
		nb_point = 30 + ((center_x + center_y)>>1);
		coef_x = ((center_x - taille_point)>>1);
		coef_y = ((center_y - taille_point)>>1);
		incang = 512; incang /= nb_point;
		inc_a = inc_b = 1;
		inc_d = inc_d2 = 1;
		angle_deb = rand() % 512;
		angle2_deb = rand() % 512;
		coef_b = rand()%3; coef_a = rand()%3;
			
		if(pts) { delete[] pts; }
		pts = new pt[nb_point];
	}

	compteur++;	old_nb_x = nb_x; old_nb_y = nb_y;

	//calcul
	switch(type) {
	case 0:	// un tortillon
		diam_x = coef_x + absolue(((coef_x * (cosinus(angle_deb)))>>16)); 
		diam_y = coef_y + absolue(((coef_y * (cosinus(angle2_deb)))>>16)); 
		for(angle = angle_deb, i = 0; i < nb_point; angle += incang, i++)
		{
			pts[i].x = center_x + ( (((diam_x * cosinus((int)angle)) >> 16) * sinus(int(angle * coef_a/2))) >> 16);
			pts[i].y = center_y + ( (((diam_y * sinus((int)angle)) >> 16) * cosinus(int(angle * coef_b/2))) >> 16);
		}
		break;

	case 1:	// un autre tortillon
		for(angle = angle_deb, i = 0; i < nb_point; angle += incang, i++)
		{
			int tmp = cosinus(int(angle * coef_a/2));
			pts[i].x = center_x + (( ((coef_x * cosinus(int(angle))) >> 15) * tmp) >> 16);
			pts[i].y = center_y + (( ((coef_y * sinus(int(angle))) >> 16) * tmp) >> 16) + (((coef_y) * sinus(int(angle * coef_b/2))) >> 16);
		}
		break;
	case 2:	// une ondulation sympa
		diam_x = (coef_x + absolue(((coef_x * (cosinus(angle_deb)))>>16)))/2; 
		diam_y = (coef_y + absolue(((coef_y * (cosinus(angle2_deb)))>>16)))/2; 
		for(angle = angle_deb, i = 0; i < nb_point; angle += incang, i++)
		{
			pts[i].x = center_x + ((diam_x * cosinus((int)angle)) >> 16) + (((coef_x>>1) * cosinus(int(angle * coef_a))) >> 16);
			pts[i].y = center_y + ((diam_y * sinus((int)angle)) >> 16) + (((coef_y>>1) * sinus(int(angle * coef_b))) >> 16);
		}
		break;
	default: case 3:	// une autre forme sympa moins vivante mais plus reactive
		diam_x = (coef_x + absolue(((coef_x * (cosinus(angle_deb)))>>16)))/2; 
		diam_y = (coef_y + absolue(((coef_y * (cosinus(angle2_deb)))>>16)))/2; 
		for(angle = angle_deb, angle2 = angle2_deb, i = 0; i < nb_point; angle += incang, angle2 += (accel/300), i++)
		{
			pts[i].x = center_x + ((diam_x * cosinus((int)angle)) >> 16) + (((coef_x>>2) * sinus(int(angle * coef_b))) >> 16) + (((coef_x>>2) * cosinus(int(angle2))) >> 16);
			pts[i].y = center_y + ((diam_y * sinus((int)angle)) >> 16) + (((coef_y>>2) * cosinus(int(angle * coef_a))) >> 16) + (((coef_y>>2) * sinus(int(angle2))) >> 16);
		}
	}

	if(inc_a == 1) { float var = accel + rand()%10; var /= 3000; coef_a += var; }
	else { float var = -accel - rand()%10; var /= 3000; coef_a += var; }
	if(coef_a > 2.5) { inc_a = -inc_a; coef_a = 2.5;}
	if(coef_a < -2.5) { inc_a = -inc_a; coef_a = -2.5;}
	if(inc_b == 1) { float var = accel + rand()%50; var /= 4000; coef_b += var; }
	else { float var = -accel - rand()%50; var /= 3000; coef_b += var; }
	if(coef_b > 2.5) { inc_b = -inc_b; coef_b = 2.5;}
	if(coef_b < -2.5) { inc_b = -inc_b; coef_b = -2.5;}
	angle_deb += (inc_d == 1)?(accel>>2) + rand()%5:-(accel>>2) - rand()%5;
	if(angle_deb > 511) { inc_d = -inc_d; angle_deb = 511;}
	if(angle_deb < 0) { inc_d = -inc_d; angle_deb = 0;}
	angle2_deb += (inc_d2 == 1)?(accel>>2) + rand()%5:-(accel>>2) - rand()%5;
	if(angle2_deb > 511) { inc_d2 = -inc_d2; angle2_deb = 511;}
	if(angle2_deb < 0) { inc_d2 = -inc_d2; angle2_deb = 0;}
	
	//affichage
	i = -1;

	/**/while(++i < nb_point) point_taille(screen, pts[i].x, pts[i].y, taille_point, coul, nb_x, nb_y);
	
	/*
	switch(taille_point) {
	case 1: 
		while(++i < nb_point) 
			point(screen, pts[i].x, pts[i].y, coul, nb_x);
		break;
	case 2:
		while(++i < nb_point) {
			point(screen, pts[i].x, pts[i].y, coul, nb_x);
			point(screen, pts[i].x, pts[i].y + 1, coul, nb_x);
			point(screen, pts[i].x + 1, pts[i].y, coul, nb_x);
			point(screen, pts[i].x + 1, pts[i].y + 1, coul, nb_x);
		}
		break;
	default: case 3: 
		while(++i < nb_point) 
			for(int j = 0; j < taille_point; j++)
				horizontal_ligne(screen, pts[i].y + j, pts[i].x, pts[i].x + taille_point, coul, nb_x);
	}
	*/
}



	
//objet RotaLineEff
int RotaLineEff::nombre = 0;
int RotaLineEff::sens = 1;

RotaLineEff::RotaLineEff(RotaLineEff *Rota[MAX_ROTALINE_EFFECTS])
{
	nombre++;
	
	/**/pos = nombre;
	
	if(nombre > 1) {
		int angdep = 0, incang = 512 / (nombre);

		for(int i = 0;  i < nombre - 1; i++) {
			Rota[i]->angle = angdep;
			angdep += incang;
		}
		angle = angdep;
	}
	else angle = 0;
		
	coul1 = rand() % 19;
	do nextcoul1 = rand() % 19; while(coul1 == nextcoul1);
	do coul2 = rand() % 19; while(coul1 == coul2);
	do nextcoul2 = rand() % 19; while(coul2 == nextcoul2);
	coul1 = GetCoul(coul1, 0, 1); coul1 <<= 16;
	coul2 = GetCoul(coul1, 0, 1); coul2 <<= 16;
	nextcoul1 = GetCoul(nextcoul1, 0, 1);
	nextcoul2 = GetCoul(nextcoul2, 0, 1);
}

RotaLineEff::~RotaLineEff() { nombre--; }

void RotaLineEff::Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat) {
	int type;
	static int oldmode555 = 0;
	static int compte_sens = 20;

	if(nombre < 6) type = FILL;
	else if(nombre < 11) type = LINE;
	else type = PT;

	accel = ((accel)?accel:1);
	
	if(oldmode555 != mode555)  {
		coul1 = rand() % 19;
		do nextcoul1 = rand() % 19; while(coul1 == nextcoul1);
		do coul2 = rand() % 19; while(coul1 == coul2);
		do nextcoul2 = rand() % 19; while(coul2 == nextcoul2);
		coul1 = GetCoul(coul1, mode555, primary); coul1 <<= 16;
		coul2 = GetCoul(coul1, mode555, primary); coul2 <<= 16;
		nextcoul1 = GetCoul(nextcoul1, mode555, primary);
		nextcoul2 = GetCoul(nextcoul2, mode555, primary);
	} else {
		if((coul1>>16) == nextcoul1) 
			while( ((coul1>>16) == nextcoul1)||(nextcoul2 == nextcoul1) )
				nextcoul1 = GetCoul(rand() % 19, mode555, primary);
		if((coul2>>16) == nextcoul2)
			while( ((coul2>>16) == nextcoul2)||(nextcoul1 == nextcoul2) )
				nextcoul2 = GetCoul(rand() % 19, mode555, primary);
		// interpolage de la couleur de base avec la prochaine:
		coul1 = GetInterpoledCoul(coul1, nextcoul1, mode555, (accel>>3)+1);
		coul2 = GetInterpoledCoul(coul2, nextcoul2, mode555, (accel>>3)+1);
	}
	
	oldmode555 = mode555;

	int mi_y = (nb_y >> 1), mi_x = (nb_x >> 1);
	
	//angle = (angle + (1 + (accel>>3) + 416 / ((nb_x + nb_y) >> 1)) )&511;
	int incangle = (1 + (accel>>2));
	if(pos==1) {
//		if( (accel > 70)&&/*((rand()%3) > 0)&&*/(compte_sens-- <= 0) ) { 
		if( (Beat)&&/*((rand()%3) > 0)&&*/(compte_sens-- <= 0) ) { 
			sens = ((sens)?0:1);
			compte_sens = 10;
		}
	}
	if(sens == 0) angle = (angle + incangle)&511;
	else angle = ((angle >= incangle)?(angle - incangle):angle + (512-incangle));
	

	float i, inci = 512;
	int x, y, data, d, old_x = mi_x, old_y = mi_y;//, incc;
	int couleur = (coul1>>16), couleur2 = (coul2>>16);

	inci /= min(nb_x, nb_y);	
	//inci *= 2;
	/**/inci *= ( ((90/accel)<2)?2:(90/accel) );

	
	USHORT rougecoul = (mode555)?(couleur>>10):(couleur>>11);
	USHORT vertcoul = (mode555)?(couleur>>5)&0x1F:(couleur>>5)&0x3F;
	USHORT bleucoul = couleur&0x1F;
	USHORT rougedestcoul = (mode555)?(couleur2>>10):(couleur2>>11);
	USHORT vertdestcoul = (mode555)?(couleur2>>5)&0x1F:(couleur2>>5)&0x3F;
	USHORT bleudestcoul = couleur2&0x1F;
		
	SHORT dvert =  vertdestcoul - vertcoul;
	SHORT drouge = rougedestcoul - rougecoul;
	SHORT dbleu = bleudestcoul - bleucoul;
		
	float nbinc = 512 / inci;
/*	int comptvert = int(nbinc + 1), comptrouge = int(nbinc + 1), comptbleu = int(nbinc + 1);
	
	if(dvert != 0) comptvert = int(nbinc / ( absolue(dvert) >> 5 ) );
	if(drouge != 0) comptrouge = int(nbinc / ( absolue(drouge) >> ((mode555)?10:11) ) );
	if(dbleu != 0) comptbleu = int(nbinc / absolue(dbleu) );
*/	int comptvert = int(nbinc / ( (absolue(dvert) >> 5) + 1 ) );
	int comptrouge = int(nbinc / ( (absolue(drouge) >> ((mode555)?10:11)) + 1 ) );
	int comptbleu = int(nbinc / (absolue(dbleu) + 1) );
		  
	int rouge = (mode555)?(1<<10):(1<<11);
	int vert = (1<<5);
	
	/**
	comptvert /= 2;
	comptrouge /= 2;
	comptbleu /= 2;
	/**/

	int cv = comptvert, cr = comptrouge, cb = comptbleu;
	
	for (d = 0, i = inci ;i < 512; i += inci, d++)
	{   	
		if(cr-- <= 0) {
			cr = comptrouge;
			if(drouge < 0) couleur -= rouge;
			else couleur += rouge;
		}

		if(cv-- <= 0) {
			cv = comptvert;
			if(dvert < 0) couleur -= vert;
			else couleur += vert;
		}

		if(cb-- <= 0) {
			cb = comptbleu;
			if(dbleu < 0) couleur--;
			else couleur++;
		}

		data = (UCHAR)(wavedata[int(i)]^128);
		//data = 0;
		x = mi_x + ((cosinus(angle + (data>>1)) * d)>>16);
		y = mi_y + ((sinus(angle + (data>>1)) * d)>>16);
		switch(type) {
		case PT : point(screen, x, y, couleur, nb_x); break;
		case FILL :	ligne(screen, mi_x + ((cosinus(angle + 64) * d)>>16), mi_y + ((sinus(angle + 64) * d)>>16), x, y, couleur, nb_x, nb_y); break;
		case LINE :	default: ligne(screen, old_x, old_y, x, y, couleur, nb_x, nb_y); old_x = x; old_y = y;
		}
	}
}
