/***************************************************************************
 *   Copyright (C) 2026 by SimulIDE-Edu                                    *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#pragma once

#include <QString>
#include <QColor>

#include "e-resistor.h"
#include "comp2pin.h"

class LibraryItem;

class CeramicResistor : public Comp2Pin, public eResistor
{
    public:
        CeramicResistor( QString type, QString id );
        virtual ~CeramicResistor();

        static Component* construct( QString type, QString id );
        static LibraryItem* libraryItem();

        void paint( QPainter* p, const QStyleOptionGraphicsItem* o, QWidget* w ) override;

        void setResistance( double resist ) override;

        double powerRating() { return m_powerRating; }
        void setPowerRating( double p ) override;

        QString powerRatingStr();
        void setPowerRatingStr( QString str );

        double tolerance() { return m_tolerance; }
        void setTolerance( double t );

        QString toleranceStr();
        void setToleranceStr( QString str );

        bool realistic() { return m_realistic; }
        void setRealistic( bool r );

    protected:
        void updateDimensions();
        void drawRealistic( QPainter* p );
        static QString formatValueCode( double R );
        static QString toleranceLetter( double tol );

        double m_tolerance;   // 1, 2, 5, 10, 20 %
        bool   m_realistic;

        double m_bodyLength;
        double m_bodyHeight;
        double m_pinDistance; // half-span of pins from center
};
