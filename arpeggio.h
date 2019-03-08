#ifndef ARPEGGIO_H_INCLUDED
#define ARPEGGIO_H_INCLUDED

#include <ArduinoSTL.h>
#include <vector>
#include <cstdlib>      // std::rand, std::srand
#include <ctime>
#include "Bjorklund.h"
#include "musical.h"

#define ARP_MAX_STEPS 32

using namespace std;

typedef enum {up, down, updown, randomized} arp_style;

class arpeggio {

public:
    unsigned char getNextNote(unsigned *root_change, unsigned *root_note, unsigned *played);
    void addChord(chord chord_add);
    void setSteps(unsigned user_steps);
    void setSeqOctave(unsigned octave);
    void setRootOctave(unsigned octave);
    void setStyle(arp_style s);
    void setPulse(unsigned p);
    void setPulseShift(unsigned p);
    void clearSeq();
    arpeggio();
//private:
    // Arpeggio configuration
    int steps;
    arp_style style;
    vector<chord> chords;
    int root_octave, seq_octave;

    //state machine
    int current_note, last_root_note;

    // Arpeggio sequence
    vector<unsigned char>arp_sequence;
    // Bjorklund part:
    unsigned pulse_shift;
    Bjorklund bj_steps;

    // internal methods
    void addChordToSeq(chord chord_add);
    void rebuildSequnce();

};
#endif // ARPEGGIO_H_INCLUDED
