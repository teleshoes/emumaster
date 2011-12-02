#include "hostinputdevice.h"

QString HostInputDevice::conf() const {
	if (m_confIndex >= 0)
		return m_confList.at(m_confIndex);
	else
		return tr("None");
}
