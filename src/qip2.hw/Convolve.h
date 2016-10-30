// ======================================================================
// IMPROC: Image Processing Software Package
// Copyright (C) 2016 by George Wolberg
//
// Convolve.h - Convolve widget
//
// Written by: George Wolberg, 2016
// ======================================================================

#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "ImageFilter.h"

class Convolve : public ImageFilter {
	Q_OBJECT

public:
	Convolve			(QWidget *parent = 0);	// constructor
	QGroupBox*	controlPanel	();			// create control panel
	bool		applyFilter	(ImagePtr, ImagePtr);	// apply filter to input
	void		convolve	(ImagePtr, ImagePtr, ImagePtr);

protected slots:
	int		load		();

private:
	// widgets
	QPushButton*	m_button;	// Convolve pushbutton
	QTextEdit*	m_values;	// text field for kernel values
	QGroupBox*	m_ctrlGrp;	// groupbox for panel

	// variables
	QString		m_file;
	QString		m_currentDir;
	ImagePtr	m_kernel;
};

#endif	// CONVOLVE_H
