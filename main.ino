#include <TimerOne.h>
#include <MIDI.h>
#include "arpeggio.h"
#include "beats.h"
MIDI_CREATE_DEFAULT_INSTANCE();
#define rootOctavePin 0
#define seqOctavePin 5
#define octaveShiftpin 1
#define stepsPin 4
#define orderpin 7
#define modePin 6
#define pulsePin 3

#define LEDPin 13
#define syncinpin 3

#define buttonTimeout 3

struct st_machine {
  unsigned int steps;
  int root_octave, seq_octave;
  int pulse;
  int pulse_shift;
  arp_style a_mode;
} state_machine;

arpeggio a;

unsigned buttons_wait[7];
unsigned buttons_pressed[7];

#define max_poti_val 790
int cutByPotiLimit(int X) { return ((X) < (max_poti_val) ? (X) : (max_poti_val)); }

void addChordbyButton(int button, int intchord)
{
  bool wrong_chord = false;
  chord c = {C, maj};
  if ((intchord == 0)||(intchord == 2))
    c.chord_type = maj;
  else  if ((intchord == 1)||(intchord == 3))
    c.chord_type = minor;
  switch (button)
  {
    case 0: if ((intchord == 0)||(intchord == 1)) c.root_note = C; else if ((intchord == 2)||(intchord == 3)) c.root_note = CD; break;
    case 1: if ((intchord == 0)||(intchord == 1)) c.root_note = D; else if ((intchord == 2)||(intchord == 3)) c.root_note = DD; break;
    case 2: if ((intchord == 0)||(intchord == 1)) c.root_note = E; else if ((intchord == 2)||(intchord == 3)) wrong_chord = true; break;
    case 3: if ((intchord == 0)||(intchord == 1)) c.root_note = F; else if ((intchord == 2)||(intchord == 3)) c.root_note = FD; break;
    case 4: if ((intchord == 0)||(intchord == 1)) c.root_note = G; else if ((intchord == 2)||(intchord == 3)) c.root_note = GD; break;
    case 5: if ((intchord == 0)||(intchord == 1)) c.root_note = A; else if ((intchord == 2)||(intchord == 3)) c.root_note = AD; break;
    case 6: if ((intchord == 0)||(intchord == 1)) c.root_note = B; else if ((intchord == 2)||(intchord == 3)) wrong_chord = true; break;
  }
  if (wrong_chord == false)
    a.addChord(c);
}

void updateControls()
{
  int a_read, mapped, i, button;

  a_read = cutByPotiLimit(analogRead(seqOctavePin));
  mapped = map(a_read, 0, max_poti_val, 0, 8);
  if (mapped != state_machine.seq_octave)
  {
    state_machine.seq_octave = mapped;
    a.setSeqOctave(mapped);
    //cout << "seqOctavePin " << mapped << endl;
  }
  
 /*a_read = analogRead(rootOctavePin);
  mapped = map(a_read, 0, max_poti_val, 0, 8);
  if (mapped != state_machine.root_octave)
  {
    state_machine.root_octave = mapped;
    a.setRootOctave(mapped);
    //cout << "rootOctavePin " << mapped << endl;
  }*/

  a_read = cutByPotiLimit(analogRead(stepsPin));
  mapped = map(a_read, 0, max_poti_val, 0, 12);
  if (mapped != state_machine.steps)
  {
    state_machine.steps = mapped;
    a.setSteps(mapped);
    //cout << "stepsPin " << mapped << endl;
  }

  a_read = cutByPotiLimit(analogRead(pulsePin));
  mapped = map(a_read, 0, max_poti_val, 1, state_machine.steps);
  if (mapped != state_machine.pulse)
  {
    state_machine.pulse = mapped;
    a.setPulse(mapped);
    //cout << "pulsePin " << mapped << endl;
  }

  
  a_read = cutByPotiLimit(analogRead(modePin));
  mapped = map(a_read, 0, max_poti_val, 0, 3);
  if (mapped != state_machine.a_mode)
  {
    state_machine.a_mode = mapped;
    a.setStyle((arp_style) mapped);
    //cout << "modePin " << mapped << endl;
  }

  a_read = cutByPotiLimit(analogRead(orderpin));
  mapped = map(a_read, 0, max_poti_val, 0, state_machine.steps-1);
  if (mapped != state_machine.pulse_shift)
  {
    state_machine.pulse_shift = mapped;
    a.setPulseShift(mapped);
    //cout << "orderpin " << mapped << endl;
  }

  // In my setup the buttons are connected to pins 6..12
  for (i=12;i>5;i--)
  {
    button = 12-i;
    if (!(digitalRead(i))) { 
      if (buttons_pressed[button] == 0)
      {
        buttons_pressed[button] = 1;
      } 
      else {
        if (buttons_wait[button] > 0)
          buttons_wait[button]--;
        if (buttons_wait[button] == 1)
        {
          a_read = cutByPotiLimit(analogRead(octaveShiftpin));
          mapped = map(a_read, 0, max_poti_val, 0, 4);
          if (mapped == 4) {
            a.clearSeq();
            return;
          }
          digitalWrite(LEDPin, HIGH);
          addChordbyButton(button, mapped);
        }
      }
    } else {
      if (buttons_pressed[button] == 1)
        digitalWrite(LEDPin, LOW);
      buttons_pressed[button] = 0;
      buttons_wait[button] = buttonTimeout;
    }
  }
}

void setup() {
  int i;
  Timer1.initialize(200000);          // initialize timer1, and set a 1/10 second period
  Timer1.attachInterrupt(updateControls); // We will read potis and buttons values within timer interrupt
  
  // LED pin
  pinMode(LEDPin, OUTPUT);
  
  // Initialize pins for 2-pins pushbuttons with pullup enabled
  for (i=6;i<13;i++)
    pinMode(i,INPUT_PULLUP);

  for (i=0; i<7; i++) {
    buttons_wait[i] = buttonTimeout;
    buttons_pressed[i] = 0;
  }
    
  //Serial.begin(57600); 
  MIDI.begin(4);
}

void loop() {
    unsigned i, r1, r2, played;
    unsigned int note;

    updateControls();
    while (1) {
        note = a.getNextNote(&r1, &r2, &played);
        if (note)
        {
          if (r1) MIDI.sendNoteOn(BEATS_Kick, 127, 10);
          if (played)
          {
            MIDI.sendNoteOn(note, 127, 1);
            MIDI.sendNoteOn(BEATS_Agogo, 127, 10);
          } 
          else
          {
            MIDI.sendNoteOn(note, map(   cutByPotiLimit(analogRead(rootOctavePin) ) , 0, max_poti_val, 0, 127), 1);
            MIDI.sendNoteOn(BEATS_Claves, 127, 10);
          }
        }
          // Wait for the next sync signal
          while ((digitalRead(syncinpin) == 0));
          delay(65);
          
          MIDI.sendNoteOff(note, 0, 1); // Stop the note
         // cout << r1 << " " << r2 << " " << played << " " << (int) note << endl;
        //}
    }
}
