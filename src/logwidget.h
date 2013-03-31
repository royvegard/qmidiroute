#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QString>
#include <QCheckBox>
#include <QTextEdit>
#include <alsa/asoundlib.h>


class LogWidget : public QWidget

{
  Q_OBJECT

  private:
    QTextEdit *logText;
    QCheckBox *enableLog;
    QCheckBox *logMidiClock;
    QColor textColor;
    bool logActive;
    bool logMidiActive;
    unsigned int lines;


  public:
    LogWidget(QWidget* parent=0);
    ~LogWidget();
  	 
  public slots:
    void logMidiToggle(bool on);
    void enableLogToggle(bool on);
    void appendEvent(snd_seq_event_t *ev);
    void appendText(const QString&);
    void clear();
};
  
#endif
