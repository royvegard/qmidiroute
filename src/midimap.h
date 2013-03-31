#ifndef MIDIMAP_H
#define MIDIMAP_H

#include <QObject>
#include <QTextStream>
#include <alsa/asoundlib.h>

enum eventType {EVENT_NOTE, EVENT_CONTROLLER, EVENT_PITCHBEND, EVENT_PRGCHANGE};
enum outModeType {OUTMODE_OFFSET, OUTMODE_REVERSE, OUTMODE_FIXED};

class MidiMap : public QObject  {
    
  Q_OBJECT
    
  private:
    int linMap(int in, int p_rangeIn[], int p_rangeOut[]);  
    int clip(int value, int min, int max, bool *outOfRange);
    
  public:
    eventType typeIn, typeOut;           // Type of input/output events
    int chIn[2], chOut;                  // Channel of input/output events
    int indexIn[2], indexOut;            // Index input/output (for Controller events)
    int rangeIn[2], rangeOut[2];         // Parameter that is mapped, [0] low, [1] high boundary
    outModeType chOutMode, indexOutMode;
    int portOut;                         // Output port (ALSA Sequencer)
          
  public:
    MidiMap();
    ~MidiMap();
    bool isMap(snd_seq_event_t *evIn);   // Check if evIn is in the input range of the map
    void doMap(snd_seq_event_t *evIn, snd_seq_event_t *evOut, bool *outOfRange); // Map evIn to evOut
    void saveFileText(QTextStream&);
};
                              
#endif
