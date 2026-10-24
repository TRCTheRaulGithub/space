// notes:
// any window that remains in foreground should optimally pass 
// keystrokes to the parent (winamp's) window, so that the user
// can still control it. unless escape is hit, or some option 
// key specific to the vis is hit. As for storing configuration,
// Configuration data should be stored in <dll directory>\plugin.ini
// Look at the example plugin for a framework.

// ints are 32 bits, and structure members are aligned on the default 8 byte boundaries
// tested with VC++ 4.2 and 5.0

#ifndef _VIS_H_
#define _VIS_H_

typedef struct winampVisModule {
  char *description; // description du module
  HWND hwndParent;   // fenêtre parent (remplie par winamp)
  HINSTANCE hDllInstance; // instance utilisant ce DLL (remplie par winamp)
  int sRate;		 // taux d'echantillonage (remplie par l'application appelante)
  int nCh;			 // nombre de canaux (remplie par l'application appelante)
  int latencyMs;     // latency from call of RenderFrame to actual drawing
                     // (l'appliaction appelante regarde cette valeur quand il obtient les données)
  int delayMs;       // delai entre appels en ms

  // les données sont remplies respectivement au Nch entrée
  int spectrumNch;
  int waveformNch;
  unsigned char spectrumData[2][576];
  unsigned char waveformData[2][576];

  void (*Config)(struct winampVisModule *this_mod);  // configuration
  int (*Init)(struct winampVisModule *this_mod);     // 0 si succes, (crée la fenêtre, etc )
  int (*Render)(struct winampVisModule *this_mod);   // retourne 0 si succes, 1 si vis doit terminer
  void (*Quit)(struct winampVisModule *this_mod);    // appelé quand vis terminé

  void *userData; // données utilisateur, optionnel
} winampVisModule;

typedef struct {
  int version;       // VID_HDRVER
  char *description; // description de la bibliothèque (library)
  winampVisModule* (*getModule)(int);
} winampVisHeader;

// symboles exportées
typedef winampVisHeader* (*winampVisGetHeaderType)();

// version du module courant (0x101 == 1.01)
#define VIS_HDRVER 0x001

#endif  // _VIS_H_