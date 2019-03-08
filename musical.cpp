#include "musical.h"

void createChord(unsigned int root_note, chord_types type, unsigned int *notes)
{
    notes[0] = root_note;
    switch (type) {
        case maj:   notes[1] = root_note + 4; notes[2] = root_note + 7; notes[3] = root_note + 11; break;
        case minor: notes[1] = root_note + 3; notes[2] = root_note + 7; notes[3] = root_note + 10; break;
        case dim:   notes[1] = root_note + 3; notes[2] = root_note + 6; notes[3] = root_note + 9;  break;
        case aug:   notes[1] = root_note + 4; notes[2] = root_note + 8; notes[3] = root_note + 10; break;
    }

}

unsigned int  midiByNote (notes note, int octave)
{
    if ((octave < -1)||(octave > 8))
        return -1;

    return 24 + (octave)*12 + (int)note;
}
