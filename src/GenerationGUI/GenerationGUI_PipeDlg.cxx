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
// File   : GenerationGUI_PipeDlg.cxx
// Author : Lucien PIGNOLONI, Open CASCADE S.A.S.
//
#include "GenerationGUI_PipeDlg.h"

#include <DlgRef.h>
#include <GeometryGUI.h>
#include <GEOMBase.h>

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Application.h>
#include <LightApp_SelectionMgr.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_MapOfInteger.hxx>

#include <GEOMImpl_Types.hxx>

//=================================================================================
// class    : GenerationGUI_PipeDlg()
// purpose  : Constructs a GenerationGUI_PipeDlg which is a child of 'parent', with the
//            name 'name' and widget flags set to 'f'.
//            The dialog will by default be modeless, unless you set 'modal' to
//            TRUE to construct a modal dialog.
//=================================================================================
GenerationGUI_PipeDlg::GenerationGUI_PipeDlg (GeometryGUI* theGeometryGUI, QWidget* parent,
                                              bool modal, Qt::WindowFlags fl)
  : GEOMBase_Skeleton(theGeometryGUI, parent, modal, fl)
{
  QPixmap image0 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE")));
  QPixmap image1 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_SELECT")));
  QPixmap image2 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_BINORMAL")));
  QPixmap image3 (SUIT_Session::session()->resourceMgr()->loadPixmap("GEOM", tr("ICON_DLG_PIPE_SECTION")));

  setWindowTitle(tr("GEOM_PIPE_TITLE"));

  /***************************************************************/
  mainFrame()->GroupConstructors->setTitle(tr("GEOM_PIPE"));
  mainFrame()->RadioButton1->setIcon(image0);
  mainFrame()->RadioButton2->setIcon(image2);
  mainFrame()->RadioButton3->setIcon(image3);
  // mainFrame()->RadioButton3->setAttribute(Qt::WA_DeleteOnClose);
  // mainFrame()->RadioButton3->close();

  GroupPoints = new DlgRef_3Sel1Check(centralWidget());

  GroupPoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupPoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupPoints->TextLabel2->setText(tr("GEOM_PATH_OBJECT"));
  GroupPoints->TextLabel3->setText(tr("GEOM_BINORMAL"));
  GroupPoints->PushButton1->setIcon(image1);
  GroupPoints->PushButton2->setIcon(image1);
  GroupPoints->PushButton3->setIcon(image1);
  GroupPoints->CheckButton1->setText(tr("GEOM_SELECT_UNPUBLISHED_EDGES"));

  GroupMakePoints = new DlgRef_3Sel2Check3Spin(centralWidget());

  GroupMakePoints->GroupBox1->setTitle(tr("GEOM_ARGUMENTS"));
  GroupMakePoints->TextLabel1->setText(tr("GEOM_BASE_OBJECT"));
  GroupMakePoints->TextLabel2->setText(tr("GEOM_LOCATIONS"));
  GroupMakePoints->TextLabel3->setText(tr("GEOM_PATH_OBJECT"));
  GroupMakePoints->PushButton1->setIcon(image1);
  GroupMakePoints->PushButton2->setIcon(image1);
  GroupMakePoints->PushButton3->setIcon(image1);
  GroupMakePoints->CheckBox1->setText(tr("GEOM_WITH_CONTACT"));
  GroupMakePoints->CheckBox2->setText(tr("GEOM_WITH_CORRECTION"));
  GroupMakePoints->SpinBox1->close();
  GroupMakePoints->SpinBox2->close();
  GroupMakePoints->SpinBox3->close();
  GroupMakePoints->TextLabel4->close();
  GroupMakePoints->TextLabel5->close();
  GroupMakePoints->TextLabel6->close();

  QVBoxLayout* layout = new QVBoxLayout(centralWidget());
  layout->setMargin(0); layout->setSpacing(6);
  layout->addWidget(GroupPoints);
  layout->addWidget(GroupMakePoints);
  /***************************************************************/

  setHelpFileName("create_extrusion_alongpath_page.html");

  // Initialisation
  Init();
}

//=================================================================================
// function : ~GenerationGUI_PipeDlg()
// purpose  : Destroys the object and frees any allocated resources
//=================================================================================
GenerationGUI_PipeDlg::~GenerationGUI_PipeDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

//=================================================================================
// function : Init()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::Init()
{
  // init variables
  GroupPoints->LineEdit1->setReadOnly(true);
  GroupPoints->LineEdit2->setReadOnly(true);
  GroupPoints->LineEdit3->setReadOnly(true);

  GroupPoints->LineEdit1->setText("");
  GroupPoints->LineEdit2->setText("");
  GroupPoints->LineEdit3->setText("");
  myPath.nullify();
  myVec.nullify();

  GroupPoints->CheckButton1->setEnabled(false);

  GroupMakePoints->LineEdit1->setReadOnly(true);
  GroupMakePoints->LineEdit2->setReadOnly(true);
  GroupMakePoints->LineEdit3->setReadOnly(true);

  GroupMakePoints->LineEdit1->setText("");
  GroupMakePoints->LineEdit2->setText("");
  GroupMakePoints->LineEdit3->setText("");

  showOnlyPreviewControl();

  // signals and slots connections
  connect(buttonOk(),    SIGNAL(clicked()), this, SLOT(ClickOnOk()));
  connect(buttonApply(), SIGNAL(clicked()), this, SLOT(ClickOnApply()));

  connect(this,          SIGNAL(constructorsClicked(int)), this, SLOT(ConstructorsClicked(int)));

  connect(GroupPoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupPoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));

  connect(GroupPoints->CheckButton1,   SIGNAL(toggled(bool)), this, SLOT(SelectionTypeButtonClicked()));

  connect(GroupMakePoints->PushButton1, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->PushButton2, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->PushButton3, SIGNAL(clicked()), this, SLOT(SetEditCurrentArgument()));
  connect(GroupMakePoints->CheckBox1, SIGNAL(clicked()), this, SLOT(processPreview()));
  connect(GroupMakePoints->CheckBox2, SIGNAL(clicked()), this, SLOT(processPreview()));

  initName(tr("GEOM_PIPE"));
  resize(100,100);

  GroupPoints->TextLabel3->hide();
  GroupPoints->PushButton3->hide();
  GroupPoints->LineEdit3->hide();
  ConstructorsClicked(0);

  GroupPoints->PushButton1->click();
  SelectionIntoArgument();
}

//=================================================================================
// function : ConstructorsClicked()
// purpose  : Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::ConstructorsClicked( int constructorId )
{
  erasePreview();

  switch (constructorId) {
  case 0:
    GroupMakePoints->hide();
    GroupPoints->show();
    GroupPoints->TextLabel3->hide();
    GroupPoints->PushButton3->hide();
    GroupPoints->LineEdit3->hide();
    GroupPoints->PushButton1->click();
    break;
  case 1:
    GroupMakePoints->hide();
    GroupPoints->show();
    GroupPoints->TextLabel3->show();
    GroupPoints->PushButton3->show();
    GroupPoints->LineEdit3->show();
    GroupPoints->PushButton1->click();
    break;
  case 2:
    GroupPoints->hide();
    GroupMakePoints->show();
    GroupMakePoints->PushButton1->click();
    break;
  default:
    break;
  }

  qApp->processEvents();
  updateGeometry();
  resize(100,100);

  SelectionIntoArgument();
  processPreview();
}

//=================================================================================
// function : SelectionBittonClicked()
// purpose  : Selection type Radio button management
//=================================================================================
void GenerationGUI_PipeDlg::SelectionTypeButtonClicked()
{
  globalSelection();
  if ( GroupPoints->CheckButton1->isChecked() ) {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
  } else {
    TColStd_MapOfInteger aMap;
    aMap.Add(GEOM_COMPOUND);
    aMap.Add(GEOM_WIRE);
    aMap.Add(GEOM_LINE);
    globalSelection(aMap);
  }
  if ( myEditCurrentArgument == GroupPoints->LineEdit2 ) {
    myEditCurrentArgument->setText("");
    myPath.nullify();
  }
  processPreview();
}

//=================================================================================
// function : ClickOnOk()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ClickOnOk()
{
  setIsApplyAndClose( true );
  if (ClickOnApply())
    ClickOnCancel();
}

//=================================================================================
// function : ClickOnApply()
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::ClickOnApply()
{
  if (!onAccept())
    return false;

  initName();

  return true;
}

//=================================================================================
// function : SelectionIntoArgument()
// purpose  : Called when selection is changed or on dialog initialization or activation
//=================================================================================
void GenerationGUI_PipeDlg::SelectionIntoArgument()
{
  erasePreview();
  myEditCurrentArgument->setText("");

  if ( myEditCurrentArgument == GroupPoints->LineEdit1 ) {
    myBaseObjects.clear();
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_SHAPE, -1 );
    for ( int i = 0; i < objects.count(); i++ ) {
      GEOM::shape_type stype = objects[i]->GetMaxShapeType();
      GEOM::shape_type maxallowed = getConstructorId() == 1 ? GEOM::FACE : GEOM::SHELL;
      GEOM::shape_type minallowed = getConstructorId() == 1 ? GEOM::EDGE : GEOM::VERTEX;
      if ( stype < maxallowed || stype > minallowed )
        continue;
      myBaseObjects << objects[i];
    }
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit2) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE;
    myPath = getSelected( types );
    if ( myPath ) {
      QString aName = GEOMBase::GetName( myPath.get() );
      myEditCurrentArgument->setText( aName );
      if ( getConstructorId() == 1 && !myVec )
        GroupPoints->PushButton3->click();
      else if ( myBaseObjects.isEmpty() )
        GroupPoints->PushButton1->click();
    }
  }
  else if (myEditCurrentArgument == GroupPoints->LineEdit3) {
    myVec = getSelected( TopAbs_EDGE );
    if ( myVec ) {
      QString aName = GEOMBase::GetName( myVec.get() );
      myEditCurrentArgument->setText( aName );
      if ( myBaseObjects.isEmpty() )
        GroupPoints->PushButton1->click();
      else if ( !myPath )
        GroupPoints->PushButton2->click();
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit1 ) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE << TopAbs_FACE << TopAbs_SHELL;
    QList<GEOM::GeomObjPtr> objects = getSelected( types, -1 );
    GEOMBase::Synchronize( myBaseObjects, objects );
    if ( !myBaseObjects.isEmpty() ) {
      QString aName = myBaseObjects.count() > 1 ? QString( "%1_objects").arg( myBaseObjects.count() ) : GEOMBase::GetName( myBaseObjects[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit2 ) {
    localSelection( GEOM::GEOM_Object::_nil(), TopAbs_VERTEX );
    QList<GEOM::GeomObjPtr> objects = getSelected( TopAbs_VERTEX, -1 );
    GEOMBase::Synchronize( myLocations, objects );
    if ( !myLocations.isEmpty() ) {
      QString aName = myLocations.count() > 1 ? QString( "%1_objects").arg( myLocations.count() ) : GEOMBase::GetName( myLocations[0].get() );
      myEditCurrentArgument->setText( aName );
    }
  }
  else if ( myEditCurrentArgument == GroupMakePoints->LineEdit3 ) {
    QList<TopAbs_ShapeEnum> types;
    types << TopAbs_EDGE << TopAbs_WIRE;
    myPath = getSelected( types );
    if ( myPath ) {
      QString aName = GEOMBase::GetName( myPath.get() );
      myEditCurrentArgument->setText( aName );
    }
  }

  processPreview();
}

//=================================================================================
// function : SetEditCurrentArgument()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::SetEditCurrentArgument()
{
  QPushButton* send = (QPushButton*)sender();

  disconnect(myGeomGUI->getApp()->selectionMgr(), 0, this, 0);
  globalSelection(GEOM_ALLSHAPES);
  GroupPoints->PushButton1->setDown(false);
  GroupPoints->PushButton2->setDown(false);
  GroupPoints->PushButton3->setDown(false);
  GroupPoints->LineEdit1->setEnabled(false);
  GroupPoints->LineEdit2->setEnabled(false);
  GroupPoints->LineEdit3->setEnabled(false);
  if (send == GroupPoints->PushButton1) {
    myEditCurrentArgument = GroupPoints->LineEdit1;
    GroupPoints->CheckButton1->setEnabled(false);
  }
  else if (send == GroupPoints->PushButton2) {
    myEditCurrentArgument = GroupPoints->LineEdit2;

    if ( GroupPoints->CheckButton1->isChecked() ) {
      localSelection( GEOM::GEOM_Object::_nil(), TopAbs_EDGE );
    } else {
      TColStd_MapOfInteger aMap;
      aMap.Add(GEOM_COMPOUND);
      aMap.Add(GEOM_WIRE);
      aMap.Add(GEOM_LINE);
      globalSelection(aMap);
    }
    GroupPoints->CheckButton1->setEnabled(true);
  }
  else if(send == GroupPoints->PushButton3) {
    myEditCurrentArgument = GroupPoints->LineEdit3;
    GroupPoints->CheckButton1->setEnabled(false);
    localSelection(GEOM::GEOM_Object::_nil(), TopAbs_EDGE);
  }

  GroupMakePoints->PushButton1->setDown(false);
  GroupMakePoints->PushButton2->setDown(false);
  GroupMakePoints->PushButton3->setDown(false);
  GroupMakePoints->LineEdit1->setEnabled(false);
  GroupMakePoints->LineEdit2->setEnabled(false);
  GroupMakePoints->LineEdit3->setEnabled(false);
  if (send == GroupMakePoints->PushButton1) {
    myEditCurrentArgument = GroupMakePoints->LineEdit1;
  }
  else if(send == GroupMakePoints->PushButton2) {
    myEditCurrentArgument = GroupMakePoints->LineEdit2;
  }
  else if (send == GroupMakePoints->PushButton3) {
    myEditCurrentArgument = GroupMakePoints->LineEdit3;
  }

  connect(myGeomGUI->getApp()->selectionMgr(), SIGNAL(currentSelectionChanged()),
          this, SLOT(SelectionIntoArgument()));

  // enable line edit
  myEditCurrentArgument->setEnabled(true);
  myEditCurrentArgument->setFocus();
  // after setFocus(), because it will be setDown(false) when loses focus
  send->setDown(true);

  // seems we need it only to avoid preview disappearing, caused by selection mode change
  processPreview();
}

//=================================================================================
// function : ActivateThisDialog()
// purpose  :
//=================================================================================
void GenerationGUI_PipeDlg::ActivateThisDialog()
{
  GEOMBase_Skeleton::ActivateThisDialog();
  connect( myGeomGUI->getApp()->selectionMgr(), SIGNAL( currentSelectionChanged() ),
           this, SLOT( SelectionIntoArgument() ) );

  ConstructorsClicked(getConstructorId());
}

//=================================================================================
// function : enterEvent()
// purpose  : when mouse enter onto the QWidget
//=================================================================================
void GenerationGUI_PipeDlg::enterEvent (QEvent*)
{
  if (!mainFrame()->GroupConstructors->isEnabled())
    ActivateThisDialog();
}

//=================================================================================
// function : createOperation
// purpose  :
//=================================================================================
GEOM::GEOM_IOperations_ptr GenerationGUI_PipeDlg::createOperation()
{
  return getGeomEngine()->GetI3DPrimOperations(getStudyId());
}

//=================================================================================
// function : isValid
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::isValid (QString&)
{
  bool ok = false;
  switch ( getConstructorId() ) {
  case 0 :
    ok = !myBaseObjects.isEmpty() && myPath;
    break;
  case 1 :
    ok = !myBaseObjects.isEmpty() && myPath && myVec;
    break;
  case 2 :
    ok = !myBaseObjects.isEmpty() && ( myLocations.isEmpty() || myBaseObjects.count() == myLocations.count() ) && myPath;
    break;
  default:
    break;
  }
  return ok;
}

//=================================================================================
// function : execute
// purpose  :
//=================================================================================
bool GenerationGUI_PipeDlg::execute (ObjectList& objects)
{
  GEOM::GEOM_Object_var anObj;

  GEOM::GEOM_I3DPrimOperations_var anOper = GEOM::GEOM_I3DPrimOperations::_narrow(getOperation());
  switch( getConstructorId() ) {
  case 0:
  case 1:
    for (int i = 0; i < myBaseObjects.count(); i++) {
      switch ( getConstructorId() ) {
      case 0 :
	anObj = anOper->MakePipe(myBaseObjects[i].get(), myPath.get());
	break;
      case 1 :
	anObj = anOper->MakePipeBiNormalAlongVector(myBaseObjects[i].get(), myPath.get(), myVec.get());
	break;
      }
    
      if (!anObj->_is_nil())
	objects.push_back(anObj._retn());
    }
    break;
  case 2:
    {
      GEOM::ListOfGO_var myBaseGO = new GEOM::ListOfGO();
      GEOM::ListOfGO_var myLocationsGO = new GEOM::ListOfGO();
      myBaseGO->length( myBaseObjects.count() );
      myLocationsGO->length( myLocations.count() );
      for (int i = 0; i < myBaseObjects.count(); i++) {
	myBaseGO[i] = myBaseObjects[i].copy();
      }
      for (int i = 0; i < myLocations.count(); i++) {
	myLocationsGO[i] = myLocations[i].copy();
      }
      
      anObj = anOper->MakePipeWithDifferentSections(myBaseGO.in(), myLocationsGO.in(), myPath.get(), 
						    GroupMakePoints->CheckBox1->isChecked(), 
						    GroupMakePoints->CheckBox2->isChecked());
      if (!anObj->_is_nil())
	objects.push_back(anObj._retn());
    }
    break;
  default:
    break;
  }
  return true;
}

//=================================================================================
// function : addSubshapeToStudy
// purpose  : virtual method to add new SubObjects if local selection
//=================================================================================
void GenerationGUI_PipeDlg::addSubshapesToStudy()
{
  GEOMBase::PublishSubObject( myPath.get() );
  GEOMBase::PublishSubObject( myVec.get() );
}

//=================================================================================
// function : extractPrefix
// purpose  : redefined from GEOMBase_Helper class
//=================================================================================
bool GenerationGUI_PipeDlg::extractPrefix() const
{
  return myBaseObjects.count() > 1;
}
