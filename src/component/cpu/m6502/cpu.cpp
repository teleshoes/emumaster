#include "m6502.h"

// TODO defines to variables
#if defined(M6502_DECO16)
static const uint NmiVectorAddress		= 0xFFF0;
static const uint ResetVectorAddress	= 0xFFF2;
static const uint IrqVectorAddress		= 0xFFF4;
#else
static const uint NmiVectorAddress		= 0xFFFA;
static const uint ResetVectorAddress	= 0xFFFC;
static const uint IrqVectorAddress		= 0xFFFE;
#endif

// TODO remove #include <QFile>
//static  QFile log("cpu.log");

M6502::M6502(QObject *parent) :
	QObject(parent),
	A(0), Y(0), X(0), PC(0), S(0), P(0),
	m_nmiState(false) {
	m_signals = Reset;
	qMemSet(ZNTable + 0x00, 0, 0x80);
	qMemSet(ZNTable + 0x80, N, 0x80);
	ZNTable[0] = Z;

//	log.open(QIODevice::ReadWrite|QIODevice::Truncate);
}

uint M6502::executeOne() {
	m_currentCycles = 0;
	if (m_signals) {
		if ((m_signals & (Reset | Nmi)) || !(P & I)) {
			uint vector;
			if (m_signals & Reset) {
				vector = ResetVectorAddress;
				P = I;
			} else if (m_signals & Nmi) {
				vector = NmiVectorAddress;
				m_signals &= ~Nmi;
			} else {
				vector = IrqVectorAddress;
			}
			ADDCYC(7);
			PUSH(PC >> 8);
			PUSH(PC);
			PUSH((P & ~B) | U);
			P |= I;
			PC = READ(vector);
			PC |= READ(vector + 1) << 8;
			return m_currentCycles;
		}
	}
	quint8 opcode = READ(PC);
	ADDCYC(cyclesTable[opcode]);
	PC++;

//	log.write(qPrintable(QString::number(opcode, 16)));
//	log.write(" ");
//	log.write(instructionName(opcode));
//	for (int i = 0; i < instructionSize(opcode)-1; i++) {
//		log.write(" ");
//		log.write(qPrintable(QString::number(READ(PC + i), 16)));
//	}
//	if (opcode == 0xD0) {
//		log.write(" ");
//		log.write(qPrintable(QString::number(X, 16)));
//	}
//	log.write("\n");

	execute(opcode);
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
