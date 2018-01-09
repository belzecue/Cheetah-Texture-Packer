#ifndef SIZECONSTRAINTSETTINGS_H
#define SIZECONSTRAINTSETTINGS_H
#include <QStringList>
#include <QAbstractSpinBox>
#include <QString>


QStringList g_sizeConstraintStrings();
QString     g_sizeConstraintToolTip();

int g_roundConstrainedValue(int constraint, int v);
int g_stepConstrainedValue(int constraint, int v, int step);
bool g_testConstrainedValue(int constraint, int v);
bool g_canTypeConstraint(int constraint);

#endif // SIZECONSTRAINTSETTINGS_H
