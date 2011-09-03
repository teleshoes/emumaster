#include "m6502.h"
#include <QDataStream>

// TODO remove log
//#include <QFile>
//static  QFile cpuLog("cpu.log");

M6502::M6502(QObject *parent) :
	QObject(parent),
	A(0), Y(0), X(0), PC(0), S(0xFF), P(0),
	m_nmiState(false) {
	m_signals = Reset;
	qMemSet(ZNTable + 0x00, 0, 0x80);
	qMemSet(ZNTable + 0x80, N, 0x80);
	ZNTable[0] = Z;

//	cpuLog.open(QIODevice::ReadWrite|QIODevice::Truncate);
}

uint M6502::executeOne() {
	m_currentCycles = 0;
	quint8 opcode = READ(PC);
	ADDCYC(cyclesTable[opcode]);
	PC++;

//	cpuLog.write(qPrintable(QString::number(opcode, 16)));

//	cpuLog.write(" PC=");
//	cpuLog.write(qPrintable(QString::number(PC, 16)));
//	cpuLog.write(" A=");
//	cpuLog.write(qPrintable(QString::number(A, 16)));
//	cpuLog.write(" Y=");
//	cpuLog.write(qPrintable(QString::number(Y, 16)));
//	cpuLog.write(" X=");
//	cpuLog.write(qPrintable(QString::number(X, 16)));
//	cpuLog.write(" S=");
//	cpuLog.write(qPrintable(QString::number(S, 16)));

//	cpuLog.write(" [S+1]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x101), 16)));
//	cpuLog.write(" [S+2]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x102), 16)));
//	cpuLog.write(" [S+3]=");
//	cpuLog.write(qPrintable(QString::number(READ(S+0x103), 16)));

//	cpuLog.write(" [PC+1]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+0), 16)));
//	cpuLog.write(" [PC+2]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+1), 16)));
//	cpuLog.write(" [PC+3]=");
//	cpuLog.write(qPrintable(QString::number(READ(PC+2), 16)));

	execute(opcode);

//	cpuLog.write(" PCa=");
//	cpuLog.write(qPrintable(QString::number(PC, 16)));
//	cpuLog.write("\n");

	if (m_signals) {
		if ((m_signals & (Reset | Nmi)) || !(P & I)) {
			uint vector;
			if (m_signals & Reset) {
				vector = ResetVectorAddress;
				P = I;
				A = 0;
				Y = 0;
				X = 0;
				S = 0xFF;
			} else if (m_signals & Nmi) {
				vector = NmiVectorAddress;
				m_signals &= ~Nmi;
//				cpuLog.write("nmi\n");
			} else {
				vector = IrqVectorAddress;
//				cpuLog.write("irq\n");
			}
			ADDCYC(7);
			PUSH(PC >> 8);
			PUSH(PC);
			PUSH((P & ~B) | U);
			P |= I;
			PC = READ(vector);
			PC |= READ(vector + 1) << 8;
		}
	}
	return m_currentCycles;
}

void M6502::setSignal(SignalIn sig, bool on) {
	if (on)
		m_signals |=  sig;
	else
		m_signals &= ~sig;
}

void M6502::irq0_i(bool on)
{ setSignal(Irq0, on); }

void M6502::nmi_i(bool on) {
	if (on && !m_nmiState)
		setSignal(Nmi, true);
	m_nmiState = on;
}
void M6502::reset_i(bool on)
{ setSignal(Reset, on); }

void M6502::setPC(quint16 address)
{ PC = address; }

bool M6502::save(QDataStream &s) {
	s << QString("M6502");
	s << int(m_signals);
	s << A;
	s << Y << X;
	s << PC;
	s << S;
	s << P;
	s << m_nmiState;
	return true;
}

bool M6502::load(QDataStream &s) {
	QString header;
	s >> header;
	if (header != "M6502")
		return false;
	int sigs;
	s >> sigs;
	m_signals = static_cast<SignalIn>(sigs);
	s >> A;
	s >> Y >> X;
	s >> PC;
	s >> S;
	s >> P;
	s >> m_nmiState;
	return true;
}
