//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : RepairGUI_FreeBoundDlg.cxx
//  Author : Sergey LITONIN
//  Module : GEOM
//  $Header$

#include "RepairGUI_FreeBoundDlg.h"
#include "QAD_Desktop.h"
#include "GEOMBase.h"
#include "GeometryGUI.h"
#include "GEOM_Displayer.h"
#include "SALOMEGUI_QtCatchCorbaException.hxx"
#include "GEOMImpl_Types.hxx"
#include "SALOME_Prs.h"
#include "SALOME_Selection.h"

#include <qlineedit.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qpushbutton.h>

#define SPACING 5
#define MARGIN 10

/*
  Calss       : RepairGUI_FreeBoundDlg
  Description : Dialog for displaying free boundaries of selected face, shell or solid
*/


//=================================================================================
// function : RepairGUI_FreeBoundDlg
// purpose  : Constructor
//=================================================================================
RepairGUI_FreeBoundDlg::RepairGUI_FreeBoundDlg( QWidget* theParen, SALOME_Selection* theSelection )
: QDialog( theParen, "RepairGUI_FreeBoundDlg", false,
    WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose )
{
  setCaption( tr( "CAPTION" ) );

  QPixmap iconSelect( QAD_Desktop::getResourceManager()->loadPixmap( "GEOM",tr( "ICON_SELECT" ) ) );
  
  QGroupBox* aMainGrp = new QGroupBox( 1, Qt::Horizontal, tr( "FREE_BOUND" ), this );
  
  QGroupBox* aSelGrp = new QGroupBox( 1, Qt::Vertical, aMainGrp );
  aSelGrp->setInsideMargin( 0 );
  aSelGrp->setFrameStyle( QFrame::NoFrame );
  new QLabel( tr( "GEOM_OBJECT" ), aSelGrp );
  ( new QPushButton( aSelGrp ) )->setPixmap( iconSelect );
  myEdit = new QLineEdit( aSelGrp );
  myEdit->setReadOnly( true );
  myEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );

  myClosedLbl = new QLabel( tr( "NUMBER_CLOSED" ), aMainGrp );
  myOpenLbl = new QLabel( tr( "NUMBER_OPEN" ), aMainGrp );

  QFrame* aFrame = new QFrame( this );
  aFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
  QPushButton* aCloseBtn = new QPushButton( tr( "GEOM_BUT_CLOSE" ), aFrame );
  QHBoxLayout* aBtnLay = new QHBoxLayout( aFrame, MARGIN, SPACING );
  aBtnLay->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aBtnLay->addWidget( aCloseBtn );
  aBtnLay->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum ) );

  QVBoxLayout* aLay = new QVBoxLayout( this );
  aLay->setSpacing( SPACING );
  aLay->setMargin( MARGIN );
  aLay->addWidget( aMainGrp );
  aLay->addWidget( aFrame );
  
  connect( aCloseBtn, SIGNAL( clicked() ), SLOT( onClose() ) );

  Init( theSelection );
}

//=================================================================================
// function : ~RepairGUI_FreeBoundDlg
// purpose  : Destructor
//=================================================================================
RepairGUI_FreeBoundDlg::~RepairGUI_FreeBoundDlg()
{
}

//=================================================================================
// function : onClose
// purpose  : SLOT. Called when "close" button pressed. Close dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onClose()
{
  globalSelection();
  disconnect( mySelection, 0, this, 0 );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( 0 );
  reject();
  erasePreview();
}

//=================================================================================
// function : onDeactivate
// purpose  : Deactivate this dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onDeactivate()
{
  setEnabled(false);
  globalSelection();
  disconnect( mySelection, 0, this, 0 );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : onActivate
// purpose  : Activate this dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::onActivate()
{
  GeometryGUI::GetGeomGUI()->EmitSignalDeactivateDialog();
  setEnabled( true );
  GeometryGUI::GetGeomGUI()->SetActiveDialogBox( this );
  connect( mySelection, SIGNAL( currentSelectionChanged() ), SLOT  ( onSelectionDone() ) );
  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : onSelectionDone
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeBoundDlg::onSelectionDone()
{
  if( mySelection->IObjectCount() != 1 )
    return;

  Standard_Boolean isOk = Standard_False;
  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject( mySelection->firstIObject(), isOk );

  if ( !isOk || anObj->_is_nil() || !GEOMBase::IsShape( anObj ) )
    return;
  else
  {
    myObj = anObj;
    displayPreview( false, true, true, 3 );
  }

}

//=================================================================================
// function : Init
// purpose  : Initialize dialog fields
//=================================================================================
void RepairGUI_FreeBoundDlg::Init( SALOME_Selection* theSel )
{
  myNbClosed = myNbOpen = 0;
  myObj = GEOM::GEOM_Object::_nil();
  mySelection = theSel;

  GeometryGUI* aGeomGUI = GeometryGUI::GetGeomGUI();
  connect( aGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), SLOT  ( onDeactivate() ) );
  connect( mySelection, SIGNAL( currentSelectionChanged() ), SLOT  ( onSelectionDone() ) );

  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : enterEvent
// purpose  : Activate dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::enterEvent( QEvent* e )
{
  onActivate();
}

//=================================================================================
// function : closeEvent
// purpose  : Close dialog
//=================================================================================
void RepairGUI_FreeBoundDlg::closeEvent( QCloseEvent* e )
{
  onClose();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void RepairGUI_FreeBoundDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_FACE );
  aMap.Add( GEOM_SHELL );
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : createOperation
// purpose  : Create operation
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeBoundDlg::createOperation()
{
  return getGeomEngine()->GetIHealingOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeBoundDlg::isValid( QString& msg )
{
  return !myObj->_is_nil();
}

//=================================================================================
// function : execute
// purpose  : Get free boundaries
//=================================================================================
bool RepairGUI_FreeBoundDlg::execute( ObjectList& objects )
{
  if ( !IsPreview() || myObj->_is_nil() )
    return false;

  GEOM::ListOfGO_var aClosed, anOpen;

  bool result = GEOM::GEOM_IHealingOperations::_narrow(
    getOperation() )->GetFreeBoundary( myObj, aClosed, anOpen );

  if ( result )
  {
    myNbClosed = aClosed->length();
    myNbOpen = anOpen->length();
    int i;
    for ( i = 0; i < myNbClosed; i++ )
      objects.push_back( aClosed[i]._retn() );
    for ( i = 0; i < myNbOpen; i++ )
      objects.push_back( anOpen[i]._retn() );

    myEdit->setText( GEOMBase::GetName( myObj ) );
    QString aLabelText = tr( "NUMBER_CLOSED" ) + QString( "%1" ).arg( myNbClosed );
    myClosedLbl->setText( aLabelText );
    aLabelText = tr( "NUMBER_OPEN" ) + QString( "%1" ).arg( myNbOpen );
    myOpenLbl->setText( aLabelText );
  }
  else
  {
    myEdit->setText( GEOMBase::GetName( myObj ) );
    myClosedLbl->setText( tr( "NUMBER_CLOSED" ) );
    myOpenLbl->setText( tr( "NUMBER_OPEN" ) );
  }

  return result;
}













