#include "heuristicsettings.h"
#include "maxrects.h"
#include <QApplication>
#include <iostream>

#define PR(x) QApplication::translate("Preferences", x, Q_NULLPTR)
#define HT(x) QApplication::translate("HelpText", x, Q_NULLPTR)

QStringList g_heuristicStrings()
{
	return QStringList()
		<< PR("None")
		<< PR("Top left")
		<< PR("Best area")
		<< PR("Best short side")
		<< PR("Best long side")
		<< PR("Min width")
		<< PR("Min height");
}

QString     g_heuristicToolTip()
{
	return QString();
}


int MaxRects::calculateHeuristic(QRect itr, QSize img, bool & leftNeighbor, bool & rightNeighbor) const
{
	switch(heuristic)
	{
    case NONE: return 0;
	case TL:
	{
		int m = itr.y();
		for(int k = 0; k < R.size(); k++)
		{
			if(qAbs(R.at(k).y() + R.at(k).height() / 2 - itr.y() - itr.height() / 2) <
					qMax(R.at(k).height(), itr.height()) / 2)
			{
				if(R.at(k).x() + R.at(k).width() == itr.x())
				{
					m -= 5;
					leftNeighbor = true;
				}
				if(R.at(k).x() == itr.x() + itr.width())
				{
					m -= 5;
					rightNeighbor = true;
				}
			}
		}
		if(leftNeighbor || rightNeighbor == false)
		{
			if(itr.x() + itr.width() == w)
			{
				m -= 1;
				rightNeighbor = true;
			}
			if(itr.x() == 0)
			{
				m -= 1;
				leftNeighbor = true;
			}
		}

		return m;
	} break;
	case BAF:  return itr.width() * itr.height();
	case BSSF: return qMin(itr.width() - img.width(), itr.height() - img.height());
	case BLSF: return qMax(itr.width() - img.width(), itr.height() - img.height());
	case MINW: return itr.width();
	case MINH: return itr.height();
	default: break;
	}

	return 0;
}
