#include "sizeconstraintsettings.h"
#include "maxrects.h"
#include <QApplication>
#include <QSpinBox>
#include <iostream>
#include <cmath>

#define PR(x) QApplication::translate("Preferences", x, Q_NULLPTR)
#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_sizeConstraintStrings()
{
    return QStringList()
        << PR("None")
        << PR("Multiple of 4")
        << PR("Power of two");
}

QString     g_sizeConstraintToolTip()
{
    return QString();
}

#define rndPow2(v) (v? (int)(std::log2(v) +.5) : 0)

bool g_canTypeConstraint(int constraint)
{
    return constraint <= 1;
}

int g_roundConstrainedValue(int constraint, int v)
{
    switch(constraint)
    {
    case 0: return v;
    case 1: return (v + 2) & 0xFFffFFfc; break;
    case 2: return v? 1 << rndPow2(v) : 0; break;
    default: break;
    }

    return v;
}

bool g_testConstrainedValue(int constraint, int v)
{
    switch(constraint)
    {
    case 0: return true;
    case 1: return v % 4 == 0;
    case 2: { float x = std::log2(v); return x == (int)x; }
    default: break;
    }

    return false;
}

int g_stepConstrainedValue(int constraint, int v, int step)
{
 //want it to just snap between 0 and 64
    if(step < 0 && v == 64) return -64;
    if(step > 0 && v ==  0) return  64;

    switch(constraint)
    {
    case 0: return step;
    case 1: return step * 4; break;
    case 2: return step < 0? -v/2 : v; break;
    default: break;
    }

    return step;
}
