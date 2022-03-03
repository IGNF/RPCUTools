//-----------------------------------------------------------------------------
//								XWaitConsole.h
//								===============
//
// Auteur : F.Becirspahic - IGN / DSTI / SIMV
//
// Date : 07/4/2021
//-----------------------------------------------------------------------------

#ifndef XWAITCONSOLE_H
#define XWAITCONSOLE_H

#include "XBase.h"
#include <iostream>

class XWaitConsole : public XWait {
protected:
	int		m_nPercent;

public:
	XWaitConsole() { m_nPercent = 0; }

	virtual void StepIt() {
		int percent = 100 * m_nStep / (m_nMax - m_nMin);
		m_nStep++;
		if (percent == m_nPercent) return;
		m_nPercent = percent;
		std::cout << "\r" << m_nPercent << "%";
	}
	virtual void SetStatus(const char* s) { std::cout << s << std::endl; }

};

#endif //XWAITCONSOLE_H