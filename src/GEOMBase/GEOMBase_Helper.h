//  GEOM GEOMGUI : GUI for Geometry component
//
//  Copyright (C) 2004  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : GEOMBase_Helper.h
//  Author : Sergey ANIKIN
//  Module : GEOM
//  $Header$

#ifndef GEOMBASE_HELPER_H
#define GEOMBASE_HELPER_H

#include "GEOM_Displayer.h"
#include "SALOME_Prs.h"
#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(GEOM_Gen)

#include <qstring.h>

#include <list>

typedef std::list<GEOM::GEOM_Object_ptr> ObjectList;

class GEOM_Operation;
class QAD_Study;
class TopoDS_Shape;
class QAD_ViewFrame;
class TColStd_MapOfInteger;

//================================================================
// Class       : GEOMBase_Helper
// Description : Helper class for dialog box development, can be used as 
//               the second base class for dialog boxes. Contains convenient methods
//               performing common operations (display/erase, selection activation,
//               publication in a study, transaction management)
//================================================================
class GEOMBase_Helper
{
public:
  GEOMBase_Helper();
  virtual ~GEOMBase_Helper();

protected:
  static GEOM::GEOM_Gen_ptr getGeomEngine();

  void display         ( const ObjectList&, const bool = true );
  void display         ( GEOM::GEOM_Object_ptr, const bool = true );
  void erase           ( const ObjectList&, const bool = true );
  void erase           ( GEOM::GEOM_Object_ptr, const bool = true );
  void redisplay       ( const ObjectList&, const bool = true, const bool = true );
  void redisplay       ( GEOM::GEOM_Object_ptr, const bool = true, const bool = true );

  virtual void displayPreview ( const bool   activate = false, 
                                const bool   update = true,
                                const bool   toRemoveFromEngine = true,
                                const double lineWidth = -1 );
  // This is the easiest way to show preview. It is based on execute() method.
  // It removes temporary GEOM::GEOM_Objects automatically.

  virtual void displayPreview  ( GEOM::GEOM_Object_ptr obj, 
                                 const bool   append = false, 
                                 const bool   activate = false, 
                                 const bool   update = true,
                                 const double lineWidth = -1 );
  void displayPreview  ( const SALOME_Prs* prs, 
			 const bool append = false, 
			 const bool = true );
  void erasePreview    ( const bool = true );

  void localSelection( const ObjectList&, const int );
  void localSelection( GEOM::GEOM_Object_ptr, const int );
  void activate( const int );
  void globalSelection( const int = GEOM_ALLOBJECTS, const bool = false  );
  void globalSelection( const TColStd_MapOfInteger&, const bool = false );
  void updateViewer    ();

  void prepareSelection( const ObjectList&, const int );
  void prepareSelection( GEOM::GEOM_Object_ptr, const int );

  void addInStudy      ( GEOM::GEOM_Object_ptr, const char* theName ); 

  bool openCommand     ();
  bool abortCommand    ();
  bool commitCommand   ( const char* = 0 );
  bool hasCommand      () const;

  void updateObjBrowser() const;
  int  getStudyId      () const;
  QAD_Study* getStudy  () const;
  bool checkViewFrame  ();

  bool onAccept( const bool publish = true, const bool useTransaction = true );
  // This method should be called from "OK" button handler.
  // <publish> == true means that objects returned by execute() 
  // should be published in a study.

  void showError();
  // Shows a message box with infromation about an error taken from getOperation()->GetErrorCode()
  void showError( const QString& msg );
  // Shows a error message followed by <msg>

  GEOM::GEOM_IOperations_ptr getOperation();
  // If <myOperation> is nil --> calls createOperation() and put the result
  // into <myOperation> and returns it;
  // otherwise, simply returns <myOperation>

  inline void setPrefix( const QString& prefix ) { myPrefix = prefix; }
  QString getPrefix( GEOM::GEOM_Object_ptr = GEOM::GEOM_Object::_nil() ) const;

  ////////////////////////////////////////////////////////////////////////////
  // Virtual methods, to be redefined in dialog classes
  ////////////////////////////////////////////////////////////////////////////

  virtual GEOM::GEOM_IOperations_ptr createOperation();
  // This method should be redefined in dialog boxes so as to return 
  // proper GEOM_IOperation interface.
  // Returns nil reference by default

  virtual bool isValid( QString& msg );
  // Called by onAccept(). Redefine this method to check validity of user input in dialog boxes.

  virtual bool execute( ObjectList& objects );
  // This method is called by onAccept(). 
  // It should perform the required operation and put all new or modified objects into 
  // <objects> argument.Should return <false> if some error occurs during its execution. 

  virtual GEOM::GEOM_Object_ptr getFather( GEOM::GEOM_Object_ptr theObj );
  // This method is called by addInStudy(). It should return a father object
  // for <theObj> or a nil reference if <theObj> should be published
  // as a top-level object.

  virtual const char* getNewObjectName() const; 

  void SetIsPreview(const bool thePreview) {isPreview = thePreview;}
  bool IsPreview() {return isPreview;}

  GEOM_Displayer*             getDisplayer();

private:
  char* getEntry              ( GEOM::GEOM_Object_ptr ) const;
  void                        clearShapeBuffer( GEOM::GEOM_Object_ptr );

private:
  typedef std::list<SALOME_Prs*> PrsList;

  PrsList                     myPreview;
  GEOM_Displayer*             myDisplayer;
  GEOM_Operation*             myCommand;
  GEOM::GEOM_IOperations_var  myOperation;
  QAD_ViewFrame*              myViewFrame;
  QString                     myPrefix;
  bool                        isPreview;

};

#endif