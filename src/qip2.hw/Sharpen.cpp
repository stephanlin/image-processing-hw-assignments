// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Sharpen.cpp - Sharpen widget.
//
// Written by: George Wolberg, 2016
// ======================================================================

#include "MainWindow.h"
#include "Sharpen.h"
#include "hw2/HW_sharpen.cpp"

extern MainWindow *g_mainWindowP;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::Sharpen:
//
// Constructor.
//
Sharpen::Sharpen(QWidget *parent) : ImageFilter(parent)
{}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::controlPanel:
//
// Create group box for control panel.
//
QGroupBox*
Sharpen::controlPanel()
{
	// init group box
	m_ctrlGrp = new QGroupBox("Sharpen");

	// layout for assembling filter widget
	QGridLayout *layout = new QGridLayout;

	// alloc array of labels
	QLabel *label[2];

	// create sliders and spinboxes
	for(int i=0; i<2; i++) {
		// create label[i]
		label[i] = new QLabel(m_ctrlGrp);
		if(!i) label[i]->setText("Filter size");
		else   label[i]->setText("Factor");

		// create slider
		m_slider [i] = new QSlider(Qt::Horizontal, m_ctrlGrp);
		m_slider [i]->setRange(1, 30);
		m_slider [i]->setValue(3);
		m_slider [i]->setSingleStep(2);
		m_slider [i]->setTickPosition(QSlider::TicksBelow);
		m_slider [i]->setTickInterval(5);

		// create spinbox
		m_spinBox[i] = new QSpinBox(m_ctrlGrp);
		m_spinBox[i]->setRange(1, 30);
		m_spinBox[i]->setValue(3);
		m_spinBox[i]->setSingleStep(2);

		// assemble dialog
		layout->addWidget(label    [i], i, 0);
		layout->addWidget(m_slider [i], i, 1);
		layout->addWidget(m_spinBox[i], i, 2);
	}

	// update factor slider properties
	m_slider [1]->setValue(1);
	m_spinBox[1]->setValue(1);
	m_slider [1]->setSingleStep(1);
	m_spinBox[1]->setSingleStep(1);

	// init signal/slot connections
	connect(m_slider [0], SIGNAL(valueChanged(int)), this, SLOT(changeSize  (int)));
	connect(m_spinBox[0], SIGNAL(valueChanged(int)), this, SLOT(changeSize  (int)));
	connect(m_slider [1], SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));
	connect(m_spinBox[1], SIGNAL(valueChanged(int)), this, SLOT(changeFactor(int)));

	// assign layout to group box
	m_ctrlGrp->setLayout(layout);

	return(m_ctrlGrp);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::applyFilter:
//
// Run filter on the image, transforming I1 to I2.
// Overrides ImageFilter::applyFilter().
// Return 1 for success, 0 for failure.
//
bool
Sharpen::applyFilter(ImagePtr I1, ImagePtr I2)
{
	// error checking
	if(I1.isNull()) return 0;

	// collect parameters
	int	size   = m_slider[0]->value();	// filter size
	double	factor = m_slider[1]->value();	// factor

	// apply sharpen filter
	sharpen(I1, size, factor, I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::sharpen:
//
// Sharpen image I1 using a square box filter of dimension size.
// Multiply the difference between I1 and the blurred image by factor
// before adding it back to I1 to produce the output.
// Output is in I2.
//
void
Sharpen::sharpen(ImagePtr I1, int size, double factor, ImagePtr I2)
{
	HW_sharpen(I1, size, factor, I2);
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::changeSize:
//
// Slot to process change in filter size caused by moving the slider.
//
void
Sharpen::changeSize(int value)
{
	m_slider [0]->blockSignals(true);
	m_slider [0]->setValue    (value);
	m_slider [0]->blockSignals(false);
	m_spinBox[0]->blockSignals(true);
	m_spinBox[0]->setValue    (value);
	m_spinBox[0]->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::changeFactor:
//
// Slot to process change in factor caused by moving the slider.
//
void
Sharpen::changeFactor(int value)
{
	m_slider [1]->blockSignals(true);
	m_slider [1]->setValue    (value);
	m_slider [1]->blockSignals(false);
	m_spinBox[1]->blockSignals(true);
	m_spinBox[1]->setValue    (value);
	m_spinBox[1]->blockSignals(false);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen::reset:
//
// Reset parameters.
//
void
Sharpen::reset()
{
	m_slider[0]->setValue(3);
	m_slider[1]->setValue(1);

	// apply filter to source image and display result
	g_mainWindowP->preview();
}
