#include "arpeggio.h"

// random generator function:
int myrandom (int i)
{
    return std::rand()%i;
}

unsigned char arpeggio::getNextNote(unsigned *root_change, unsigned *root_note, unsigned *played)
{
    if (arp_sequence.size() > 0)
    {
        unsigned char n = arp_sequence[current_note];
        *root_change = !(current_note % steps);
        *root_note = chords[current_note / steps].root_note + root_octave*12;

        if (bj_steps.sequence.size() > 0)
            *played = bj_steps.getSequence((current_note+pulse_shift) % steps);
        else
            *played = 1;

        if (current_note == arp_sequence.size()-1)
            current_note = 0;
        else
            current_note++;
        return n;
    }
    else
        return 0;
}
void arpeggio::addChord(chord chord_add)
{
    chords.push_back(chord_add);
    addChordToSeq(chord_add);
}

void arpeggio::addChordToSeq(chord chord_add)
{
    unsigned int i, j, note_num=0, oct_shift = 0;
    unsigned int root_note = 0;
    unsigned int base_chord_notes[4];
    vector<unsigned char>tmp_sequence;

    root_note = midiByNote(chord_add.root_note, seq_octave);        // get the MIDI value for the root note
    createChord(root_note, chord_add.chord_type, base_chord_notes); // get 4 MIDI values for the current chord

    unsigned up_notes = steps/2;
    unsigned down_notes = steps - up_notes;

    switch (style)
    {
    case up:
        for (j=0; j<steps; j++)
        {
            note_num = j % 4;
            oct_shift = j / 4;
            arp_sequence.push_back(base_chord_notes[note_num] + (12 * oct_shift));
        }
        break;
    case down:
        for (j=0; j<steps; j++)
        {
            note_num = j % 4;
            oct_shift = j / 4;
            tmp_sequence.push_back(base_chord_notes[note_num] + (12 * oct_shift));
        }
        arp_sequence.insert(arp_sequence.end(), tmp_sequence.rbegin(), tmp_sequence.rend());
        break;
    case updown:
        for (j=0; j<up_notes+1; j++)
        {
            note_num = j % 4;
            oct_shift = j / 4;
            arp_sequence.push_back(base_chord_notes[note_num] + (12 * oct_shift));
        }

        for (j=0; j<down_notes-1; j++)
        {
            note_num = j % 4;
            oct_shift = j / 4;
            tmp_sequence.push_back(base_chord_notes[note_num] + (12 * oct_shift));
        }
        arp_sequence.insert(arp_sequence.end(), tmp_sequence.rbegin(), tmp_sequence.rend());
        break;
    case randomized:
        for (j=0; j<steps; j++)
        {
            note_num = j % 4;
            oct_shift = j / 4;
            tmp_sequence.push_back(base_chord_notes[note_num] + (12 * oct_shift));
        }
        random_shuffle(tmp_sequence.begin(), tmp_sequence.end(), myrandom);
        arp_sequence.insert(arp_sequence.end(), tmp_sequence.begin(), tmp_sequence.end());
        break;
    }
}

void arpeggio::rebuildSequnce()
{
    unsigned int i;
    arp_sequence.clear();
    arp_sequence.reserve(chords.size() * steps); // Reserve vector capacity for required notes amount
    current_note = 0;
    for (i=0; i<chords.size(); i++)
        addChordToSeq(chords[i]);

    bj_steps.clear();
    //if (steps > 0)
    //  bj_steps.init(steps, bj_steps);
}

void arpeggio::clearSeq()
{
    chords.clear();
    arp_sequence.clear();
}

arpeggio::arpeggio()
{
    steps = 0;
    style = up;
    root_octave = seq_octave = 0;
    chords.clear();
    arp_sequence.clear();
    bj_steps.clear();
    current_note = 0;
    pulse_shift = 0;
    std::srand ( unsigned ( std::time(0) ) );
}

void arpeggio::setSteps(unsigned user_steps)
{
    if (steps < ARP_MAX_STEPS)
    {
        steps = user_steps;
        rebuildSequnce();
    }
}
void arpeggio::setSeqOctave(unsigned octave)
{
    if (octave < 8)
    {
        seq_octave = octave;
        rebuildSequnce();
    }
}
void arpeggio::setStyle(arp_style s)
{
    style = s;
    rebuildSequnce();
}

void arpeggio::setRootOctave(unsigned octave)
{
    if (octave < 8)
    {
        root_octave = octave;
    }
}

void arpeggio::setPulse(unsigned p)
{
    bj_steps.clear();
    //if ((p>0)&&(steps>0))
        bj_steps.init(steps, p);
}

void arpeggio::setPulseShift(unsigned p)
{
    pulse_shift = p;
}
