#include <cstdio> // for snprintf()

#include "seqdriver.h"
#include "config.h"


SeqDriver::SeqDriver(QList<MidiMap *> *p_midiMapList, QWidget *parent)
    : QWidget(parent), modified(false)
{
    int err;

    midiMapList = p_midiMapList; 
    portCount = 0;
    discardUnmatched = true;
    portUnmatched = 0;
    clientid = -1;
    
    err = snd_seq_open(&seq_handle, "hw", SND_SEQ_OPEN_DUPLEX, 0);
    if (err < 0) {
        qWarning("Error opening ALSA sequencer (%s).", snd_strerror(err));
        exit(1);
    }
    snd_seq_set_client_name(seq_handle, PACKAGE);
    clientid = snd_seq_client_id(seq_handle);
    portid_in = snd_seq_create_simple_port(seq_handle, "in",
                    SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
                    SND_SEQ_PORT_TYPE_APPLICATION);
    if (portid_in < 0) {
        qWarning("Error creating sequencer port (%s).",
                snd_strerror(portid_in));
        exit(1);
    }
}

SeqDriver::~SeqDriver()
{
}

void SeqDriver::registerPorts(int num)
{
    int l1;
    char buf[16];

    portCount = num;
    for (l1 = 0; l1 < portCount; l1++) {
        snprintf(buf, sizeof(buf), "out %d", l1 + 1);
        portid_out[l1] = snd_seq_create_simple_port(seq_handle, buf,
                        SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
                        SND_SEQ_PORT_TYPE_APPLICATION);
        if (portid_out[l1] < 0) {
            qWarning("Error creating sequencer port (%s).",
                    snd_strerror(portid_out[l1]));
            exit(1);
        }
    }
    initSeqNotifier();
}

void SeqDriver::initSeqNotifier()
{
    int alsaEventFd = 0;

    struct pollfd pfd[1];
    snd_seq_poll_descriptors(seq_handle, pfd, 1, POLLIN);
    alsaEventFd = pfd[0].fd;
    seqNotifier = new QSocketNotifier(alsaEventFd, QSocketNotifier::Read);
    connect(seqNotifier, SIGNAL(activated(int)),
            this, SLOT(procEvents()));
}


int SeqDriver::getPortCount()
{
    return(portCount);
}

void SeqDriver::procEvents()
{
    int l1;
    snd_seq_event_t *evIn, evOut;
    bool outOfRange = false;
    bool unmatched = false;
    MidiMap* mm;

    do {
        snd_seq_event_input(seq_handle, &evIn);
        emit midiEvent(evIn);
        unmatched = true;
        for(l1 = 0; l1 < midiMapList->count(); l1++) {
            mm = midiMapList->at(l1);
            if (mm->isMap(evIn)) {
                unmatched = false;
                mm->doMap(evIn, &evOut, &outOfRange);
                if (!outOfRange) {
                    snd_seq_ev_set_subs(&evOut);  
                    snd_seq_ev_set_direct(&evOut);
                    snd_seq_ev_set_source(&evOut, portid_out[mm->portOut]);
                    snd_seq_event_output_direct(seq_handle, &evOut);
                }  
            }
        }
        if (!discardUnmatched && unmatched) {
            snd_seq_ev_set_subs(evIn);  
            snd_seq_ev_set_direct(evIn);
            snd_seq_ev_set_source(evIn, portid_out[portUnmatched]);
            snd_seq_event_output_direct(seq_handle, evIn);
        }
    } while (snd_seq_event_input_pending(seq_handle, 0) > 0);  
}

void SeqDriver::setDiscardUnmatched(bool on)
{
    discardUnmatched = on;
    modified = true;
}

void SeqDriver::setPortUnmatched(int id)
{
    portUnmatched = id;
    modified = true;
}

void SeqDriver::saveFileText(QTextStream& ts)
{
    ts << (int)discardUnmatched << ' ' << portUnmatched << '\n';
    modified = false;
}

void SeqDriver::setModified(bool m)
{
    modified = m;
}

bool SeqDriver::isModified()
{
    return modified;
}

int SeqDriver::getAlsaClientId()
{
    return clientid;
}
