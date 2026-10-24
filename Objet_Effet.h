#ifndef _EFFET_H_
#define _EFFET_H_

#include "graph.h"
#include "objet_transformation.h"		// pour connaitre éventuellement les transformations en cours
#include "stdlib.h"
#include <assert.h>

#define MAX_WAVEFORM_EFFECTS 20
#define MAX_CERCLEWAVE_EFFECTS 10
#define MAX_CERCLEBASS_EFFECTS 9
#define MAX_CONFETTIS_EFFECTS 10
#define MAX_MATHSHAPE_EFFECTS 10
#define MAX_ROTALINE_EFFECTS 15
#define MAX_LITTLEWAVE_EFFECTS 50
#define MAX_LITTLEMATH_EFFECTS 10
#define MAX_GEOGRAPH_EFFECTS 8

#define HORIZONTALE 1
#define VERTICALE 0

#define ALL_SCREEN 0
#define BORDER_SCREEN 1
#define CENTER_SCREEN 2

#define CARRE 0
#define ETOILE 1
#define CROIX 2

typedef struct pt_ {
	int x;
	int y;
} pt;

	
//extern bool OnBassChange;

// retourne la couleur correspondante au nombre (entre 0 et 18) et au mode
UINT GetCoul(int number, int mode555, int primary);
// retourne la couleur interpolée / destcoul
UINT GetInterpoledCoul(UINT coul, UINT destcoul, int mode555, int inc);

class WaveFormEff {
	int type, direction, angle;
	UINT basecoul, nextcoul;
	void VerticalRender(USHORT *screen, UINT nb_x, UINT nb_y, UINT decalage, unsigned char *wavedata, int coul);
	void HorizontalRender(USHORT *screen, UINT nb_x, UINT nb_y, UINT decalage, unsigned char *wavedata, int coul);
public :
	static int nombre;
	WaveFormEff(WaveFormEff *WaveForm[MAX_WAVEFORM_EFFECTS]);
	~WaveFormEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat);
};

class CercleWaveEff {
	UINT pos, type, angle, decal, basecoul, nextcoul;
public :
	static int nombre;
	static int sens;	// sens de rotation des cercles
	CercleWaveEff(CercleWaveEff *CercleWave[MAX_CERCLEWAVE_EFFECTS]);
	~CercleWaveEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat);
};

class RotaLineEff {
	UINT angle, coul1, coul2, nextcoul1, nextcoul2, pos;	// ligne en dégradé de coul1 vers coul2
public :
	static int nombre;
	static int sens;	// sens de rotation des lignes
	RotaLineEff(RotaLineEff *Rota[MAX_ROTALINE_EFFECTS]);
	~RotaLineEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat);
};

class CercleBassEff {
	float coefx, coefy;
	enum {blanc, rouge, bleu} coul;
	enum {grande, petite} taille;
public :
	static int nombre;
	CercleBassEff();
	~CercleBassEff();
	void Render(USHORT *screen, UCHAR bass, UINT nb_x, UINT nb_y, int mode555, int primary);
};

class ConfettisEff {
	int randomcoul;
public:
	static int nombre;
	ConfettisEff();
	~ConfettisEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat);
};

class MathShapeEff {
	UINT basecoul, nextcoul;
	pt* pts;	//tableau de point
	float angle, angle2, incang, incang2, coef_a, coef_b, inc_a, inc_b, inc_d, inc_d2, old_nb_x, old_nb_y, old_mode555;
	int type, angle_deb, angle2_deb, diam_x, diam_y, create, compteur/*, nb_coul*/;
public :
	static int nombre;
	MathShapeEff();
	~MathShapeEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat);
};

class LittleWaveEff {
	UINT basecoul, nextcoul, type;
	UINT angle1, angle2, angle3, angle4;
	int x1, y1, x2, y2, xx2, xy2;
public :
	static int nombre;
	LittleWaveEff();
	~LittleWaveEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, unsigned char *wavedata, int mode555, int accel, int primary, bool Beat);
};

class LittleMathEff {
	UINT basecoul, nextcoul;
	int x, y;
	int style, counter;	// au bout d'un moment, on change d'effet
public :
	static int nombre;
	LittleMathEff();
	~LittleMathEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat);
};

class GeoGraphEff {
	UINT basecoul, nextcoul;
	int anglecentre, angleeff;
	int style, counter, counter2, old_accel;	
	bool decal, gauche;
public :
	static int nombre;
	GeoGraphEff(GeoGraphEff *GeoGraph[MAX_GEOGRAPH_EFFECTS]);
	~GeoGraphEff();
	void Render(USHORT *screen, UINT nb_x, UINT nb_y, int mode555, int accel, int primary, bool Beat);
};


#endif