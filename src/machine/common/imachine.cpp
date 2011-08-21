#include "imachine.h"

IMachine::IMachine(QObject *parent) :
	QObject(parent),
	m_frameRate(1),
	m_audioEnable(true),
	m_audioStereoEnable(true),
	m_audioSampleRate(22050) {
}

IMachine::~IMachine() {
}

void IMachine::setFrameRate(qreal rate)
{ m_frameRate = rate; }
