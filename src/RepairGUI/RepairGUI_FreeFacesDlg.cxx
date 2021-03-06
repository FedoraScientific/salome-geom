// Copyright (C) 2007-2014  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

// GEOM GEOMGUI : GUI for Geometry component
// File   : RepairGUI_FreeFacesDlg.cxx
// Author : Vladimir KLYACHIN, Open CASCADE S.A.S. (vladimir.klyachin@opencascade.com)
//
#include "RepairGUI_FreeFacesDlg.h"

#include <LightApp_Application.h>
#include <LightApp_SelectionMgr.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Tools.h>
#include <SalomeApp_Study.h>

#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_OverrideCursor.h>
#include <SUIT_Desktop.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>

#include <GEOMImpl_Types.hxx>
#include <GEOM_Constants.h>

#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_MapOfInteger.hxx>
#include <TopExp.hxx>

#include <GEOMBase.h>
#include <GeometryGUI.h>
#include <GEOM_Displayer.h>

#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>

#define SPACING 6
#define MARGIN  9
#define MIN_WIDTH 200

//=================================================================================
// class    : RepairGUI_FreeFacesDlg()
// purpose  : Constructs a RepairGUI_FreeFacesDlg  which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
RepairGUI_FreeFacesDlg::RepairGUI_FreeFacesDlg( GeometryGUI* GUI, QWidget* parent,
                                                bool modal )
  : QDialog( parent, 0 ),
    GEOMBase_Helper( dynamic_cast<SUIT_Desktop*>( parent ) ),
    myGeomGUI( GUI ), 
    myDisplayer( 0 )
{
  setAttribute( Qt::WA_DeleteOnClose );

  setSizeGripEnabled( true );

  QPixmap image1( SUIT_Session::session()->resourceMgr()->loadPixmap( "GEOM", tr( "ICON_SELECT" ) ) );

  setWindowTitle( tr( "GEOM_FREE_FACES_TITLE" ) );

  /***************************************************************/

  QGroupBox* aMainGrp = new QGroupBox( tr( "GEOM_SELECTED_SHAPE" ), this );
  
  QLabel* lab = new QLabel( tr( "GEOM_OBJECT" ), aMainGrp );
  mySelBtn = new QPushButton( aMainGrp );
  mySelBtn->setIcon( image1 );
  myEdit = new QLineEdit( aMainGrp );
  myEdit->setReadOnly( true );
  myEdit->setSizePolicy( QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed ) );
  myEdit->setMinimumWidth( MIN_WIDTH );

  QHBoxLayout* aMainLay = new QHBoxLayout( aMainGrp );
  aMainLay->setSpacing( SPACING );
  aMainLay->setMargin( MARGIN );
  aMainLay->addWidget( lab );
  aMainLay->addWidget( mySelBtn );
  aMainLay->addWidget( myEdit );

  QFrame* aFrame = new QFrame( this );
  aFrame->setFrameStyle( QFrame::Box | QFrame::Sunken );
  QPushButton* aCloseBtn = new QPushButton( tr( "GEOM_BUT_CLOSE" ), aFrame );
  QPushButton* aHelpBtn = new QPushButton( tr( "GEOM_BUT_HELP" ), aFrame );

  QHBoxLayout* aBtnLay = new QHBoxLayout( aFrame );
  aBtnLay->setSpacing( SPACING );
  aBtnLay->setMargin( MARGIN );
  aBtnLay->addWidget( aCloseBtn );
  aBtnLay->addSpacing( SPACING );
  aBtnLay->addStretch();
  aBtnLay->addWidget( aHelpBtn );

  QVBoxLayout* aLay = new QVBoxLayout( this );
  aLay->setSpacing( SPACING );
  aLay->setMargin( MARGIN );
  aLay->addWidget( aMainGrp );
  aLay->addStretch();
  aLay->addWidget( aFrame );

  myHelpFileName = "free_faces_page.html";

  connect( aCloseBtn, SIGNAL( clicked() ), SLOT( onClose() ) );
  connect( aHelpBtn,  SIGNAL( clicked() ), SLOT( onHelp() ) );
  connect( mySelBtn,  SIGNAL( clicked() ),
           this,      SLOT  ( onSetEditCurrentArgument() ) );
  /***************************************************************/

  Init();
}


//=================================================================================
// function : ~RepairGUI_FreeFacesDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
RepairGUI_FreeFacesDlg::~RepairGUI_FreeFacesDlg()
{
}


//=================================================================================
// function : onClose
// purpose  : SLOT. Called when "close" button pressed. Close dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onClose()
{
  globalSelection();
  disconnect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 0, this, 0 );
  myGeomGUI->SetActiveDialogBox( 0 );
  reject();
  erasePreview();
}

//=================================================================================
// function : onHelp()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::onHelp()
{
  LightApp_Application* app = (LightApp_Application*)( SUIT_Session::session()->activeApplication() );
  if ( app )
    app->onHelpContextModule( myGeomGUI ? app->moduleName( myGeomGUI->moduleName() ) : QString(""), myHelpFileName );
  else {
    QString platform;
#ifdef WIN32
    platform = "winapplication";
#else
    platform = "application";
#endif
    SUIT_MessageBox::warning( this, 
                              tr( "WRN_WARNING" ),
                              tr( "EXTERNAL_BROWSER_CANNOT_SHOW_PAGE" ).
                              arg( app->resourceMgr()->stringValue( "ExternalBrowser", 
                                                                    platform ) ).
                              arg( myHelpFileName ) );
  }
}

//=================================================================================
// function : onDeactivate
// purpose  : Deactivate this dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onDeactivate()
{
  setEnabled( false );
  globalSelection();
  disconnect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 0, this, 0 );
  myGeomGUI->SetActiveDialogBox( 0 );
}

//=================================================================================
// function : onActivate
// purpose  : Activate this dialog
//=================================================================================
void RepairGUI_FreeFacesDlg::onActivate()
{
  myGeomGUI->EmitSignalDeactivateDialog();
  setEnabled( true );
  myGeomGUI->SetActiveDialogBox( this );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), SLOT( onSelectionDone() ) );
  activateSelection();
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::Init()
{
  myObj = GEOM::GEOM_Object::_nil();

  /* signals and slots connections */
  connect( myGeomGUI, SIGNAL( SignalDeactivateActiveDialog() ), SLOT  ( onDeactivate() ) );
  connect( ( (SalomeApp_Application*)( SUIT_Session::session()->activeApplication() ) )->selectionMgr(), 
           SIGNAL( currentSelectionChanged() ), SLOT  ( onSelectionDone() ) );

  activateSelection();
  onSelectionDone();
}

//=================================================================================
// function : onSelectionDone
// purpose  : SLOT. Called when selection changed.
//=================================================================================
void RepairGUI_FreeFacesDlg::onSelectionDone()
{
  erasePreview();

  LightApp_SelectionMgr* aSelMgr = myGeomGUI->getApp()->selectionMgr();
  SALOME_ListIO aSelList;
  aSelMgr->selectedObjects(aSelList);

  if ( aSelList.Extent() != 1 ) {
    myEdit->setText( "" );
    return;
  }

  GEOM::GEOM_Object_var anObj =
    GEOMBase::ConvertIOinGEOMObject( aSelList.First() );

  if ( !GEOMBase::IsShape( anObj ) ) {
    myEdit->setText( "" );
    return;
  }
  else {
    myObj = anObj;
    displayPreview( true, false, true, true, 3 );
  }
}

//=================================================================================
// function : enterEvent()
// purpose  : Mouse enter onto the dialog to activate it
//=================================================================================
void RepairGUI_FreeFacesDlg::enterEvent( QEvent* )
{
  onActivate();
}

//=================================================================================
// function : activateSelection
// purpose  : activate selection of faces, shells, and solids
//=================================================================================
void RepairGUI_FreeFacesDlg::activateSelection()
{
  TColStd_MapOfInteger aMap;
  aMap.Add( GEOM_SOLID );
  aMap.Add( GEOM_COMPOUND );
  globalSelection( aMap );
}

//=================================================================================
// function : closeEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::closeEvent( QCloseEvent* )
{
  onClose();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr RepairGUI_FreeFacesDlg::createOperation()
{
  return getGeomEngine()->GetIShapesOperations( getStudyId() );
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::isValid( QString& )
{
  return !myObj->_is_nil() ;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool RepairGUI_FreeFacesDlg::execute( ObjectList& objects )
{
  bool aResult = false;
  GEOM::GEOM_IShapesOperations_var anOper = GEOM::GEOM_IShapesOperations::_narrow( getOperation() );
  GEOM::ListOfLong_var aFaceLst = anOper->GetFreeFacesIDs( myObj );
  TopoDS_Shape aSelShape;
  TopoDS_Shape aFace; 
  TopTools_IndexedMapOfShape anIndices;
  if ( !myObj->_is_nil() && GEOMBase::GetShape( myObj, aSelShape ) ) {
    myEdit->setText( GEOMBase::GetName( myObj ) );
    QString aMess;
    if ( !isValid( aMess ) ) {
      erasePreview( true );
      return false;
    }
    
    SUIT_OverrideCursor();

    TopExp::MapShapes( aSelShape, anIndices);
    SALOME_Prs* aPrs = 0;
    
    //Get object trancparency and set it to preview
    SalomeApp_Application* app = dynamic_cast< SalomeApp_Application* >( SUIT_Session::session()->activeApplication() );
    double transparency = 1.0;
    if( app ) {
      SUIT_ViewWindow* window = app->desktop( )->activeWindow( );
      if( window && window->getViewManager() ) {
	if ( app ) {
	  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
	  if( aStudy ) {
	    int aMgrId = window->getViewManager()->getGlobalId();
	    CORBA::String_var aMainEntry = myObj->GetStudyEntry();
	    QString anEntry = aMainEntry.in();
	    QVariant v = aStudy->getObjectProperty( aMgrId , anEntry , GEOM::propertyName( GEOM::Transparency ) , transparency );
	    if( v.canConvert( QVariant::Double ) ) {
	      transparency = v.toDouble();
	    }
	  }
	}
      }
    }
    
    for ( int i = 0, n = aFaceLst->length(); i < n; i++ ) {
      aFace = anIndices.FindKey( aFaceLst[i] );
      try {
        getDisplayer()->SetColor( Quantity_NOC_RED );
        getDisplayer()->SetToActivate( false );
	getDisplayer()->SetTransparency( transparency );
        aPrs = !aFace.IsNull() ? getDisplayer()->BuildPrs( aFace ) : 0;
        if ( aPrs )
          displayPreview( aPrs, true );
      }
      catch( const SALOME::SALOME_Exception& e )
      {
        SalomeApp_Tools::QtCatchCorbaException( e );
      }
    }
  }
  return aResult;
}

//================================================================
// Function : getDisplayer
// Purpose  :
//================================================================
GEOM_Displayer* RepairGUI_FreeFacesDlg::getDisplayer()
{
  if ( !myDisplayer )
    myDisplayer = new GEOM_Displayer( getStudy() );
  return myDisplayer;
}

//=================================================================================
// function : SetEditCurrentArgument
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::onSetEditCurrentArgument()
{
  myEdit->setFocus();
  onSelectionDone();
}

//=================================================================================
// function : keyPressEvent()
// purpose  :
//=================================================================================
void RepairGUI_FreeFacesDlg::keyPressEvent( QKeyEvent* e )
{
  QDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    onHelp();
  }
}
