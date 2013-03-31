#include "midimap.h"


MidiMap::MidiMap()
{
    int l1;

    typeIn = EVENT_NOTE;
    typeOut = EVENT_NOTE;
    for (l1 = 0; l1 < 2; l1++) {
        chIn[l1] = 0;
        rangeIn[l1] = 0;
        indexIn[l1] = 0;
        rangeOut[l1] = 0;
    }  
    chOut = 0;
    indexOut = 0;
    chOutMode = OUTMODE_OFFSET;
    indexOutMode = OUTMODE_OFFSET;
    portOut = 0;
}

MidiMap::~MidiMap(){
}

bool MidiMap::isMap(snd_seq_event_t *evIn)
{
    if ((evIn->data.control.channel < chIn[0])
            || (evIn->data.control.channel > chIn[1])) {
        return(false);
    }       
    switch(typeIn) {
        case EVENT_NOTE:
            if (((evIn->type != SND_SEQ_EVENT_NOTEON)
                        && (evIn->type != SND_SEQ_EVENT_NOTEOFF)) 
                    || ((evIn->data.note.note < indexIn[0])
                        || (evIn->data.note.note > indexIn[1])) 
                    || ((evIn->data.note.velocity < rangeIn[0])
                        || (evIn->data.note.velocity > rangeIn[1]))) {
                return(false);
            }  
            break;

        case EVENT_CONTROLLER:
            if ((evIn->type != SND_SEQ_EVENT_CONTROLLER)          
                    || ((int)evIn->data.control.param < indexIn[0])
                    || ((int)evIn->data.control.param > indexIn[1])
                    || (evIn->data.control.value < rangeIn[0])
                    || (evIn->data.control.value > rangeIn[1])) {
                return(false);
            }
            break;

        case EVENT_PITCHBEND:
            if ((evIn->type != SND_SEQ_EVENT_PITCHBEND)          
                    || (evIn->data.control.value < rangeIn[0])
                    || (evIn->data.control.value > rangeIn[1])) {
                return(false);
            }
            break;

        case EVENT_PRGCHANGE:
            if ((evIn->type != SND_SEQ_EVENT_PGMCHANGE)          
                    || (evIn->data.control.value < rangeIn[0])
                    || (evIn->data.control.value > rangeIn[1])) {
                return(false);
            }
            break;
    }  
    return(true);
}

void MidiMap::doMap(snd_seq_event_t *evIn, snd_seq_event_t *evOut,
        bool *outOfRange)
{
    *evOut = *evIn;

    switch(chOutMode) {
        case OUTMODE_OFFSET:
            evOut->data.control.channel = clip(evOut->data.control.channel
                    + chOut, 0, 15, outOfRange);
            break;

        case OUTMODE_REVERSE:
            evOut->data.control.channel = clip(chOut
                    - evOut->data.control.channel, 0, 15, outOfRange);
            break;

        case OUTMODE_FIXED:
            evOut->data.control.channel = chOut;
            *outOfRange = false;
            break;
    }

    switch(typeIn) {
        case EVENT_NOTE:
            switch(typeOut) {
                case EVENT_NOTE:
                    switch(indexOutMode) {
                        case OUTMODE_OFFSET:
                            evOut->data.note.note = clip(evIn->data.note.note
                                    + indexOut, 0, 127, outOfRange);
                            break;

                        case OUTMODE_REVERSE:
                            evOut->data.note.note = clip(indexOut
                                    - evIn->data.note.note, 0, 127, outOfRange);
                            break;

                        case OUTMODE_FIXED:
                            evOut->data.note.note = indexOut;
                            break;
                    }  
                    evOut->data.note.velocity = linMap(
                            evIn->data.note.velocity, rangeIn, rangeOut);
                    break;

                case EVENT_CONTROLLER:
                    evOut->type = SND_SEQ_EVENT_CONTROLLER;
                    switch(indexOutMode) {
                        case OUTMODE_OFFSET:
                            evOut->data.control.param = clip(
                                    evIn->data.note.note + indexOut,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_REVERSE:
                            evOut->data.control.param = clip(
                                    indexOut - evIn->data.note.note,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_FIXED:
                            evOut->data.control.param = indexOut;
                            break;  
                    }  
                    evOut->data.control.value = linMap(
                            evIn->data.note.velocity, rangeIn, rangeOut);
                    break;

                case EVENT_PITCHBEND:
                    evOut->type = SND_SEQ_EVENT_PITCHBEND;
                    evOut->data.control.value = linMap(
                            evIn->data.note.velocity, rangeIn, rangeOut);
                    break;

                case EVENT_PRGCHANGE:
                    evOut->type = SND_SEQ_EVENT_PGMCHANGE;
                    switch(indexOutMode) {
                        case OUTMODE_OFFSET:
                            evOut->data.control.value = clip(
                                    evIn->data.note.note + indexOut,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_REVERSE:
                            evOut->data.control.value = clip(
                                    indexOut - evIn->data.note.note,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_FIXED:
                            evOut->data.control.value = indexOut;
                            break;  
                    }  
                    break;
            }  
            break;

        case EVENT_CONTROLLER:
            switch(typeOut) {
                case EVENT_NOTE:
                    evOut->type = SND_SEQ_EVENT_NOTEON;
                    switch(indexOutMode) {
                        case OUTMODE_OFFSET:
                            evOut->data.note.note = clip(
                                    evIn->data.control.param + indexOut,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_REVERSE:
                            evOut->data.note.note = clip(
                                    indexOut - evIn->data.control.param,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_FIXED:
                            evOut->data.note.note = indexOut;
                            break;

                    }  
                    evOut->data.note.velocity = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_CONTROLLER:
                    switch(indexOutMode) {
                        case OUTMODE_OFFSET:
                            evOut->data.control.param = clip(
                                    evIn->data.control.param + indexOut,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_REVERSE:
                            evOut->data.control.param = clip(
                                    indexOut - evIn->data.control.param,
                                    0, 127, outOfRange);
                            break;

                        case OUTMODE_FIXED:
                            evOut->data.control.param = indexOut;
                            break;
                    }  
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PITCHBEND:
                    evOut->type = SND_SEQ_EVENT_PITCHBEND;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PRGCHANGE:
                    evOut->type = SND_SEQ_EVENT_PGMCHANGE;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;
            }  
            break;
        case EVENT_PITCHBEND:
            switch(typeOut) {
                case EVENT_NOTE:
                    evOut->type = SND_SEQ_EVENT_NOTEON;
                    evOut->data.note.velocity = indexOut;
                    evOut->data.note.note = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_CONTROLLER:
                    evOut->type = SND_SEQ_EVENT_CONTROLLER;
                    evOut->data.control.param = indexOut;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PITCHBEND:
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PRGCHANGE:
                    evOut->type = SND_SEQ_EVENT_PGMCHANGE;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;
            }  
            break;

        case EVENT_PRGCHANGE:
            switch(typeOut) {
                case EVENT_NOTE:
                    evOut->type = SND_SEQ_EVENT_NOTEON;
                    evOut->data.note.velocity = indexOut;
                    evOut->data.note.note = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_CONTROLLER:
                    evOut->type = SND_SEQ_EVENT_CONTROLLER;
                    evOut->data.control.param = indexOut;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PITCHBEND:
                    evOut->type = SND_SEQ_EVENT_PITCHBEND;
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;

                case EVENT_PRGCHANGE:
                    evOut->data.control.value = linMap(
                            evIn->data.control.value, rangeIn, rangeOut);
                    break;
            }  
            break;
    }  
}

int MidiMap::linMap(int in, int p_rangeIn[], int p_rangeOut[])
{
    if ((p_rangeIn[0] == p_rangeIn[1]) || (p_rangeOut[0] == p_rangeOut[1])) {
        return(p_rangeOut[0]);
    }
    return(p_rangeOut[0] + (int)((float)(in - p_rangeIn[0]) *
                (float)(p_rangeOut[1] - p_rangeOut[0]) /
                (float)(p_rangeIn[1] - p_rangeIn[0])));
}

int MidiMap::clip(int value, int min, int max, bool *outOfRange)
{
    int tmp = value;

    *outOfRange = false;
    if (tmp > max) {
        tmp = max;
        *outOfRange = true;
    } else if (tmp < min) {
        tmp = min;
        *outOfRange = true;
    }  
    return(tmp);
}

void MidiMap::saveFileText(QTextStream& ts)
{
    ts << typeIn << ' ' << (int)typeOut << '\n';
    ts << chIn[0] << ' ' << chIn[1] << ' ' << chOut << '\n';
    ts << indexIn[0] << ' ' << indexIn[1] << ' ' << indexOut << '\n';
    ts << rangeIn[0] << ' ' << rangeIn[1] << ' ';
    ts << rangeOut[0] << ' ' << rangeOut[1] << '\n';
    ts << (int)chOutMode << ' ' << (int)indexOutMode << '\n';
    ts << portOut << '\n';
}

