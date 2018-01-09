#ifndef QCONSTRAINEDSPINBOX_H
#define QCONSTRAINEDSPINBOX_H
#include "preferences/sizeconstraintsettings.h"
#include <QSpinBox>

class QConstrainedSpinBox : public QSpinBox
{
    Q_OBJECT

    int m_constraint;

public:
    QConstrainedSpinBox(QWidget *parent) :
        QSpinBox(parent), m_constraint(0) {}
    virtual ~QConstrainedSpinBox() {}

    void setConstraint(int c)
    {
        m_constraint = c;
        setValue(g_roundConstrainedValue(c, value()));
    }

    void stepBy(int step) override
    {
        QSpinBox::stepBy(g_stepConstrainedValue(m_constraint, value(), step));
    }

    int valueFromText(const QString &text) const override
    {
        return g_roundConstrainedValue(m_constraint, text.toUInt());
    }

};

#endif // QCONSTRAINEDSPINBOX_H
