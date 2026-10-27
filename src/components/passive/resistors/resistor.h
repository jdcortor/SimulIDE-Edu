/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#pragma once

#include <vector>
#include <QColor>
#include <QString>

#include "e-resistor.h"
#include "comp2pin.h"

class LibraryItem;

class Resistor : public Comp2Pin, public eResistor
{
    public:
        Resistor( QString type, QString id );
        virtual ~Resistor();

        static Component* construct( QString type, QString id );
        static LibraryItem* libraryItem();

        static void drawAnsi( QPainter* p, int x, int y, double sX=1, double sY=1 );

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void setResistance( double resist ) override;

        double powerRating() { return m_powerRating; }
        void setPowerRating( double p ) override;

        double tolerance() { return m_tolerance; }
        void setTolerance( double t );

        QString toleranceStr();
        void setToleranceStr( QString str );

        int bands() { return m_bands; }
        void setBands( int b );

        QString bandsStr();
        void setBandsStr( QString str );

        int tcr() { return m_tcr; }
        void setTcr( int t );

        QString tcrStr();
        void setTcrStr( QString str );

        bool realistic() { return m_realistic; }
        void setRealistic( bool r );

    protected:
        void slotProperties() override;
        void updateBands();
        void updateDimensions();
        void drawRealistic( QPainter* p );

        static QColor colorForDigit( int d );
        static QColor colorForMultiplier( int m );
        static QColor colorForTolerance( double tol );
        static QColor colorForTcr( int tcr );

        double m_tolerance;
        int    m_bands;       // 4, 5, or 6
        int    m_tcr;         // 100, 50, 25, 15, 10, 5 ppm/K
        bool   m_realistic;

        double m_bodyLength;
        double m_bodyRadius;
        std::vector<QColor> m_bandColors;
};
