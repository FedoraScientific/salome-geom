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
//  File   : TransformationGUI_RotationDlg.h
//  Author : Lucien PIGNOLONI
//  Module : GEOM
//  $Header$

#ifndef DIALOGBOX_ROTATION_H
#define DIALOGBOX_ROTATION_H

#include "GEOMBase_Skeleton.h"
#include "DlgRef_2Sel1Spin2Check.h"
  
//=================================================================================
// class    : TransformationGUI_RotationDlg
// purpose  :
//=================================================================================
class TransformationGUI_RotationDlg : public GEOMBase_Skeleton
{ 
    Q_OBJECT

public:
    TransformationGUI_RotationDlg(QWidget* parent = 0, const char* name = 0, SALOME_Selection* Sel = 0, bool modal = FALSE, WFlags fl = 0);
    ~TransformationGUI_RotationDlg();

protected:
    // redefined from GEOMBase_Helper
    virtual GEOM::GEOM_IOperations_ptr createOperation();
    virtual bool isValid( QString& );
    virtual bool execute( ObjectList& objects );

    virtual void closeEvent( QCloseEvent* e );

private :
    void Init();
    void enterEvent(QEvent* e);
    double GetAngle() const;

    GEOM::ListOfGO myObjects;
    GEOM::GEOM_Object_var myAxis;
    
    DlgRef_2Sel1Spin2Check* GroupPoints;

private slots:
    void ClickOnOk();
    bool ClickOnApply();
    void ClickOnCancel();
    void ActivateThisDialog();
    void DeactivateActiveDialog();
    void LineEditReturnPressed();
    void SelectionIntoArgument();
    void SetEditCurrentArgument();
    void ValueChangedInSpinBox();
    void CreateCopyModeChanged(bool isCreateCopy);
    void onReverse();
};

#endif // DIALOGBOX_ROTATION_H