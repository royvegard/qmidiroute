#ifndef PASSWIDGET_H
#define PASSWIDGET_H

#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QString>


class PassWidget : public QWidget
{
  Q_OBJECT

  private:
    QCheckBox *discardCheck;
    QSpinBox *portUnmatchedSpin;
    QLabel *portLabel;
          
  public:
    PassWidget(int p_portcount, QWidget* parent=0);
    ~PassWidget();
    void setDiscard(bool on);
    void setPortUnmatched(int id);
    
  signals:
    void discardToggled(bool);  
    void newPortUnmatched(int);
        
  public slots:
    void updateDiscard(bool on);
    void updatePortUnmatched(int);
};
  
#endif
