/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2017 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef DIALOG_FILTER_FREQUENCIES_H
#define DIALOG_FILTER_FREQUENCIES_H

#include <QDialog>
#include "qcustomplot.h"

namespace Ui {
class DialogFilterFrequencies;
}

class DialogFilterFrequencies : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFilterFrequencies(QWidget *parent = 0);
    ~DialogFilterFrequencies();
    void setNbFourier(int nbFourier);
    void addFourierTransform(QByteArray baData, quint32 sampleRate);

signals:
    void accepted(QVector<double> dValues);

private slots:
    void accept();

private:
    QVector<double> getStoredCurve();
    void storeCurve(QVector<double> val);

    Ui::DialogFilterFrequencies *ui;
};

class GraphFilterFrequencies : public QCustomPlot
{
    Q_OBJECT

public:
    explicit GraphFilterFrequencies(QWidget *parent = 0);
    ~GraphFilterFrequencies();

    bool eventFilter(QObject* o, QEvent* e)
    {
        if ((e->type() == QEvent::MouseMove ||
             e->type() == QEvent::MouseButtonPress ||
             e->type() == QEvent::MouseButtonRelease ||
             e->type() == QEvent::Leave)
                && o == this)
        {
            QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(e);
            QPoint pos = mouseEvent->pos();
            if (mouseEvent->type() == QEvent::MouseMove)
                this->mouseMoved(pos);
            else if (mouseEvent->type() == QEvent::Leave)
                this->mouseLeft();
            else if (mouseEvent->button() == Qt::LeftButton)
            {
                if (mouseEvent->type() == QEvent::MouseButtonPress)
                    this->mousePressed(pos);
                else if (mouseEvent->type() == QEvent::MouseButtonRelease)
                    this->mouseReleased(pos);
            }
            return true;
        }
        return false;
    }

    void setNbFourier(int nbFourier) { _nbFourier = nbFourier; }
    void addFourierTransform(QVector<float> fData, quint32 sampleRate);
    QVector<double> getValues();
    void setValues(QVector<double> val);

private:
    QVector<double> dValues;
    bool flagEdit;
    void replot();
    double raideurExp;
    QCPItemText * labelCoord;
    int previousX;
    double previousY;
    int _nbFourier;

    void mousePressed(QPoint pos);
    void mouseReleased(QPoint pos);
    void mouseMoved(QPoint pos);
    void mouseLeft();
    void write(QPoint pos);
    void afficheCoord(double x, double y);

    static const int POINT_NUMBER;
};

#endif // DIALOG_FILTER_FREQUENCIES_H
