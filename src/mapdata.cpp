#include <qstring.h>
#include <qfile.h>
#include <qlist.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <alsa/asoundlib.h>

#include "seqdriver.h"
#include "mapdata.h"


MapData::MapData(QWidget *parent) : QWidget(parent), modified(false)
{
    seqDriver = new SeqDriver(&midiMapList, this);
    // midiMapList.setAutoDelete(true);
}

MapData::~MapData()
{
}

/* create e new MIDI map and add it to the internal list, return
 * pointer to the created map */
MidiMap* MapData::createMidiMap()
{
    MidiMap *midiMap = new MidiMap();
    midiMapList.append(midiMap);
    return midiMap;
}

void MapData::addMapWidget(MapWidget *mapWidget)
{
    mapWidgetList.append(mapWidget);
    modified = true;
}

void MapData::removeMidiMap(MidiMap *mm)
{
    int i = midiMapList.indexOf(mm);
    if (i != -1)
        delete midiMapList.takeAt(i);
}

/* remove map widget from internal list (without delete) and
 * delete associated mapdata item */
void MapData::removeMapWidget(MapWidget *mw)
{
    removeMidiMap(mw->getMidiMap());
    mapWidgetList.removeOne(mw);
    modified = true;
}

int MapData::midiMapCount()
{
    return(midiMapList.count());
}

int MapData::mapWidgetCount()
{
    return(mapWidgetList.count());
}

MidiMap *MapData::midiMap(int index)
{
    return(midiMapList.at(index));
}

MapWidget *MapData::mapWidget(int index)
{
    return(mapWidgetList.at(index));
}

void MapData::registerPorts(int num)
{
    portCount = num;
    seqDriver->registerPorts(num);
}

int MapData::getPortCount()
{
    return(portCount);
}

bool MapData::isModified()
{
    bool mapmodified = false;

    for (int l1 = 0; l1 < mapWidgetCount(); l1++)
        if (mapWidget(l1)->isModified()) {
            mapmodified = true;
            break;
        }

    return modified || seqDriver->isModified() || mapmodified;
}

void MapData::setModified(bool m)
{
    modified = m;
    seqDriver->setModified(m);

    for (int l1 = 0; l1 < mapWidgetCount(); l1++)
        mapWidget(l1)->setModified(m);
}

void MapData::saveFileText(QTextStream& ts)
{
    seqDriver->saveFileText(ts);

    for (int l1 = 0; l1 < mapWidgetCount(); l1++)
        mapWidget(l1)->writeMap(ts);

    modified = false;
}

void MapData::readFileText(QTextStream& ts)
{
    int count = midiMapCount();
    if (count > 0)
        mapWidget(count - 1)->readMap(ts);

    modified = false;
}

int MapData::getAlsaClientId()
{
    return seqDriver->getAlsaClientId();
}
