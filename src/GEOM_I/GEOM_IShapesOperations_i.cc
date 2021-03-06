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

#include <Standard_Stream.hxx>

#include "GEOM_IShapesOperations_i.hh"

#include "utilities.h"
#include "OpUtil.hxx"
#include "Utils_ExceptHandlers.hxx"

#include "GEOM_Engine.hxx"
#include "GEOM_Object.hxx"

#include <TopAbs.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HArray1OfInteger.hxx>

//=============================================================================
/*!
 *   constructor:
 */
//=============================================================================
GEOM_IShapesOperations_i::GEOM_IShapesOperations_i (PortableServer::POA_ptr thePOA,
                                                    GEOM::GEOM_Gen_ptr theEngine,
                                                    ::GEOMImpl_IShapesOperations* theImpl)
:GEOM_IOperations_i(thePOA, theEngine, theImpl)
{
  MESSAGE("GEOM_IShapesOperations_i::GEOM_IShapesOperations_i");
}

//=============================================================================
/*!
 *  destructor
 */
//=============================================================================
GEOM_IShapesOperations_i::~GEOM_IShapesOperations_i()
{
  MESSAGE("GEOM_IShapesOperations_i::~GEOM_IShapesOperations_i");
}


//=============================================================================
/*!
 *  MakeEdge
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeEdge
                      (GEOM::GEOM_Object_ptr thePnt1, GEOM::GEOM_Object_ptr thePnt2)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference points
  Handle(GEOM_Object) aPnt1 = GetObjectImpl(thePnt1);
  Handle(GEOM_Object) aPnt2 = GetObjectImpl(thePnt2);

  if (aPnt1.IsNull() || aPnt2.IsNull()) return aGEOMObject._retn();

  //Create the Edge
  Handle(GEOM_Object) anObject = GetOperations()->MakeEdge(aPnt1, aPnt2);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeEdgeOnCurveByLength
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeEdgeOnCurveByLength
                  (GEOM::GEOM_Object_ptr theCurve,
                   CORBA::Double         theLength,
                   GEOM::GEOM_Object_ptr theStartPoint)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference curve
  Handle(GEOM_Object) aRefCurve = GetObjectImpl(theCurve);
  if (aRefCurve.IsNull()) return aGEOMObject._retn();

  //Get the reference point (can be NULL)
  Handle(GEOM_Object) aRefPoint;
  if (!CORBA::is_nil(theStartPoint)) {
    aRefPoint = GetObjectImpl(theStartPoint);
  }

  //Create the point
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeEdgeOnCurveByLength(aRefCurve, theLength, aRefPoint);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeEdgeWire
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeEdgeWire
                      (GEOM::GEOM_Object_ptr theWire,
                       const CORBA::Double theLinearTolerance,
                       const CORBA::Double theAngularTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the source wire
  Handle(GEOM_Object) aWire = GetObjectImpl(theWire);

  if (aWire.IsNull()) return aGEOMObject._retn();

  //Create the Edge
  Handle(GEOM_Object) anObject = GetOperations()->MakeEdgeWire(aWire, theLinearTolerance, theAngularTolerance);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeWire
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeWire
                           (const GEOM::ListOfGO& theEdgesAndWires,
                            const CORBA::Double   theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theEdgesAndWires.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theEdgesAndWires[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeWire(aShapes, theTolerance);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeFace
                      (GEOM::GEOM_Object_ptr theWire,
                       const CORBA::Boolean  isPlanarWanted)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference wire
  Handle(GEOM_Object) aWire = GetObjectImpl(theWire);
  if (aWire.IsNull()) return aGEOMObject._retn();

  //Create the Face
  Handle(GEOM_Object) anObject = GetOperations()->MakeFace(aWire, isPlanarWanted);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // enable warning status
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeFaceWires
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeFaceWires
                                         (const GEOM::ListOfGO& theWires,
                                          const CORBA::Boolean  isPlanarWanted)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theWires.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theWires[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Face
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeFaceWires(aShapes, isPlanarWanted);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // enable warning status
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeShell
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeShell
                                      (const GEOM::ListOfGO& theFacesAndShells)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theFacesAndShells.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theFacesAndShells[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeShell(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSolidShell
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeSolidShell
                                                (GEOM::GEOM_Object_ptr theShell)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShell = GetObjectImpl(theShell);
  if (aShell.IsNull()) return aGEOMObject._retn();

  std::list<Handle(GEOM_Object)> aShapes;
  aShapes.push_back(aShell);

  //Create the Solid
  Handle(GEOM_Object) anObject = GetOperations()->MakeSolidShells(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSolidShells
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeSolidShells
                                      (const GEOM::ListOfGO& theShells)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theShells.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theShells[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeSolidShells(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeCompound
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeCompound
                                      (const GEOM::ListOfGO& theShapes)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aShapes;

  //Get the shapes
  aLen = theShapes.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theShapes[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aShapes.push_back(aSh);
  }

  // Make Solid
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeCompound(aShapes);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeGlueFaces
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeGlueFaces
                                           (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double   theTolerance,
                                            CORBA::Boolean  doKeepNonSolids)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Perform the gluing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueFaces(aShape, theTolerance, doKeepNonSolids);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // to allow warning
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetGlueFaces
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetGlueFaces
                                           (GEOM::GEOM_Object_ptr theShape,
                                            const CORBA::Double   theTolerance)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    //GetOperations()->GetGlueFaces(aShape, theTolerance);
    GetOperations()->GetGlueShapes(aShape, theTolerance, TopAbs_FACE);

  //if (!GetOperations()->IsDone() || aHSeq.IsNull())
  // to allow warning
  if(aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeGlueFacesByList
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeGlueFacesByList
                                           (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double   theTolerance,
                                            const GEOM::ListOfGO& theFaces,
                                            CORBA::Boolean doKeepNonSolids,
                                            CORBA::Boolean doGlueAllEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> aFaces;
  //Get the shapes
  aLen = theFaces.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theFaces[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    aFaces.push_back(aSh);
  }

  //Perform the gluing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueFacesByList(aShape, theTolerance, aFaces, doKeepNonSolids, doGlueAllEdges);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // to allow warning
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeGlueEdges
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeGlueEdges
                                           (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double   theTolerance)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Perform the gluing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueEdges(aShape, theTolerance);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // to allow warning
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetGlueEdges
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetGlueEdges
                                           (GEOM::GEOM_Object_ptr theShape,
                                            const CORBA::Double   theTolerance)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetGlueShapes(aShape, theTolerance, TopAbs_EDGE);

  //if (!GetOperations()->IsDone() || aHSeq.IsNull())
  // to allow warning
  if (aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeGlueEdgesByList
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::MakeGlueEdgesByList
                                           (GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Double   theTolerance,
                                            const GEOM::ListOfGO& theEdges)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  int ind, aLen;
  std::list<Handle(GEOM_Object)> anEdges;
  //Get the shapes
  aLen = theEdges.length();
  for (ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theEdges[ind]);
    if (aSh.IsNull()) return aGEOMObject._retn();
    anEdges.push_back(aSh);
  }

  //Perform the gluing
  Handle(GEOM_Object) anObject =
    GetOperations()->MakeGlueEdgesByList(aShape, theTolerance, anEdges);
  //if (!GetOperations()->IsDone() || anObject.IsNull())
  // to allow warning
  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetExistingSubObjects
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetExistingSubObjects (GEOM::GEOM_Object_ptr theShape,
                                                                 CORBA::Boolean        theGroupsOnly)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetExistingSubObjects(aShape, theGroupsOnly);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeExplode (including theShape itself, bad sorting)
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::MakeExplode (GEOM::GEOM_Object_ptr theShape,
                                                       const CORBA::Long     theShapeType,
                                                       const CORBA::Boolean  isSorted)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakeExplode(aShape, theShapeType, isSorted,
                                 GEOMImpl_IShapesOperations::EXPLODE_OLD_INCLUDE_MAIN);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  MakeAllSubShapes (including theShape itself, good sorting)
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::MakeAllSubShapes (GEOM::GEOM_Object_ptr theShape,
                                                            const CORBA::Long     theShapeType,
                                                            const CORBA::Boolean  isSorted)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->MakeExplode(aShape, theShapeType, isSorted,
                                 GEOMImpl_IShapesOperations::EXPLODE_NEW_INCLUDE_MAIN);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  ExtractSubShapes (excluding theShape itself, good sorting)
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::ExtractSubShapes (GEOM::GEOM_Object_ptr theShape,
                                                            const CORBA::Long     theShapeType,
                                                            const CORBA::Boolean  isSorted)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    // TODO: enum instead of bool for the last argument
    GetOperations()->MakeExplode(aShape, theShapeType, isSorted,
                                 GEOMImpl_IShapesOperations::EXPLODE_NEW_EXCLUDE_MAIN);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  SubShapeAllIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::SubShapeAllIDs (GEOM::GEOM_Object_ptr theShape,
                                                            const CORBA::Long     theShapeType,
                                                            const CORBA::Boolean  isSorted)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->SubShapeAllIDs(aShape, theShapeType, isSorted,
                                    GEOMImpl_IShapesOperations::EXPLODE_OLD_INCLUDE_MAIN);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetAllSubShapesIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetAllSubShapesIDs (GEOM::GEOM_Object_ptr theShape,
                                                                const CORBA::Long     theShapeType,
                                                                const CORBA::Boolean  isSorted)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->SubShapeAllIDs(aShape, theShapeType, isSorted,
                                    GEOMImpl_IShapesOperations::EXPLODE_NEW_INCLUDE_MAIN);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSubShape
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetSubShape
                                           (GEOM::GEOM_Object_ptr theMainShape,
                                            const CORBA::Long     theID)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theMainShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  Handle(GEOM_Object) anObject = GetOperations()->GetSubShape(aShape, theID);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  MakeSubShapes
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::MakeSubShapes (GEOM::GEOM_Object_ptr theMainShape,
                                                         const GEOM::ListOfLong& theIndices)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;
  Standard_Integer i;

  //Set a not done flag
  GetOperations()->SetNotDone();

  if (theIndices.length() < 1)
    return aSeq._retn();

  Handle(GEOM_Object) aShape = GetObjectImpl(theMainShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HArray1OfInteger) anArray = new TColStd_HArray1OfInteger (1, theIndices.length());
  for (i = 0; i < theIndices.length(); i++)
    anArray->SetValue(i+1, theIndices[i]);

  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->MakeSubShapes(aShape, anArray);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (i = 0; i < aLength; i++)
    aSeq[i] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i+1)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSubShapeIndex
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::GetSubShapeIndex
  (GEOM::GEOM_Object_ptr theMainShape, GEOM::GEOM_Object_ptr theSubShape)
{
  //Get the reference shapes
  Handle(GEOM_Object) aMainShapeRef = GetObjectImpl(theMainShape);
  Handle(GEOM_Object) aSubShapeRef = GetObjectImpl(theSubShape);

  if (aMainShapeRef.IsNull() || aSubShapeRef.IsNull()) return -1;

  //Get the unique ID of <theSubShape> inside <theMainShape>
  CORBA::Long anID = GetOperations()->GetSubShapeIndex(aMainShapeRef, aSubShapeRef);
  if (!GetOperations()->IsDone())
    return -1;

  return anID;
}

//=============================================================================
/*!
 *  GetSubShapesIndices
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetSubShapesIndices
  (GEOM::GEOM_Object_ptr theMainShape, const GEOM::ListOfGO& theSubShapes)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;
  
  //Get the reference main shape
  Handle(GEOM_Object) aMainShapeRef = GetObjectImpl(theMainShape); 
  if (aMainShapeRef.IsNull()) return aSeq._retn();
      
  //Get the subshapes
  std::list<Handle(GEOM_Object)> aShapes;
  int aLen = theSubShapes.length();
  for (int ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theSubShapes[ind]);
    if (aSh.IsNull())
    {
      MESSAGE("NULL shape")
      return aSeq._retn();
    }
    aShapes.push_back(aSh);
  }

  //Get the IDs of <theSubShapes> inside <theMainShape>
  Handle(TColStd_HSequenceOfInteger) aHSeq = 
  GetOperations()->GetSubShapesIndices(aMainShapeRef, aShapes);
  
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}


//=============================================================================
/*!
 *  GetTopologyIndex
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::GetTopologyIndex
  (GEOM::GEOM_Object_ptr theMainShape, GEOM::GEOM_Object_ptr theSubShape)
{
  //Get the reference shapes
  Handle(GEOM_Object) aMainShapeRef = GetObjectImpl(theMainShape);
  Handle(GEOM_Object) aSubShapeRef = GetObjectImpl(theSubShape);

  if (aMainShapeRef.IsNull() || aSubShapeRef.IsNull()) return -1;

  //Get an ID of <theSubShape>, unique among all sub-shapes of <theMainShape> of the same type
  CORBA::Long anID = GetOperations()->GetTopologyIndex(aMainShapeRef, aSubShapeRef);
  if (!GetOperations()->IsDone())
    return -1;

  return anID;
}

//=============================================================================
/*!
 *  GetShapeTypeString
 */
//=============================================================================
char* GEOM_IShapesOperations_i::GetShapeTypeString (GEOM::GEOM_Object_ptr theShape)
{
  //Get the reference shape
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return NULL;

  // Get shape parameters
  TCollection_AsciiString aDescription = GetOperations()->GetShapeTypeString(aShape);
  return CORBA::string_dup(aDescription.ToCString());
}

//=============================================================================
/*!
 *  NumberOfFaces
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::NumberOfFaces (GEOM::GEOM_Object_ptr theShape)
{
  return NumberOfSubShapes(theShape, Standard_Integer(TopAbs_FACE));
}

//=============================================================================
/*!
 *  NumberOfEdges
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::NumberOfEdges (GEOM::GEOM_Object_ptr theShape)
{
  return NumberOfSubShapes(theShape, Standard_Integer(TopAbs_EDGE));
}

//=============================================================================
/*!
 *  NumberOfSubShapes
 */
//=============================================================================
CORBA::Long GEOM_IShapesOperations_i::NumberOfSubShapes (GEOM::GEOM_Object_ptr theShape,
                                                         const CORBA::Long     theShapeType)
{
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return -1;

  CORBA::Long aNb = GetOperations()->NumberOfSubShapes(aShape, theShapeType);
  if (!GetOperations()->IsDone()) return -1;

  return aNb;
}

//=============================================================================
/*!
 *  ChangeOrientation
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::ChangeOrientation
                                                (GEOM::GEOM_Object_ptr theShape)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aGEOMObject._retn();

  //Create the Solid
  Handle(GEOM_Object) anObject = GetOperations()->ReverseShape(aShape);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetFreeFacesIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetFreeFacesIDs (GEOM::GEOM_Object_ptr theShape)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  if (aShape.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetFreeFacesIDs(aShape);
  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSharedShapes
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetSharedShapes
                                          (GEOM::GEOM_Object_ptr theShape1,
                                           GEOM::GEOM_Object_ptr theShape2,
                                           const CORBA::Long     theShapeType)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  Handle(GEOM_Object) aShape1 = GetObjectImpl(theShape1);
  Handle(GEOM_Object) aShape2 = GetObjectImpl(theShape2);

  if (aShape1.IsNull() || aShape2.IsNull()) return aSeq._retn();

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetSharedShapes(aShape1, aShape2, theShapeType);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetSharedShapesMulti
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetSharedShapesMulti
                                          (const GEOM::ListOfGO& theShapes,
                                           const CORBA::Long     theShapeType)
{
  //Set a not done flag
  GetOperations()->SetNotDone();

  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Get the shapes
  std::list<Handle(GEOM_Object)> aShapes;
  int aLen = theShapes.length();
  for (int ind = 0; ind < aLen; ind++) {
    Handle(GEOM_Object) aSh = GetObjectImpl(theShapes[ind]);
    if (aSh.IsNull()) return aSeq._retn();
    aShapes.push_back(aSh);
  }

  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetSharedShapes(aShapes, theShapeType);
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

static GEOMAlgo_State ShapeState (const GEOM::shape_state theState)
{
  GEOMAlgo_State aState = GEOMAlgo_ST_UNKNOWN;

  switch (theState) {
  case GEOM::ST_ON:
    aState = GEOMAlgo_ST_ON;
    break;
  case GEOM::ST_OUT:
    aState = GEOMAlgo_ST_OUT;
    break;
  case GEOM::ST_ONOUT:
    aState = GEOMAlgo_ST_ONOUT;
    break;
  case GEOM::ST_IN:
    aState = GEOMAlgo_ST_IN;
    break;
  case GEOM::ST_ONIN:
    aState = GEOMAlgo_ST_ONIN;
    break;
  default:
    break;
  }

  return aState;
}

//=============================================================================
/*!
 *  GetShapesOnPlane
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnPlane
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAx1,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAx1 = GetObjectImpl(theAx1);

  if (aShape.IsNull() || anAx1.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetShapesOnPlane(aShape, theShapeType, anAx1, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnPlaneWithLocation
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnPlaneWithLocation
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAx1,
                                                 GEOM::GEOM_Object_ptr   thePnt,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAx1 = GetObjectImpl(theAx1);
  Handle(GEOM_Object) anPnt = GetObjectImpl(thePnt);

  if (aShape.IsNull() || anAx1.IsNull() || anPnt.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfTransient) aHSeq =
    GetOperations()->GetShapesOnPlaneWithLocation(aShape, theShapeType, anAx1, anPnt, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinder
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnCylinder
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAxis,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aShape.IsNull() || anAxis.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnCylinder
    (aShape, theShapeType, anAxis, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinderWithLocation
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnCylinderWithLocation
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAxis,
                                                 GEOM::GEOM_Object_ptr   thePnt,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);
  Handle(GEOM_Object) aPnt   = GetObjectImpl(thePnt);

  if (aShape.IsNull() || anAxis.IsNull() || aPnt.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnCylinderWithLocation
    (aShape, theShapeType, anAxis, aPnt, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnSphere
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnSphere
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theCenter,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aCenter = GetObjectImpl(theCenter);

  if (aShape.IsNull() || aCenter.IsNull()) return aSeq._retn();

  //Get Shapes On Sphere
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnSphere
    (aShape, theShapeType, aCenter, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnQuadrangle
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnQuadrangle
                                                (GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::GEOM_Object_ptr theTopLeftPoint,
                                                 GEOM::GEOM_Object_ptr theTopRigthPoint,
                                                 GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                                 GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aTopLeftPoint = GetObjectImpl(theTopLeftPoint);
  Handle(GEOM_Object) aTopRigthPoint = GetObjectImpl(theTopRigthPoint);
  Handle(GEOM_Object) aBottomLeftPoint = GetObjectImpl(theBottomLeftPoint);
  Handle(GEOM_Object) aBottomRigthPoint = GetObjectImpl(theBottomRigthPoint);

  if (aShape.IsNull() ||
      aTopLeftPoint.IsNull() ||
      aTopRigthPoint.IsNull() ||
      aBottomLeftPoint.IsNull() ||
      aBottomRigthPoint.IsNull())
    return aSeq._retn();

  //Get Shapes On Quadrangle
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnQuadrangle
    (aShape, theShapeType,
     aTopLeftPoint, aTopRigthPoint, aBottomLeftPoint, aBottomRigthPoint,
     ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnPlaneIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnPlaneIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAx1,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAx1 = GetObjectImpl(theAx1);

  if (aShape.IsNull() || anAx1.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetShapesOnPlaneIDs(aShape, theShapeType, anAx1, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnPlaneWithLocationIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnPlaneWithLocationIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAx1,
                                                 GEOM::GEOM_Object_ptr   thePnt,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAx1 = GetObjectImpl(theAx1);
  Handle(GEOM_Object) anPnt = GetObjectImpl(thePnt);

  if (aShape.IsNull() || anAx1.IsNull() || anPnt.IsNull()) return aSeq._retn();

  //Get Shapes On Plane
  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetShapesOnPlaneWithLocationIDs(aShape, theShapeType,
                                                     anAx1, anPnt, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinderIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnCylinderIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAxis,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);

  if (aShape.IsNull() || anAxis.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnCylinderIDs
    (aShape, theShapeType, anAxis, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnCylinderWithLocationIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnCylinderWithLocationIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theAxis,
                                                 GEOM::GEOM_Object_ptr   thePnt,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) anAxis = GetObjectImpl(theAxis);
  Handle(GEOM_Object) aPnt   = GetObjectImpl(thePnt);

  if (aShape.IsNull() || anAxis.IsNull() || aPnt.IsNull()) return aSeq._retn();

  //Get Shapes On Cylinder
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnCylinderWithLocationIDs
    (aShape, theShapeType, anAxis, aPnt, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnSphereIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnSphereIDs
                                                (GEOM::GEOM_Object_ptr   theShape,
                                                 const CORBA::Long       theShapeType,
                                                 GEOM::GEOM_Object_ptr   theCenter,
                                                 const CORBA::Double     theRadius,
                                                 const GEOM::shape_state theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aCenter = GetObjectImpl(theCenter);

  if (aShape.IsNull() || aCenter.IsNull()) return aSeq._retn();

  //Get Shapes On Sphere
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnSphereIDs
    (aShape, theShapeType, aCenter, theRadius, ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnQuadrangleIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnQuadrangleIDs
                                                (GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::GEOM_Object_ptr theTopLeftPoint,
                                                 GEOM::GEOM_Object_ptr theTopRigthPoint,
                                                 GEOM::GEOM_Object_ptr theBottomLeftPoint,
                                                 GEOM::GEOM_Object_ptr theBottomRigthPoint,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aTopLeftPoint = GetObjectImpl(theTopLeftPoint);
  Handle(GEOM_Object) aTopRigthPoint = GetObjectImpl(theTopRigthPoint);
  Handle(GEOM_Object) aBottomLeftPoint = GetObjectImpl(theBottomLeftPoint);
  Handle(GEOM_Object) aBottomRigthPoint = GetObjectImpl(theBottomRigthPoint);

  if (aShape.IsNull() ||
      aTopLeftPoint.IsNull() ||
      aTopRigthPoint.IsNull() ||
      aBottomLeftPoint.IsNull() ||
      aBottomRigthPoint.IsNull() )
    return aSeq._retn();

  //Get Shapes On Quadrangle
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnQuadrangleIDs
    (aShape, theShapeType,
     aTopLeftPoint, aTopRigthPoint, aBottomLeftPoint, aBottomRigthPoint,
     ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnBox
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnBox
                                                (GEOM::GEOM_Object_ptr theBox,
                                                 GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aBox = GetObjectImpl(theBox);

  if (aShape.IsNull() || aBox.IsNull() )
    return aSeq._retn();

  //Get Shapes On Box
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnBox
    (aBox,aShape, theShapeType,ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}

//=============================================================================
/*!
 *  GetShapesOnQuadrangleIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnBoxIDs
                                                (GEOM::GEOM_Object_ptr theBox,
                                                 GEOM::GEOM_Object_ptr theShape,
                                                 CORBA::Long           theShapeType,
                                                 GEOM::shape_state     theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aBox = GetObjectImpl(theBox);

  if (aShape.IsNull() || aBox.IsNull() )
    return aSeq._retn();

  //Get Shapes On Box
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnBoxIDs
    (aBox,aShape, theShapeType,ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}


//=============================================================================
/*!
 *  GetShapesOnShape
 */
//=============================================================================
GEOM::ListOfGO* GEOM_IShapesOperations_i::GetShapesOnShape
                                           (GEOM::GEOM_Object_ptr theCheckShape,
                                            GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Short          theShapeType,
                                            GEOM::shape_state     theState)
{
  GEOM::ListOfGO_var aSeq = new GEOM::ListOfGO;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aCheckShape = GetObjectImpl(theCheckShape);

  if (aShape.IsNull() || aCheckShape.IsNull() )
    return aSeq._retn();

  //Get Shapes On Shape
  Handle(TColStd_HSequenceOfTransient) aHSeq = GetOperations()->GetShapesOnShape
    (aCheckShape,aShape, theShapeType,ShapeState(theState));

  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = GetObject(Handle(GEOM_Object)::DownCast(aHSeq->Value(i)));

  return aSeq._retn();
}


//=============================================================================
/*!
 *  GetShapesOnShapeAsCompound
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetShapesOnShapeAsCompound
                                           (GEOM::GEOM_Object_ptr theCheckShape,
                                            GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Short          theShapeType,
                                            GEOM::shape_state     theState)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aCheckShape = GetObjectImpl(theCheckShape);

  if (aShape.IsNull() || aCheckShape.IsNull() )
    return aGEOMObject._retn();

  //Get Shapes On Shape
  Handle(GEOM_Object) anObject = GetOperations()->GetShapesOnShapeAsCompound
    (aCheckShape,aShape, theShapeType,ShapeState(theState));

  if (anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}


//=============================================================================
/*!
 *  GetShapesOnShapeIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetShapesOnShapeIDs
                                           (GEOM::GEOM_Object_ptr theCheckShape,
                                            GEOM::GEOM_Object_ptr theShape,
                                            CORBA::Short          theShapeType,
                                            GEOM::shape_state     theState)
{
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShape = GetObjectImpl(theShape);
  Handle(GEOM_Object) aCheckShape = GetObjectImpl(theCheckShape);

  if (aShape.IsNull() || aCheckShape.IsNull() )
    return aSeq._retn();

  //Get Shapes On Shape
  Handle(TColStd_HSequenceOfInteger) aHSeq = GetOperations()->GetShapesOnShapeIDs
    (aCheckShape,aShape, theShapeType,ShapeState(theState));
  if (!GetOperations()->IsDone() || aHSeq.IsNull())
    return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}


//=============================================================================
/*!
 *  GetInPlace
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetInPlace
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
                                           GEOM::GEOM_Object_ptr theShapeWhat)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetObjectImpl(theShapeWhere);
  Handle(GEOM_Object) aShapeWhat = GetObjectImpl(theShapeWhat);

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aGEOMObject._retn();

  //Get Shapes in place of aShapeWhat
  Handle(GEOM_Object) anObject =
    GetOperations()->GetInPlace(aShapeWhere, aShapeWhat);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetInPlaceOld
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetInPlaceOld
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
                                           GEOM::GEOM_Object_ptr theShapeWhat)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetObjectImpl(theShapeWhere);
  Handle(GEOM_Object) aShapeWhat = GetObjectImpl(theShapeWhat);

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aGEOMObject._retn();

  //Get Shapes in place of aShapeWhat
  Handle(GEOM_Object) anObject =
    GetOperations()->GetInPlaceOld(aShapeWhere, aShapeWhat);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetInPlaceByHistory
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetInPlaceByHistory
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
                                           GEOM::GEOM_Object_ptr theShapeWhat)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetObjectImpl(theShapeWhere);
  Handle(GEOM_Object) aShapeWhat = GetObjectImpl(theShapeWhat);

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aGEOMObject._retn();

  //Get Shapes in place of aShapeWhat
  Handle(GEOM_Object) anObject =
    GetOperations()->GetInPlaceByHistory(aShapeWhere, aShapeWhat);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetSame
 */
//=============================================================================
GEOM::GEOM_Object_ptr GEOM_IShapesOperations_i::GetSame
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
                                           GEOM::GEOM_Object_ptr theShapeWhat)
{
  GEOM::GEOM_Object_var aGEOMObject;

  //Set a not done flag
  GetOperations()->SetNotDone();

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetObjectImpl(theShapeWhere);
  Handle(GEOM_Object) aShapeWhat = GetObjectImpl(theShapeWhat);

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aGEOMObject._retn();

  //Get Shapes in place of aShapeWhat
  Handle(GEOM_Object) anObject =
    GetOperations()->GetSame(aShapeWhere, aShapeWhat);
  if (!GetOperations()->IsDone() || anObject.IsNull())
    return aGEOMObject._retn();

  return GetObject(anObject);
}

//=============================================================================
/*!
 *  GetSameIDs
 */
//=============================================================================
GEOM::ListOfLong* GEOM_IShapesOperations_i::GetSameIDs
                                          (GEOM::GEOM_Object_ptr theShapeWhere,
                                           GEOM::GEOM_Object_ptr theShapeWhat) {
  GEOM::ListOfLong_var aSeq = new GEOM::ListOfLong;

  //Get the reference objects
  Handle(GEOM_Object) aShapeWhere = GetObjectImpl(theShapeWhere);
  Handle(GEOM_Object) aShapeWhat = GetObjectImpl(theShapeWhat);

  if (aShapeWhere.IsNull() ||
      aShapeWhat.IsNull()) return aSeq._retn();


  Handle(TColStd_HSequenceOfInteger) aHSeq =
    GetOperations()->GetSameIDs(aShapeWhere, aShapeWhat);

  if (!GetOperations()->IsDone() || aHSeq.IsNull()) return aSeq._retn();

  Standard_Integer aLength = aHSeq->Length();
  aSeq->length(aLength);
  for (Standard_Integer i = 1; i <= aLength; i++)
    aSeq[i-1] = aHSeq->Value(i);

  return aSeq._retn();
}
