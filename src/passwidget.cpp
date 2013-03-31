#include <QBoxLayout>

#include "passwidget.h"


PassWidget::PassWidget(int p_portcount, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *passWidgetLayout = new QVBoxLayout;

    discardCheck = new QCheckBox(this);
    discardCheck->setText(tr("&Discard unmatched events"));
    discardCheck->setChecked(true);
    connect(discardCheck, SIGNAL(toggled(bool)), this,
            SLOT(updateDiscard(bool)));

    portLabel = new QLabel(tr("&Send unmatched events to port"), this);
    portUnmatchedSpin = new QSpinBox(this);
    portUnmatchedSpin->setMaximumWidth(80);
    portLabel->setBuddy(portUnmatchedSpin);
    portLabel->setDisabled(true);
    portUnmatchedSpin->setRange(1, p_portcount);
    portUnmatchedSpin->setSingleStep(1);
    portUnmatchedSpin->setDisabled(true);
    portUnmatchedSpin->setKeyboardTracking(false);
    connect(portUnmatchedSpin, SIGNAL(valueChanged(int)), this,
            SLOT(updatePortUnmatched(int)));
    
    QHBoxLayout *portBoxLayout = new QHBoxLayout;
    portBoxLayout->addWidget(portLabel);
    portBoxLayout->addSpacing(12);
    portBoxLayout->addWidget(portUnmatchedSpin);
    portBoxLayout->addStretch();

    passWidgetLayout->addWidget(discardCheck);
    passWidgetLayout->addLayout(portBoxLayout);
    passWidgetLayout->addStretch();

    setLayout(passWidgetLayout);
}

PassWidget::~PassWidget()
{
}

void PassWidget::updateDiscard(bool on)
{
    emit discardToggled(on);
    portUnmatchedSpin->setDisabled(on);
    portLabel->setDisabled(on);
}

void PassWidget::updatePortUnmatched(int id)
{
    emit newPortUnmatched(id - 1);
}

void PassWidget::setDiscard(bool on)
{
    discardCheck->setChecked(on);
}

void PassWidget::setPortUnmatched(int id)
{
    portUnmatchedSpin->setValue(id);
}
