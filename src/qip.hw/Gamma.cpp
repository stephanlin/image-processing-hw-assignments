// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Gamma.cpp - Gamma correction widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Gamma.h"
#include "hw1/HW_gamma.cpp"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::Gamma:
//
// Constructor.
//
Gamma::Gamma(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Gamma::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Gamma Correction");

	// create label
	QLabel *label = new QLabel("Gamma");

	// create slider
	m_slider = new QSlider(Qt::Horizontal, m_ctrlGrp);
	m_slider->setRange(1, 100);
	m_slider->setValue(10);
	m_slider->setTickPosition(QSlider::TicksBelow);
	m_slider->setTickInterval(25);

	// create spinbox
	m_spinBox = new QDoubleSpinBox(m_ctrlGrp);
	m_spinBox->setRange(0.1, 10.);
	m_spinBox->setValue	(1.0);
	m_spinBox->setSingleStep(0.1);
	m_spinBox->setDecimals	(2);

	// init signal/slot connections for Gamma
	connect(m_slider,  SIGNAL(valueChanged( int  )), this, SLOT(changeGammaI( int  )));
	connect(m_spinBox, SIGNAL(valueChanged(double)), this, SLOT(changeGammaD(double)));

	// assemble dialog
	QGridLayout *layout = new QGridLayout;
	layout->addWidget(label    , 0, 0);
	layout->addWidget(m_slider , 0, 1);
	layout->addWidget(m_spinBox, 0, 2);

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Gamma::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// get gamma value
	double gamma = m_slider->value() * 0.1;

	// error checking
	if(gamma < 0.1 || gamma > 10.0) return 0;

        // apply gamma correction
        gammaCorrect(I1, gamma, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::gammaCorrect:
//
// Gamma correct image I1. Output is in I2.
//
void
Gamma::gammaCorrect(ImagePtr I1, double gamma, ImagePtr I2)
{
	HW_gammaCorrect(I1, gamma, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::changeGammaI:
//
// Slot to process change in gamma value caused by moving the slider (int)
//
void
Gamma::changeGammaI(int gamma)
{
	// set spinbox
	m_spinBox->setValue(gamma*0.1);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::changeGammaD:
//
// Slot to process change in gamma value caused by moving the spinbox (double)
//
void
Gamma::changeGammaD(double gamma)
{
	// round gamma to the nearest int
	double g = gamma * 10;
	int  val = (g >= 0) ? (int)(g+.5) : -(int)(.5-g);

	// set slider
	m_slider->setValue(val);

	// apply filter and display output
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Gamma::reset:
//
// Reset parameters.
//
void
Gamma::reset()
{
	m_slider ->blockSignals(true);
	m_slider ->setValue    (1);
	m_slider ->blockSignals(false);
	m_spinBox->blockSignals(true);
	m_spinBox->setValue    (1);
	m_spinBox->blockSignals(false);

	// apply filter and display output
	g_mainWindowP->preview();
}
