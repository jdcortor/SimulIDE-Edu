/***************************************************************************
 *   Copyright (C) 2012 by Santiago González                               *
 *                                                                         *
 *   ( see copyright.txt file at root folder )                             *
 ***************************************************************************/

#pragma once

#include "e-element.h"

class eResistor : public eElement
{
    public:
        eResistor( QString id );
        virtual ~eResistor();

        virtual void stamp() override;

        virtual double resistance() { return m_nominalResistance; }
        virtual void setResistance( double resist );

        virtual void setResSafe( double resist );
        
        double admit() { return m_admit; }
        void setAdmit( double admit );
        void stampAdmit();

        double current();

        double powerRating() { return m_powerRating; }
        virtual void setPowerRating( double p ) { m_powerRating = p; }

    protected:
        virtual void updateVI();

        double m_admit;
        double m_current;
        double m_powerRating;
        double m_nominalResistance;
};
