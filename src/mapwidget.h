#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QString>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QTextStream>

#include "midimap.h"

class MapWidget : public QWidget

{
  Q_OBJECT

  private:
    QComboBox *typeIn, *typeOut;           // Type of input/output events
    QSpinBox *chIn[2], *chOut;             // Channel of input/output events
    QSpinBox *indexIn[2], *indexOut;       // Index input/output (for Controller events)
    QSpinBox *rangeIn[2], *rangeOut[2];    // Parameter that is mapped, [0] low, [1] high boundary
    QSpinBox *portOut;                     // Output port (ALSA Sequencer)
    QComboBox *chOutMode, *indexOutMode;   // Offset, Reverse, Fixed
    QLabel *indexInLabel, *rangeInLabel, *indexOutLabel, *rangeOutLabel;
    MidiMap *midiMap;
    int typeInHighlight, oldIndexOutMode;
    bool updateOldIndexOutMode;
    bool modified;

  public:
    QString mapName;

    MapWidget(MidiMap *p_midiMap, int portCount, QWidget* parent=0);
    ~MapWidget();
    MidiMap *getMidiMap();
    void readMap(QTextStream& mapText);
    void writeMap(QTextStream& mapText);
    void setTypeIn(int index);  
    void setTypeOut(int index);  
    void setChOutMode(int index);
    void setIndexOutMode(int index);
    void setChIn(int value);
    void setChOut(int value);
    void setIndexIn(int value);
    void setIndexOut(int value);
    void setRangeIn(int value);
    void setRangeOut(int value);
    void setPortOut(int value);
    void setChIn(int index, int value);
    void setIndexIn(int index, int value);
    void setRangeIn(int index, int value);
    void setRangeOut(int index, int value);
    bool isModified();
    void setModified(bool);
    
  public slots:
    void updateTypeIn(int index);  
    void updateTypeOut(int index);  
    void updateChOutMode(int index);
    void updateIndexOutMode(int index);
    void updateChIn(int value);
    void updateChOut(int value);
    void updateIndexIn(int value);
    void updateIndexOut(int value);
    void updateRangeIn(int value);
    void updateRangeOut(int value);
    void updatePortOut(int value);
};
  
#endif
