/***************************************************************************
 * Sandbox - Previsualizador Rápido de Resistencia Cerámica                *
 * Permite editar el código de dibujo y ver el resultado en 1 segundo      *
 * sin necesidad de compilar todo el proyecto SimulIDE.                    *
 ***************************************************************************/

#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QFont>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <cmath>

// --- Funciones de formato exactas de CeramicResistor ---

static QString toleranceLetter( double tol )
{
    if( std::abs( tol - 1.0 ) < 0.25 )  return "F";
    if( std::abs( tol - 2.0 ) < 0.25 )  return "G";
    if( std::abs( tol - 5.0 ) < 1.0 )   return "J";
    if( std::abs( tol - 10.0 ) < 2.0 )  return "K";
    if( std::abs( tol - 20.0 ) < 3.0 )  return "M";
    return "J";
}

static QString formatValueCode( double R )
{
    if( R <= 0.0 ) return "0R";

    if( R < 1.0 )
    {
        int cents = static_cast<int>( std::round( R * 100.0 ) );
        return QString( "0R%1" ).arg( cents, 2, 10, QChar('0') );
    }
    else if( R < 10.0 )
    {
        int whole = static_cast<int>( std::floor( R ) );
        int frac  = static_cast<int>( std::round( (R - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1R" ).arg( whole );
        return QString( "%1R%2" ).arg( whole ).arg( frac );
    }
    else if( R < 1000.0 )
    {
        int whole = static_cast<int>( std::floor( R ) );
        int frac  = static_cast<int>( std::round( (R - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1R" ).arg( whole );
        return QString( "%1R%2" ).arg( whole ).arg( frac );
    }
    else if( R < 1e6 )
    {
        double k = R / 1000.0;
        int whole = static_cast<int>( std::floor( k ) );
        int frac  = static_cast<int>( std::round( (k - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1K" ).arg( whole );
        return QString( "%1K%2" ).arg( whole ).arg( frac );
    }
    else
    {
        double m = R / 1e6;
        int whole = static_cast<int>( std::floor( m ) );
        int frac  = static_cast<int>( std::round( (m - whole) * 10.0 ) );
        if( frac == 0 ) return QString( "%1M" ).arg( whole );
        return QString( "%1M%2" ).arg( whole ).arg( frac );
    }
}

// --- FUNCIÓN DE DIBUJO (Copia exacta de CeramicResistor::drawRealistic) ---
void drawCeramicResistor( QPainter* p, double powerRating, double resistance, double tolerance )
{
    double m_bodyLength  = 34.0;
    double m_bodyHeight  = 16.0;
    double m_pinDistance = 24.0;

    if( powerRating <= 5.0 )
    {
        m_bodyLength  = 34.0;
        m_bodyHeight  = 16.0;
        m_pinDistance = 24.0; // 48 px pin-to-pin = 6 grid cells
    }
    else if( powerRating <= 7.0 )
    {
        m_bodyLength  = 44.0;
        m_bodyHeight  = 17.0;
        m_pinDistance = 32.0; // 64 px pin-to-pin = 8 grid cells
    }
    else if( powerRating <= 10.0 )
    {
        m_bodyLength  = 52.0;
        m_bodyHeight  = 18.0;
        m_pinDistance = 40.0; // 80 px pin-to-pin = 10 grid cells
    }
    else if( powerRating <= 15.0 )
    {
        m_bodyLength  = 62.0;
        m_bodyHeight  = 20.0;
        m_pinDistance = 48.0; // 96 px pin-to-pin = 12 grid cells
    }
    else if( powerRating <= 20.0 )
    {
        m_bodyLength  = 72.0;
        m_bodyHeight  = 22.0;
        m_pinDistance = 56.0; // 112 px pin-to-pin = 14 grid cells
    }
    else // 25 W+
    {
        m_bodyLength  = 84.0;
        m_bodyHeight  = 24.0;
        m_pinDistance = 64.0; // 128 px pin-to-pin = 16 grid cells
    }

    p->save();
    p->setRenderHint( QPainter::Antialiasing, true );

    double L = m_bodyLength;
    double H = m_bodyHeight;
    double halfL = L / 2.0;
    double halfH = H / 2.0;
    double pinX  = m_pinDistance;

    // 1. Draw shiny metallic axial lead wires (only horizontal)
    QPen wirePen( QColor( 0, 0, 0 ), 1.8, Qt::SolidLine, Qt::RoundCap );
    p->setPen( wirePen );
    p->drawLine( QPointF( -pinX, 0.0 ), QPointF( -halfL, 0.0 ) );
    p->drawLine( QPointF(  halfL, 0.0 ), QPointF(  pinX, 0.0 ) );

    // 2. Ceramic refractory cement body (pure white ceramic with subtle 3D depth)
    QRectF bodyRect( -halfL, -halfH, L, H );

    QLinearGradient blockGrad( 0.0, -halfH, 0.0, halfH );
    blockGrad.setColorAt( 0.0,  QColor( 255, 255, 255 ) );
    blockGrad.setColorAt( 0.3,  QColor( 255, 255, 255 ) );
    blockGrad.setColorAt( 0.85, QColor( 246, 247, 249 ) );
    blockGrad.setColorAt( 1.0,  QColor( 232, 234, 238 ) ); // Suave sombra inferior

    p->setBrush( blockGrad );
    p->setPen( QPen( QColor( 160, 165, 172 ), 1.0 ) );
    p->drawRoundedRect( bodyRect, 2.0, 2.0 );

    // Top edge highlight
    p->setPen( QPen( QColor( 255, 255, 255 ), 0.8 ) );
    p->drawLine( QPointF( -halfL + 2.0, -halfH + 1.0 ), QPointF( halfL - 2.0, -halfH + 1.0 ) );

    // Bottom edge subtle shadow
    p->setPen( QPen( QColor( 200, 204, 210, 180 ), 0.8 ) );
    p->drawLine( QPointF( -halfL + 2.0, halfH - 1.0 ), QPointF( halfL - 2.0, halfH - 1.0 ) );

    // 3. Industrial stamped factory legend (e.g. "5W47RJ")
    int ratedW = static_cast<int>( std::round( powerRating ) );
    QString legend = QString( "%1W%2%3" ).arg( ratedW )
                                         .arg( formatValueCode( resistance ) )
                                         .arg( toleranceLetter( tolerance ) );

    QFont font = p->font();
    font.setFamily( "Monospace" );
    font.setStyleHint( QFont::Monospace );
    font.setBold( true );
    font.setPointSizeF( (H <= 16.0) ? 6.5 : 7.8 );
    p->setFont( font );

    p->setPen( QColor( 25, 25, 25 ) ); // Tinta negra industrial estampada
    p->drawText( bodyRect, Qt::AlignCenter, legend );

    p->restore();
}

// --- Lienzo interactivo ---
class CanvasWidget : public QWidget
{
public:
    double powerRating = 5.0;
    double resistance  = 47.0;
    double tolerance   = 5.0;
    double zoom        = 3.0;
    bool   showGrid    = true;

    CanvasWidget( QWidget* parent = nullptr ) : QWidget( parent )
    {
        setMinimumSize( 500, 260 );
    }

    void saveImage( const QString& filename )
    {
        int w = width() > 100 ? width() : 600;
        int h = height() > 100 ? height() : 260;
        QPixmap pix( w, h );
        pix.fill( QColor( 242, 244, 248 ) );
        QPainter p( &pix );
        renderResistor( &p, w, h );
        p.end();
        pix.save( filename );
    }

protected:
    void paintEvent( QPaintEvent* ) override
    {
        QPainter p( this );
        p.fillRect( rect(), QColor( 242, 244, 248 ) );
        renderResistor( &p, width(), height() );
    }

    void renderResistor( QPainter* p, int w, int h )
    {
        int cx = w / 2;
        int cy = h / 2;

        // Cuadrícula SimulIDE (8px escalada por zoom)
        if( showGrid )
        {
            p->save();
            p->setPen( QPen( QColor( 215, 220, 228 ), 1.0, Qt::DotLine ) );
            double step = 8.0 * zoom;
            for( double x = cx; x < w; x += step )
                p->drawLine( QPointF( x, 0 ), QPointF( x, h ) );
            for( double x = cx; x > 0; x -= step )
                p->drawLine( QPointF( x, 0 ), QPointF( x, h ) );
            for( double y = cy; y < h; y += step )
                p->drawLine( QPointF( 0, y ), QPointF( w, y ) );
            for( double y = cy; y > 0; y -= step )
                p->drawLine( QPointF( 0, y ), QPointF( w, y ) );
            p->restore();
        }

        p->save();
        p->translate( cx, cy );
        p->scale( zoom, zoom );
        drawCeramicResistor( p, powerRating, resistance, tolerance );
        p->restore();
    }
};

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );

    QWidget mainWin;
    mainWin.setWindowTitle( "Preview Resistencia Cerámica (Standalone Sandbox)" );
    mainWin.resize( 640, 420 );

    QVBoxLayout* rootLayout = new QVBoxLayout( &mainWin );
    CanvasWidget* canvas = new CanvasWidget( &mainWin );
    rootLayout->addWidget( canvas, 1 );

    // Panel de controles interactivos
    QHBoxLayout* controls = new QHBoxLayout();

    // Potencia
    controls->addWidget( new QLabel( "Potencia:" ) );
    QComboBox* comboPower = new QComboBox();
    comboPower->addItems( { "5 W", "7 W", "10 W", "15 W", "20 W", "25 W" } );
    controls->addWidget( comboPower );

    // Resistencia
    controls->addWidget( new QLabel( "Valor (Ω):" ) );
    QDoubleSpinBox* spinRes = new QDoubleSpinBox();
    spinRes->setRange( 0.01, 1000000.0 );
    spinRes->setValue( 47.0 );
    spinRes->setDecimals( 2 );
    controls->addWidget( spinRes );

    // Tolerancia
    controls->addWidget( new QLabel( "Tolerancia:" ) );
    QComboBox* comboTol = new QComboBox();
    comboTol->addItems( { "±1 % (F)", "±2 % (G)", "±5 % (J)", "±10 % (K)", "±20 % (M)" } );
    comboTol->setCurrentIndex( 2 ); // ±5% (J)
    controls->addWidget( comboTol );

    // Zoom
    controls->addWidget( new QLabel( "Zoom:" ) );
    QComboBox* comboZoom = new QComboBox();
    comboZoom->addItems( { "1x", "2x", "3x", "4x", "5x" } );
    comboZoom->setCurrentIndex( 2 ); // 3x
    controls->addWidget( comboZoom );

    // Cuadrícula
    QCheckBox* chkGrid = new QCheckBox( "Cuadrícula" );
    chkGrid->setChecked( true );
    controls->addWidget( chkGrid );

    rootLayout->addLayout( controls );

    // Botón para exportar imagen
    QHBoxLayout* btnLayout = new QHBoxLayout();
    QPushButton* btnExport = new QPushButton( "Guardar 'ceramic_preview.png'" );
    QLabel* lblStatus = new QLabel( "" );
    lblStatus->setStyleSheet( "color: green; font-weight: bold;" );
    btnLayout->addWidget( btnExport );
    btnLayout->addWidget( lblStatus );
    btnLayout->addStretch();
    rootLayout->addLayout( btnLayout );

    // Eventos lambda
    auto updateCanvas = [=]() {
        QString pStr = comboPower->currentText();
        pStr.remove( "W" ).remove( " " );
        canvas->powerRating = pStr.toDouble();

        canvas->resistance = spinRes->value();

        int tolIdx = comboTol->currentIndex();
        double tols[] = { 1.0, 2.0, 5.0, 10.0, 20.0 };
        canvas->tolerance = tols[tolIdx];

        int zIdx = comboZoom->currentIndex();
        canvas->zoom = zIdx + 1.0;

        canvas->showGrid = chkGrid->isChecked();
        canvas->update();
        canvas->saveImage( QCoreApplication::applicationDirPath() + "/ceramic_preview.png" );
    };

    QObject::connect( comboPower, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ updateCanvas(); } );
    QObject::connect( spinRes,    QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double){ updateCanvas(); } );
    QObject::connect( comboTol,   QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ updateCanvas(); } );
    QObject::connect( comboZoom,  QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ updateCanvas(); } );
    QObject::connect( chkGrid,    &QCheckBox::toggled, [=](bool){ updateCanvas(); } );
    QObject::connect( btnExport,  &QPushButton::clicked, [=]() {
        canvas->saveImage( QCoreApplication::applicationDirPath() + "/ceramic_preview.png" );
        lblStatus->setText( "✓ Guardado en sandbox/ceramic_preview.png" );
    });

    updateCanvas();

    if( argc > 1 && QString( argv[1] ) == "--save" )
    {
        canvas->saveImage( QCoreApplication::applicationDirPath() + "/ceramic_preview.png" );
        return 0;
    }

    mainWin.show();

    return app.exec();
}
