/****************************************************************************
** Form implementation generated from reading ui file 'EntityGUI_2Spin_QTD.ui'
**
** Created: Fri Jul 30 16:06:00 2004
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "EntityGUI_2Spin_QTD.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a EntityGUI_2Spin_QTD which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
EntityGUI_2Spin_QTD::EntityGUI_2Spin_QTD( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "EntityGUI_2Spin_QTD" );
    resize( 255, 112 ); 
    setCaption( trUtf8( "EntityGUI_2Spin_QTD" ) );
    EntityGUI_2Spin_QTDLayout = new QGridLayout( this, 1, 1, 0, 6, "EntityGUI_2Spin_QTDLayout"); 

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setTitle( trUtf8( "Values" ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 6 );
    GroupBox1->layout()->setMargin( 11 );
    GroupBox1Layout = new QGridLayout( GroupBox1->layout() );
    GroupBox1Layout->setAlignment( Qt::AlignTop );

    Layout4 = new QGridLayout( 0, 1, 1, 0, 6, "Layout4"); 

    Layout1 = new QGridLayout( 0, 1, 1, 0, 6, "Layout1"); 

    TextLabel2 = new QLabel( GroupBox1, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setText( trUtf8( "TL2" ) );

    Layout1->addWidget( TextLabel2, 1, 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, TextLabel1->sizePolicy().hasHeightForWidth() ) );
    TextLabel1->setText( trUtf8( "TL1" ) );

    Layout1->addWidget( TextLabel1, 0, 0 );

    SpinBox1 = new QSpinBox( GroupBox1, "SpinBox1" );
    SpinBox1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox1->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox1, 0, 1 );

    SpinBox2 = new QSpinBox( GroupBox1, "SpinBox2" );
    SpinBox2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)7, (QSizePolicy::SizeType)0, 0, 0, SpinBox2->sizePolicy().hasHeightForWidth() ) );

    Layout1->addWidget( SpinBox2, 1, 1 );
    QSpacerItem* spacer = new QSpacerItem( 0, 82, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout1->addItem( spacer, 3, 1 );

    Layout4->addLayout( Layout1, 0, 0 );

    Layout3 = new QGridLayout( 0, 1, 1, 0, 6, "Layout3"); 
    QSpacerItem* spacer_2 = new QSpacerItem( 0, 51, QSizePolicy::Minimum, QSizePolicy::Expanding );
    Layout3->addItem( spacer_2, 2, 0 );

    Layout2 = new QGridLayout( 0, 1, 1, 0, 6, "Layout2"); 

    buttonUndo = new QPushButton( GroupBox1, "buttonUndo" );
    buttonUndo->setText( trUtf8( "Undo" ) );

    Layout2->addWidget( buttonUndo, 0, 0 );

    buttonRedo = new QPushButton( GroupBox1, "buttonRedo" );
    buttonRedo->setText( trUtf8( "Redo" ) );

    Layout2->addWidget( buttonRedo, 0, 1 );

    Layout3->addLayout( Layout2, 1, 0 );

    buttonApply = new QPushButton( GroupBox1, "buttonApply" );
    buttonApply->setText( trUtf8( "Create" ) );

    Layout3->addWidget( buttonApply, 0, 0 );

    Layout4->addLayout( Layout3, 0, 1 );

    GroupBox1Layout->addLayout( Layout4, 0, 0 );

    EntityGUI_2Spin_QTDLayout->addWidget( GroupBox1, 0, 0 );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
EntityGUI_2Spin_QTD::~EntityGUI_2Spin_QTD()
{
    // no need to delete child widgets, Qt does it all for us
}
