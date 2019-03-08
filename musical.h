#ifndef MUSICAL_H_INCLUDED
#define MUSICAL_H_INCLUDED
using namespace std;

typedef enum {maj, minor, dim, aug} chord_types;
typedef enum {C, CD, D, DD, E, F, FD, G, GD, A, AD, B} notes;

typedef struct {
    notes root_note;
    chord_types chord_type;
} chord;

void createChord(unsigned int root_note, chord_types type, unsigned int *notes);
unsigned int  midiByNote (notes note, int octave);

#endif // MUSICAL_H_INCLUDED
