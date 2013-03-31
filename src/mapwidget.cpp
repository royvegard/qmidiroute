#include <QBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QStringList>

#include "mapwidget.h"


    MapWidget::MapWidget(MidiMap *p_midiMap, int portCount, QWidget *parent)
: QWidget(parent), midiMap(p_midiMap), modified(false)
{
    QStringList typeNames; 
    typeNames << tr("Note") << tr("Controller") << tr("Pitchbend")
        << tr("Program Change");

    QStringList outModeNames;
    outModeNames << tr("Offset") << tr("Reverse, Offset") << tr("Fixed");

    // Input box on left side
    QGroupBox *inBox = new QGroupBox(tr("Input"), this);

    // type line
    QLabel *typeInLabel = new QLabel(tr("&Type"), inBox);
    typeInHighlight = 0;
    typeIn = new QComboBox(inBox);
    typeInLabel->setBuddy(typeIn);
    connect(typeIn, SIGNAL(activated(int)), this, SLOT(updateTypeIn(int)));
    typeIn->insertItems(0, typeNames);

    // channel line
    QLabel *chInLabel = new QLabel(tr("&Channel"), inBox);
    chIn[0] = new QSpinBox(inBox);
    chInLabel->setBuddy(chIn[0]);
    connect(chIn[0], SIGNAL(valueChanged(int)), this,
            SLOT(updateChIn(int)));
    chIn[0]->setRange(1, 16);
    chIn[0]->setSingleStep(1);
    chIn[0]->setKeyboardTracking(false);
    chIn[1] = new QSpinBox(inBox);
    connect(chIn[1], SIGNAL(valueChanged(int)), this, SLOT(updateChIn(int)));
    chIn[1]->setRange(1, 16);
    chIn[1]->setSingleStep(1);
    chIn[1]->setKeyboardTracking(false);
    chIn[1]->setValue(16);

    // note line
    indexInLabel = new QLabel(tr("&Note"), inBox);
    indexIn[0] = new QSpinBox(inBox);
    indexInLabel->setBuddy(indexIn[0]);
    connect(indexIn[0], SIGNAL(valueChanged(int)), this,
            SLOT(updateIndexIn(int)));
    indexIn[0]->setRange(0, 127);
    indexIn[0]->setSingleStep(1);
    indexIn[0]->setKeyboardTracking(false);
    indexIn[1] = new QSpinBox(inBox);
    connect(indexIn[1], SIGNAL(valueChanged(int)), this,
            SLOT(updateIndexIn(int)));
    indexIn[1]->setRange(0, 127);
    indexIn[1]->setSingleStep(1);
    indexIn[1]->setValue(127);
    indexIn[1]->setKeyboardTracking(false);

    // velocity line
    rangeInLabel = new QLabel(tr("&Velocity"), inBox);
    rangeIn[0] = new QSpinBox(inBox);
    rangeInLabel->setBuddy(rangeIn[0]);
    connect(rangeIn[0], SIGNAL(valueChanged(int)), this,
            SLOT(updateRangeIn(int)));
    rangeIn[0]->setRange(0, 127);
    rangeIn[0]->setSingleStep(1);
    rangeIn[0]->setKeyboardTracking(false);
    rangeIn[1] = new QSpinBox(inBox);
    connect(rangeIn[1], SIGNAL(valueChanged(int)), this,
            SLOT(updateRangeIn(int)));
    rangeIn[1]->setRange(0, 127);
    rangeIn[1]->setSingleStep(1);
    rangeIn[1]->setValue(127);
    rangeIn[1]->setKeyboardTracking(false);

    // input box layout setup
    QGridLayout* inGrid = new QGridLayout;

    inGrid->addWidget(typeInLabel, 0, 0);
    inGrid->addWidget(typeIn, 0, 1, 1, 2);

    inGrid->addWidget(chInLabel, 1, 0);
    inGrid->addWidget(indexInLabel, 2, 0);
    inGrid->addWidget(rangeInLabel, 3, 0);

    inGrid->addWidget(chIn[0], 1, 1);
    inGrid->addWidget(indexIn[0], 2 ,1);
    inGrid->addWidget(rangeIn[0], 3, 1);

    inGrid->addWidget(chIn[1], 1, 2);
    inGrid->addWidget(indexIn[1], 2, 2);
    inGrid->addWidget(rangeIn[1], 3, 2);

    QVBoxLayout* inBoxLayout = new QVBoxLayout;
    inBoxLayout->addLayout(inGrid);
    inBoxLayout->addStretch(); 

    inBox->setLayout(inBoxLayout);


    // Output box on right side
    QGroupBox *outBox = new QGroupBox(tr("Output"), this);

    // type line
    QLabel *typeOutLabel = new QLabel(tr("T&ype"), outBox);
    typeOut = new QComboBox(outBox);
    typeOutLabel->setBuddy(typeOut);
    connect(typeOut, SIGNAL(activated(int)), this, SLOT(updateTypeOut(int)));
    typeOut->insertItems(0, typeNames);

    // channel line
    QLabel *chOutLabel = new QLabel(tr("Chann&el"), outBox);
    chOutMode = new QComboBox(outBox);
    chOutLabel->setBuddy(chOutMode);
    chOutMode->insertItems(0, outModeNames);
    connect(chOutMode, SIGNAL(activated(int)), this,
            SLOT(updateChOutMode(int)));
    chOut = new QSpinBox(outBox);
    connect(chOut, SIGNAL(valueChanged(int)), this, SLOT(updateChOut(int)));
    updateChOutMode(chOutMode->currentIndex());
    chOut->setSingleStep(1);
    chOut->setValue(0);
    chOut->setKeyboardTracking(false);

    // note line
    indexOutLabel = new QLabel(tr("N&ote"), outBox);
    oldIndexOutMode = 0;
    updateOldIndexOutMode = true;
    indexOutMode = new QComboBox(outBox);
    indexOutLabel->setBuddy(indexOutMode);
    indexOutMode->insertItems(0, outModeNames);     
    connect(indexOutMode, SIGNAL(activated(int)), this,
            SLOT(updateIndexOutMode(int)));
    indexOut = new QSpinBox(outBox);
    connect(indexOut, SIGNAL(valueChanged(int)), this,
            SLOT(updateIndexOut(int)));
    indexOut->setRange(-127, 127);
    indexOut->setSingleStep(1);
    indexOut->setValue(0);
    indexOut->setKeyboardTracking(false);

    // velocity line
    rangeOutLabel = new QLabel(tr("Ve&locity"), outBox);
    rangeOut[0] = new QSpinBox(outBox);
    rangeOutLabel->setBuddy(rangeOut[0]);
    connect(rangeOut[0], SIGNAL(valueChanged(int)), this,
            SLOT(updateRangeOut(int)));
    rangeOut[0]->setRange(0, 127);
    rangeOut[0]->setSingleStep(1);
    rangeOut[0]->setKeyboardTracking(false);
    rangeOut[1] = new QSpinBox(outBox);
    connect(rangeOut[1], SIGNAL(valueChanged(int)),
            this, SLOT(updateRangeOut(int)));
    rangeOut[1]->setRange(0, 127);
    rangeOut[1]->setSingleStep(1); 
    rangeOut[1]->setValue(127);
    rangeOut[1]->setKeyboardTracking(false);

    // port line
    QLabel *portLabel = new QLabel(tr("ALSA &Port"), this);
    portOut = new QSpinBox(this);
    portLabel->setBuddy(portOut);
    portOut->setRange(1, portCount);
    portOut->setSingleStep(1);
    portOut->setKeyboardTracking(false);
    connect(portOut, SIGNAL(valueChanged(int)), this,
            SLOT(updatePortOut(int)));

    // output box layout setup
    QGridLayout* outGrid = new QGridLayout;

    outGrid->addWidget(typeOutLabel, 0, 0);
    outGrid->addWidget(typeOut, 0, 1, 1, 2);

    outGrid->addWidget(chOutLabel, 1, 0);
    outGrid->addWidget(indexOutLabel, 2, 0);
    outGrid->addWidget(rangeOutLabel, 3, 0);

    outGrid->addWidget(chOutMode, 1, 1);
    outGrid->addWidget(indexOutMode, 2, 1);
    outGrid->addWidget(rangeOut[0], 3, 1);

    outGrid->addWidget(chOut, 1, 2);
    outGrid->addWidget(indexOut, 2, 2);
    outGrid->addWidget(rangeOut[1], 3, 2);

    outGrid->addWidget(portLabel, 4, 0);
    outGrid->addWidget(portOut, 4, 1);

    QVBoxLayout* outBoxLayout = new QVBoxLayout;
    outBoxLayout->addLayout(outGrid);
    outBoxLayout->addStretch(); 

    outBox->setLayout(outBoxLayout);


    // left/right placement of in/out boxes
    QHBoxLayout *ioBoxLayout = new QHBoxLayout;
    ioBoxLayout->addWidget(inBox);
    ioBoxLayout->addWidget(outBox);
    ioBoxLayout->addStretch();

    setLayout(ioBoxLayout);
}

MapWidget::~MapWidget()
{
}

MidiMap *MapWidget::getMidiMap()
{
    return (midiMap);
}

void MapWidget::updateTypeIn(int index)
{
    typeInHighlight = index;
    midiMap->typeIn = (eventType)index;
    switch (index) {
        case 0:
            indexInLabel->setText(tr("&Note"));
            rangeInLabel->setText(tr("&Velocity"));
            rangeIn[0]->setMinimum(0);
            rangeIn[1]->setMinimum(0);
            rangeIn[0]->setMaximum(127);
            rangeIn[1]->setMaximum(127);
            indexIn[0]->setEnabled(true);
            indexIn[1]->setEnabled(true);
            break;
        case 1:
            indexInLabel->setText(tr("Cont&roller"));
            rangeInLabel->setText(tr("V&alue"));
            rangeIn[0]->setMinimum(0);
            rangeIn[1]->setMinimum(0);
            rangeIn[0]->setMaximum(127);
            rangeIn[1]->setMaximum(127);
            indexIn[0]->setEnabled(true);
            indexIn[1]->setEnabled(true);
            break;
        case 2:
            indexInLabel->setText("");
            rangeInLabel->setText(tr("P&itch"));
            rangeIn[0]->setMinimum(-8192);
            rangeIn[1]->setMinimum(-8192);
            rangeIn[0]->setMaximum(8192);
            rangeIn[1]->setMaximum(8192);
            indexIn[0]->setDisabled(true);
            indexIn[1]->setDisabled(true);
            break;    
        case 3:
            indexInLabel->setText("");
            rangeInLabel->setText(tr("V&alue"));
            rangeIn[0]->setMinimum(0);
            rangeIn[1]->setMinimum(0);
            rangeIn[0]->setMaximum(127);
            rangeIn[1]->setMaximum(127);
            indexIn[0]->setDisabled(true);
            indexIn[1]->setDisabled(true);
            break;    
    }
    updateTypeOut(-1);
}

void MapWidget::updateTypeOut(int index)
{
    if (index < 0) {
        index = typeOut->currentIndex();
    }
    midiMap->typeOut = (eventType)index;
    updateOldIndexOutMode = false;

    switch (index) {
        case 0: //Note
            switch(typeInHighlight) {
                case 0: // Note + Controller
                case 1:
                    rangeOutLabel->setText(tr("V&elocity"));
                    rangeOut[0]->setEnabled(true);
                    rangeOut[1]->setEnabled(true);
                    indexOutLabel->setText(tr("N&ote"));
                    indexOut->setEnabled(true);
                    indexOutMode->setEnabled(true);
                    indexOutMode->setCurrentIndex(oldIndexOutMode);
                    break;

                case 2: // Pitchbend
                    rangeOutLabel->setText(tr("N&ote"));
                    rangeOut[0]->setEnabled(true);
                    rangeOut[1]->setEnabled(true);
                    indexOutLabel->setText(tr("V&elocity"));
                    indexOut->setEnabled(true);
                    indexOutMode->setDisabled(true);
                    indexOutMode->setCurrentIndex(2);
                    break;

                case 3:  // Program Change
                    rangeOutLabel->setText(tr("N&ote"));
                    rangeOut[0]->setEnabled(true);
                    rangeOut[1]->setEnabled(true);
                    indexOutLabel->setText(tr("V&elocity"));
                    indexOut->setEnabled(true);
                    indexOutMode->setDisabled(true);
                    indexOutMode->setCurrentIndex(2);
                    break;
            }  
            break;

        case 1: //Controller
            rangeOutLabel->setText(tr("Val&ue"));
            rangeOut[0]->setEnabled(true);
            rangeOut[1]->setEnabled(true);
            rangeOut[0]->setMinimum(0);
            rangeOut[1]->setMinimum(0);
            rangeOut[0]->setMaximum(127);
            rangeOut[1]->setMaximum(127);
            indexOutLabel->setText(tr("C&ontroller"));
            indexOut->setEnabled(true);

            switch(typeInHighlight) {
                case 0:
                case 1:
                    indexOutMode->setEnabled(true);
                    indexOutMode->setCurrentIndex(oldIndexOutMode);
                    break;
                case 2:
                case 3:
                    indexOutMode->setDisabled(true);
                    indexOutMode->setCurrentIndex(2);
                    break;  
            }  
            break;

        case 2: //Pitchbend
            rangeOutLabel->setText(tr("Pit&ch"));
            rangeOut[0]->setEnabled(true);
            rangeOut[1]->setEnabled(true);
            rangeOut[0]->setMinimum(-8192);
            rangeOut[1]->setMinimum(-8192);
            rangeOut[0]->setMaximum(8192);
            rangeOut[1]->setMaximum(8192);
            indexOutLabel->setText("");
            indexOut->setDisabled(true);
            indexOutMode->setDisabled(true);
            indexOutMode->setCurrentIndex(2);
            break;

        case 3: //Program Change
            switch(typeInHighlight) {
                case 0:
                    indexOut->setEnabled(true);
                    indexOutMode->setEnabled(true);
                    indexOutLabel->setText(tr("&Value"));
                    rangeOutLabel->setText("");
                    rangeOut[0]->setDisabled(true);
                    rangeOut[1]->setDisabled(true);
                    break;
                case 1: 
                case 2:
                case 3:                    
                    rangeOutLabel->setText(tr("&Value"));
                    rangeOut[0]->setEnabled(true);
                    rangeOut[1]->setEnabled(true);
                    rangeOut[0]->setMinimum(0);
                    rangeOut[1]->setMinimum(0);
                    rangeOut[0]->setMaximum(127);
                    rangeOut[1]->setMaximum(127);
                    indexOutLabel->setText("");
                    indexOut->setDisabled(true);
                    indexOutMode->setCurrentIndex(oldIndexOutMode);
                    indexOutMode->setDisabled(true);
                    break;    
            }    
    }
    updateOldIndexOutMode = true;
    modified = true;
}

void MapWidget::updateChOutMode(int index)
{
    midiMap->chOutMode = (outModeType)index;
    switch (index) {
        case 0: // Offset
            chOut->setRange(-15, 15);
            midiMap->chOut = chOut->value();
            break;
        case 1: // Reverse, Offset
            chOut->setRange(2, 17);
            midiMap->chOut = chOut->value() - 2;
            break;
        case 2: // Fixed
            chOut->setRange(1, 16);
            midiMap->chOut = chOut->value() - 1;
            break;
    }
    modified = true;
}

void MapWidget::updateIndexOutMode(int index)
{
    midiMap->indexOutMode = (outModeType)index; 
    if (updateOldIndexOutMode) {
        oldIndexOutMode = index;
    }  
    modified = true;
}  

void MapWidget::updateChIn(int value)
{
    if (chIn[0] == sender()) {
        midiMap->chIn[0] = value - 1; 
    } else {
        midiMap->chIn[1] = value - 1;
    }  
    modified = true;
}

void MapWidget::updateChOut(int value)
{
    switch (chOutMode->currentIndex()) {
        case 0: // Offset
            midiMap->chOut = value;
            break;
        case 1: // Reverse, Offset
            midiMap->chOut = value - 2;
            break;
        case 2: // Fixed
            midiMap->chOut = value - 1;
            break;
    }
    modified = true;
}

void MapWidget::updateIndexIn(int value)
{
    if (indexIn[0] == sender()) {
        midiMap->indexIn[0] = value; 
    } else {
        midiMap->indexIn[1] = value;
    }  
    modified = true;
}

void MapWidget::updateIndexOut(int value)
{
    midiMap->indexOut = value;
    modified = true;
}

void MapWidget::updateRangeIn(int value)
{
    if (rangeIn[0] == sender()) {
        midiMap->rangeIn[0] = value; 
    } else {
        midiMap->rangeIn[1] = value;
    }  
    modified = true;
}

void MapWidget::updateRangeOut(int value)
{
    if (rangeOut[0] == sender()) {
        midiMap->rangeOut[0] = value; 
    } else {
        midiMap->rangeOut[1] = value;
    }  
    modified = true;
}

void MapWidget::updatePortOut(int value)
{
    midiMap->portOut = value - 1;
    modified = true;
}

void MapWidget::writeMap(QTextStream& ts)
{
    ts << mapName << '\n';
    midiMap->saveFileText(ts);
    modified = false;
}                                      

void MapWidget::readMap(QTextStream& mapText)
{
    QString qs, qs2;

    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0); 
    typeIn->setCurrentIndex(qs2.toInt());
    updateTypeIn(qs2.toInt());
    qs2 = qs.section(' ', 1, 1);
    typeOut->setCurrentIndex(qs2.toInt());
    updateTypeOut(qs2.toInt());
    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0); 
    chIn[0]->setValue(qs2.toInt() + 1);
    qs2 = qs.section(' ', 1, 1); 
    chIn[1]->setValue(qs2.toInt() + 1);
    qs2 = qs.section(' ', 2, 2);
    int chOutTemp = qs2.toInt();
    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0); 
    indexIn[0]->setValue(qs2.toInt());
    qs2 = qs.section(' ', 1, 1); 
    indexIn[1]->setValue(qs2.toInt());
    qs2 = qs.section(' ', 2, 2);
    indexOut->setValue(qs2.toInt());
    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0); 
    rangeIn[0]->setValue(qs2.toInt());
    qs2 = qs.section(' ', 1, 1); 
    rangeIn[1]->setValue(qs2.toInt());
    qs2 = qs.section(' ', 2, 2);
    rangeOut[0]->setValue(qs2.toInt());
    qs2 = qs.section(' ', 3, 3);
    rangeOut[1]->setValue(qs2.toInt());
    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0);
    chOutMode->setCurrentIndex(qs2.toInt());
    updateChOutMode(qs2.toInt());
    qs2 = qs.section(' ', 1, 1); 
    indexOutMode->setCurrentIndex(qs2.toInt());
    updateIndexOutMode(qs2.toInt());
    qs = mapText.readLine();
    qs2 = qs.section(' ', 0, 0); 
    portOut->setValue(qs2.toInt() + 1);

    switch (chOutMode->currentIndex()) {
        case 0: // Offset
            chOut->setValue(chOutTemp);
            break;
        case 1: // Reverse, Offset
            chOut->setValue(chOutTemp + 2);
            break;
        case 2: // Fixed
            chOut->setValue(chOutTemp + 1);
            break;
    }
    modified = false;
}                                      

void MapWidget::setTypeIn(int index)
{
    typeIn->setCurrentIndex(index); 
}

void MapWidget::setTypeOut(int index) 
{
    typeOut->setCurrentIndex(index); 
}

void MapWidget::setChOutMode(int index)
{
    chOutMode->setCurrentIndex(index);
}

void MapWidget::setIndexOutMode(int index)
{
    indexOutMode->setCurrentIndex(index);
    oldIndexOutMode = index; 
}  

void MapWidget::setChIn(int index, int value)
{
    chIn[index]->setValue(value);
}

void MapWidget::setChOut(int value)
{
    chOut->setValue(value);
}

void MapWidget::setIndexIn(int index, int value)
{
    indexIn[index]->setValue(value); 
}

void MapWidget::setIndexOut(int value)
{
    indexOut->setValue(value);
}

void MapWidget::setRangeIn(int index, int value)
{
    rangeIn[index]->setValue(value);
}

void MapWidget::setRangeOut(int index, int value)
{
    rangeOut[index]->setValue(value);
}

void MapWidget::setPortOut(int value)
{
    portOut->setValue(value);
}

bool MapWidget::isModified()
{
    return modified;
}

void MapWidget::setModified(bool m)
{
    modified = m;
}

