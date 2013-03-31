#include <QApplication>
#include <QBoxLayout>
#include <QDateTime>
#include <QLabel>
#include <QPushButton>

#include "logwidget.h"

static unsigned int MAXLINES = 1000;


LogWidget::LogWidget(QWidget *parent) : QWidget(parent), lines(0)
{
    logActive = true;
    logMidiActive = false;

    logText = new QTextEdit(this);
    logText->setFontFamily("Courier");
    logText->setReadOnly(true);
    textColor = logText->textColor();
    
    QCheckBox *enableLog = new QCheckBox(this);
    enableLog->setText(tr("&Enable Log"));
    enableLog->setToolTip(tr("Enable MIDI event logging"));
    QObject::connect(enableLog, SIGNAL(toggled(bool)), this,
            SLOT(enableLogToggle(bool)));
    enableLog->setChecked(logActive);
    
    QCheckBox *logMidiClock = new QCheckBox(this);
    logMidiClock->setText(tr("Log &MIDI Clock"));
    logMidiClock->setToolTip(
            tr("Enable logging of MIDI realtime clock events"));
    QObject::connect(logMidiClock, SIGNAL(toggled(bool)), this,
            SLOT(logMidiToggle(bool)));
    logMidiClock->setChecked(logMidiActive);
    
    QPushButton *clearButton = new QPushButton(tr("&Clear"), this);
    clearButton->setToolTip(tr("Clear all logged MIDI events"));
    QObject::connect(clearButton, SIGNAL(clicked()), this, SLOT(clear()));

    QHBoxLayout *buttonBoxLayout = new QHBoxLayout;
    buttonBoxLayout->addWidget(enableLog);
    buttonBoxLayout->addWidget(logMidiClock);
    buttonBoxLayout->addStretch(10);
    buttonBoxLayout->addWidget(clearButton);

    QVBoxLayout *logWidgetLayout = new QVBoxLayout;
    logWidgetLayout->addWidget(logText);
    logWidgetLayout->addLayout(buttonBoxLayout);

    setLayout(logWidgetLayout);
}

LogWidget::~LogWidget()
{
}

void LogWidget::appendEvent(snd_seq_event_t *ev) {

    QString qs;

    if (!logActive) {
        return;
    }
    switch (ev->type) {
        case SND_SEQ_EVENT_NOTEON:
            textColor = QColor(0, 0, 255);
            qs.sprintf("Ch %2d, Note On %3d, Vel %3d",
                    ev->data.control.channel + 1, 
                    ev->data.note.note, ev->data.note.velocity);
            break;
        case SND_SEQ_EVENT_NOTEOFF:
            textColor = QColor(0, 0, 255);
            qs.sprintf("Ch %2d, Note Off %3d", ev->data.control.channel + 1, 
                    ev->data.note.note);
            break;
        case SND_SEQ_EVENT_CONTROLLER:
            textColor = QColor(100, 160, 0);
            qs.sprintf("Ch %2d, Ctrl %3d, Val %3d", ev->data.control.channel + 1, 
                    ev->data.control.param, ev->data.control.value);
            break;
        case SND_SEQ_EVENT_PITCHBEND:
            textColor = QColor(100, 0, 255);
            qs.sprintf("Ch %2d, Pitch %5d", ev->data.control.channel + 1, 
                    ev->data.control.value);
            break;
        case SND_SEQ_EVENT_PGMCHANGE:
            textColor = QColor(0, 100, 100);
            qs.sprintf("Ch %2d, PrgChg %5d", ev->data.control.channel + 1, 
                    ev->data.control.value);
            break;
        case SND_SEQ_EVENT_CLOCK:
            if (logMidiActive) {
                textColor = QColor(150, 150, 150);
                qs = tr("MIDI Clock");
            }
            break;
        case SND_SEQ_EVENT_START:
            textColor = QColor(0, 192, 0);
            qs = tr("MIDI Start (Transport)");
            break;
        case SND_SEQ_EVENT_CONTINUE:
            textColor = QColor(0, 128, 0);
            qs = tr("MIDI Continue (Transport)");
            break;
        case SND_SEQ_EVENT_STOP:
            textColor = QColor(128, 96, 0);
            qs = tr("MIDI Stop (Transport)");
            break;
        default:
            textColor = QColor(0, 0, 0);
            qs = tr("Unknown event type (%1)").arg(ev->type);
            break;
    }
    if ((ev->type != SND_SEQ_EVENT_CLOCK) || logMidiActive)
        appendText(QTime::currentTime().toString("hh:mm:ss.zzz") +
                "  " + qs);
}

void LogWidget::enableLogToggle(bool on)
{
    logActive = on;
}

void LogWidget::logMidiToggle(bool on)
{
    logMidiActive = on;
}

void LogWidget::clear()
{
    logText->clear();
    lines = 0;
}

void LogWidget::appendText(const QString& qs)
{
    /* remove first line if line limit is reached */
    if (lines >= MAXLINES) {
        logText->setUpdatesEnabled(false);
        logText->textCursor().clearSelection();
        logText->moveCursor(QTextCursor::Start);
        logText->moveCursor(QTextCursor::Down, QTextCursor::KeepAnchor);
        logText->textCursor().removeSelectedText();
        logText->moveCursor(QTextCursor::End);
        logText->setUpdatesEnabled(true);
        lines--;
    }
    logText->setTextColor(textColor);
    logText->append(qs);
    lines++;
}

