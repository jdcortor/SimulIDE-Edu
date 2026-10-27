/***************************************************************************
 * Sandbox — Plantilla de Prototipado Rápido para Nuevos Componentes       *
 * Use este archivo como base para diseñar visualmente cualquier nuevo     *
 * componente (diodos, capacitores, fuentes, sensores, etc.) en 1 segundo  *
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
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <cmath>

// =========================================================================
// AQUÍ SE DIBUJA EL NUEVO ELEMENTO (Modifique esta función libremente)
// =========================================================================
void drawComponent( QPainter* p, double param1, double param2, int variant )
{
    p->save();
    p->setRenderHint( QPainter::Antialiasing, true );

    // 1. Dibujar terminales de conexión (sobre la retícula de 8 px)
    QPen wirePen( QColor( 0, 0, 0 ), 1.8, Qt::SolidLine, Qt::RoundCap );
    p->setPen( wirePen );
    p->drawLine( QPointF( -32.0, 0.0 ), QPointF( -16.0, 0.0 ) );
    p->drawLine( QPointF(  16.0, 0.0 ), QPointF(  32.0, 0.0 ) );

    // 2. Dibujar cuerpo del componente (ejemplo: caja o silueta)
    QRectF bodyRect( -16.0, -12.0, 32.0, 24.0 );

    QLinearGradient grad( 0.0, -12.0, 0.0, 12.0 );
    grad.setColorAt( 0.0, QColor( 255, 255, 255 ) );
    grad.setColorAt( 1.0, QColor( 230, 235, 240 ) );

    p->setBrush( grad );
    p->setPen( QPen( QColor( 80, 85, 90 ), 1.2 ) );
    p->drawRoundedRect( bodyRect, 2.0, 2.0 );

    // 3. Serigrafía / texto
    QFont font = p->font();
    font.setFamily( "Monospace" );
    font.setBold( true );
    font.setPointSizeF( 7.0 );
    p->setFont( font );
    p->setPen( QColor( 20, 20, 20 ) );
    p->drawText( bodyRect, Qt::AlignCenter, "NUEVO" );

    p->restore();
}

// =========================================================================
// LIENZO INTERACTIVO CON RETÍCULA Y ZOOM
// =========================================================================
class CanvasWidget : public QWidget
{
public:
    double param1   = 1.0;
    double param2   = 10.0;
    int    variant  = 0;
    double zoom     = 3.0;
    bool   showGrid = true;

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
        render( &p, w, h );
        p.end();
        pix.save( filename );
    }

protected:
    void paintEvent( QPaintEvent* ) override
    {
        QPainter p( this );
        p.fillRect( rect(), QColor( 242, 244, 248 ) );
        render( &p, width(), height() );
    }

    void render( QPainter* p, int w, int h )
    {
        int cx = w / 2;
        int cy = h / 2;

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
        drawComponent( p, param1, param2, variant );
        p->restore();
    }
};

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );

    QWidget mainWin;
    mainWin.setWindowTitle( "Sandbox — Diseñador de Nuevo Elemento" );
    mainWin.resize( 640, 420 );

    QVBoxLayout* rootLayout = new QVBoxLayout( &mainWin );
    CanvasWidget* canvas = new CanvasWidget( &mainWin );
    rootLayout->addWidget( canvas, 1 );

    QHBoxLayout* controls = new QHBoxLayout();

    controls->addWidget( new QLabel( "Tipo / Variante:" ) );
    QComboBox* comboVariant = new QComboBox();
    comboVariant->addItems( { "Variante A", "Variante B", "Variante C" } );
    controls->addWidget( comboVariant );

    controls->addWidget( new QLabel( "Parámetro 1:" ) );
    QDoubleSpinBox* spin1 = new QDoubleSpinBox();
    spin1->setRange( 0.0, 1000.0 );
    spin1->setValue( 1.0 );
    controls->addWidget( spin1 );

    controls->addWidget( new QLabel( "Zoom:" ) );
    QComboBox* comboZoom = new QComboBox();
    comboZoom->addItems( { "1x", "2x", "3x", "4x", "5x" } );
    comboZoom->setCurrentIndex( 2 ); // 3x
    controls->addWidget( comboZoom );

    QCheckBox* chkGrid = new QCheckBox( "Cuadrícula" );
    chkGrid->setChecked( true );
    controls->addWidget( chkGrid );

    rootLayout->addLayout( controls );

    auto updateCanvas = [=]() {
        canvas->variant = comboVariant->currentIndex();
        canvas->param1  = spin1->value();
        canvas->zoom    = comboZoom->currentIndex() + 1.0;
        canvas->showGrid = chkGrid->isChecked();
        canvas->update();
    };

    QObject::connect( comboVariant, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ updateCanvas(); } );
    QObject::connect( spin1,        QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double){ updateCanvas(); } );
    QObject::connect( comboZoom,    QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int){ updateCanvas(); } );
    QObject::connect( chkGrid,      &QCheckBox::toggled, [=](bool){ updateCanvas(); } );

    updateCanvas();
    mainWin.show();

    return app.exec();
}
