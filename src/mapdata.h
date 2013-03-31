#ifndef MAPDATA_H
#define MAPDATA_H

#include <QWidget>
#include <QList>
#include <QTextStream>
#include <alsa/asoundlib.h>

#include "seqdriver.h"
#include "midimap.h"
#include "mapwidget.h"

class MapData : public QWidget  {
    
  Q_OBJECT

  private:
    QList<MidiMap *> midiMapList;
    QList<MapWidget *> mapWidgetList;
    int portCount;
    bool modified;

    void removeMidiMap(MidiMap *midiMap);

  public:
    SeqDriver *seqDriver;
    MidiMap *midiMap(int index);
    MapWidget *mapWidget(int index);

    MapData(QWidget* parent=0);
    ~MapData();
    void registerPorts(int num);
    int getPortCount();
    MidiMap* createMidiMap();
    void addMapWidget(MapWidget *mapWidget);
    void removeMapWidget(MapWidget *mapWidget);
    int midiMapCount();
    int mapWidgetCount();
    int getAlsaClientId();
    bool isModified();
    void setModified(bool);
    void saveFileText(QTextStream&);
    void readFileText(QTextStream&);
};
                              
#endif
