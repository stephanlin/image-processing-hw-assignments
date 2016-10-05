// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Blur.h - Blur widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef BLUR_H
#define BLUR_H

#include "ImageFilter.h"

class Blur : public ImageFilter {
	Q_OBJECT

public:
	Blur				(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, ImagePtr);	// apply filter to input
	void		reset		();			// reset parameters
	void		blur		(ImagePtr, int, int, ImagePtr);

protected slots:
	void		changeFilterW	(int);
	void		changeFilterH	(int);
	void		setLock		(int);

private:
	// widgets
	QSlider*	m_slider [2];	// Blur sliders
	QSpinBox*	m_spinBox[2];	// Blur spin boxes
	QCheckBox*	m_checkBox;	// Blur check box
	QGroupBox*	m_ctrlGrp;	// groupbox for panel
};

#endif	// BLUR_H
