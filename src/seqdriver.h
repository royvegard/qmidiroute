#ifndef SEQDRIVER_H
#define SEQDRIVER_H

#include <QList>
#include <QSocketNotifier>
#include <QTextStream>
#include <QWidget>
#include <alsa/asoundlib.h>

#include "midimap.h"
#include "main.h"
#include "mapwidget.h"


class SeqDriver : public QWidget {
    
  Q_OBJECT

  private:
    int portCount;
    QList<MidiMap *> *midiMapList; 
    QSocketNotifier *seqNotifier;
    snd_seq_t *seq_handle;
    int clientid;
    int portid_out[MAX_PORTS];
    int portid_in;
    bool modified;

    void initSeqNotifier();  
        
  public:
    bool discardUnmatched;
    int portUnmatched;

    SeqDriver(QList<MidiMap *> *p_midiMapList, QWidget* parent=0);
    ~SeqDriver();
    void registerPorts(int num);
    int getPortCount();
    void saveFileText(QTextStream&);
    bool isModified();
    void setModified(bool);
    int getAlsaClientId();

  signals:
    void midiEvent(snd_seq_event_t *ev);

  public slots:
    void procEvents();  
    void setDiscardUnmatched(bool on);
    void setPortUnmatched(int id);
};
                              
#endif
