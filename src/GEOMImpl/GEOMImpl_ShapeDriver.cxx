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

#include <GEOMImpl_ShapeDriver.hxx>

#include <GEOMImpl_IIsoline.hxx>
#include <GEOMImpl_IShapes.hxx>
#include <GEOMImpl_IVector.hxx>
#include <GEOMImpl_Types.hxx>
#include <GEOMImpl_Block6Explorer.hxx>

#include <GEOM_Function.hxx>
#include <GEOMUtils_Hatcher.hxx>

// OCCT Includes
#include <ShapeFix_Wire.hxx>
#include <ShapeFix_Edge.hxx>
#include <ShapeFix_Shape.hxx>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeEdge.hxx>
#include <BRepTools_WireExplorer.hxx>

#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>

#include <TopAbs.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopTools_MapOfShape.hxx>
#include <TopTools_HSequenceOfShape.hxx>

#include <ElCLib.hxx>

#include <GCPnts_AbscissaPoint.hxx>

#include <Geom_TrimmedCurve.hxx>
#include <Geom_Surface.hxx>
#include <GeomAbs_CurveType.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomLProp.hxx>

#include <TColStd_SequenceOfReal.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom_Array1OfBSplineCurve.hxx>
#include <TColGeom_HArray1OfBSplineCurve.hxx>

#include <Precision.hxx>

#include <Standard_NullObject.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Standard_ConstructionError.hxx>

//modified by NIZNHY-PKV Wed Dec 28 13:48:20 2011f
//static
//  void KeepEdgesOrder(const Handle(TopTools_HSequenceOfShape)& aEdges,
//                    const Handle(TopTools_HSequenceOfShape)& aWires);
//modified by NIZNHY-PKV Wed Dec 28 13:48:23 2011t

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& GEOMImpl_ShapeDriver::GetID()
{
  static Standard_GUID aShapeDriver("FF1BBB54-5D14-4df2-980B-3A668264EA16");
  return aShapeDriver;
}


//=======================================================================
//function : GEOMImpl_ShapeDriver
//purpose  :
//=======================================================================
GEOMImpl_ShapeDriver::GEOMImpl_ShapeDriver()
{
}

//=======================================================================
//function : Execute
//purpose  :
//=======================================================================
Standard_Integer GEOMImpl_ShapeDriver::Execute(TFunction_Logbook& log) const
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) aFunction = GEOM_Function::GetFunction(Label());

  GEOMImpl_IShapes aCI (aFunction);
  Standard_Integer aType = aFunction->GetType();

  TopoDS_Shape aShape;
  TCollection_AsciiString aWarning;
  TopAbs_ShapeEnum anExpectedType = TopAbs_SHAPE;

  BRep_Builder B;

  if (aType == WIRE_EDGES) {
    anExpectedType = TopAbs_WIRE;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();

    Standard_Real aTolerance = aCI.GetTolerance();
    if (aTolerance < Precision::Confusion())
      aTolerance = Precision::Confusion();

    aShape = MakeWireFromEdges(aShapes, aTolerance);
  }
  else if (aType == FACE_WIRE) {
    anExpectedType = TopAbs_FACE;

    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aShapeBase = aRefBase->GetValue();
    if (aShapeBase.IsNull()) Standard_NullObject::Raise("Argument Shape is null");
    TopoDS_Wire W;
    if (aShapeBase.ShapeType() == TopAbs_WIRE) {
      W = TopoDS::Wire(aShapeBase);
      // check the wire is closed
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(W, aV1, aV2);
      if ( !aV1.IsNull() && !aV2.IsNull() && aV1.IsSame(aV2) )
        aShapeBase.Closed(true);
      else
        Standard_NullObject::Raise
          ("Shape for face construction is not closed");
    }
    else if (aShapeBase.ShapeType() == TopAbs_EDGE && aShapeBase.Closed()) {
      BRepBuilderAPI_MakeWire MW;
      MW.Add(TopoDS::Edge(aShapeBase));
      if (!MW.IsDone()) {
        Standard_ConstructionError::Raise("Wire construction failed");
      }
      W = MW;
    }
    else {
      Standard_NullObject::Raise
        ("Shape for face construction is neither a wire nor a closed edge");
    }
    aWarning = GEOMImpl_Block6Explorer::MakeFace(W, aCI.GetIsPlanar(), aShape);
    if (aShape.IsNull()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }
  }
  else if (aType == FACE_WIRES) {
    anExpectedType = TopAbs_FACE;

    // Try to build a face from a set of wires and edges
    int ind;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    int nbshapes = aShapes->Length();
    if (nbshapes < 1) {
      Standard_ConstructionError::Raise("No wires or edges given");
    }

    // 1. Extract all edges from the given arguments
    TopTools_MapOfShape aMapEdges;
    Handle(TopTools_HSequenceOfShape) aSeqEdgesIn = new TopTools_HSequenceOfShape;

    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefSh_i = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aSh_i = aRefSh_i->GetValue();

      TopExp_Explorer anExpE_i (aSh_i, TopAbs_EDGE);
      for (; anExpE_i.More(); anExpE_i.Next()) {
        if (aMapEdges.Add(anExpE_i.Current())) {
          aSeqEdgesIn->Append(anExpE_i.Current());
        }
      }
    }

    if (aSeqEdgesIn->IsEmpty()) {
      Standard_ConstructionError::Raise("No edges given");
    }

    // 2. Connect edges to wires of maximum length
    Handle(TopTools_HSequenceOfShape) aSeqWiresOut;
    ShapeAnalysis_FreeBounds::ConnectEdgesToWires(aSeqEdgesIn, Precision::Confusion(),
                                                  /*shared*/Standard_False, aSeqWiresOut);
    //modified by NIZNHY-PKV Wed Dec 28 13:46:55 2011f
    //KeepEdgesOrder(aSeqEdgesIn, aSeqWiresOut);
    //modified by NIZNHY-PKV Wed Dec 28 13:46:59 2011t

    // 3. Separate closed wires
    Handle(TopTools_HSequenceOfShape) aSeqClosedWires = new TopTools_HSequenceOfShape;
    Handle(TopTools_HSequenceOfShape) aSeqOpenWires = new TopTools_HSequenceOfShape;
    for (ind = 1; ind <= aSeqWiresOut->Length(); ind++) {
      if (aSeqWiresOut->Value(ind).Closed())
        aSeqClosedWires->Append(aSeqWiresOut->Value(ind));
      else
        aSeqOpenWires->Append(aSeqWiresOut->Value(ind));
    }

    if (aSeqClosedWires->Length() < 1) {
      Standard_ConstructionError::Raise
        ("There is no closed contour can be built from the given arguments");
    }

    // 4. Build a face / list of faces from all the obtained closed wires

    // 4.a. Basic face
    TopoDS_Shape aFFace;
    TopoDS_Wire aW1 = TopoDS::Wire(aSeqClosedWires->Value(1));
    aWarning = GEOMImpl_Block6Explorer::MakeFace(aW1, aCI.GetIsPlanar(), aFFace);
    if (aFFace.IsNull()) {
      Standard_ConstructionError::Raise("Face construction failed");
    }

    // 4.b. Add other wires
    if (aSeqClosedWires->Length() == 1) {
      aShape = aFFace;
    }
    else {
      TopoDS_Compound C;
      BRep_Builder aBuilder;
      aBuilder.MakeCompound(C);
      BRepAlgo_FaceRestrictor FR;

      TopAbs_Orientation OriF = aFFace.Orientation();
      TopoDS_Shape aLocalS = aFFace.Oriented(TopAbs_FORWARD);
      FR.Init(TopoDS::Face(aLocalS), Standard_False, Standard_True);

      for (ind = 1; ind <= aSeqClosedWires->Length(); ind++) {
        TopoDS_Wire aW = TopoDS::Wire(aSeqClosedWires->Value(ind));
        FR.Add(aW);
      }

      FR.Perform();

      if (FR.IsDone()) {
        int k = 0;
        TopoDS_Shape aFace;
        for (; FR.More(); FR.Next()) {
          aFace = FR.Current().Oriented(OriF);
          aBuilder.Add(C, aFace);
          k++;
        }
        if (k == 1) {
          aShape = aFace;
        } else {
          aShape = C;
        }
      }
    }

    // 5. Add all open wires to the result
    if (aSeqOpenWires->Length() > 0) {
      //Standard_ConstructionError::Raise("There are some open wires");
      TopoDS_Compound C;
      BRep_Builder aBuilder;
      if (aSeqClosedWires->Length() == 1) {
        aBuilder.MakeCompound(C);
        aBuilder.Add(C, aShape);
      }
      else {
        C = TopoDS::Compound(aShape);
      }

      for (ind = 1; ind <= aSeqOpenWires->Length(); ind++) {
        aBuilder.Add(C, aSeqOpenWires->Value(ind));
      }

      aShape = C;
    }
  }
  else if (aType == SHELL_FACES) {
    anExpectedType = TopAbs_SHELL;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add faces
    BRepBuilderAPI_Sewing aSewing (Precision::Confusion()*10.0);
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Face for shell construction is null");
      }
      aSewing.Add(aShape_i);
    }

    aSewing.Perform();

    TopoDS_Shape sh = aSewing.SewedShape();

    if (sh.ShapeType()==TopAbs_FACE && nbshapes==1) {
      // case for creation of shell from one face - PAL12722 (skl 26.06.2006)
      TopoDS_Shell ss;
      B.MakeShell(ss);
      B.Add(ss,sh);
      aShape = ss;
    }
    else {
      //TopExp_Explorer exp (aSewing.SewedShape(), TopAbs_SHELL);
      TopExp_Explorer exp (sh, TopAbs_SHELL);
      Standard_Integer ish = 0;
      for (; exp.More(); exp.Next()) {
        aShape = exp.Current();
        ish++;
      }

      if (ish != 1) {
        // try the case of one face (Mantis issue 0021809)
        TopExp_Explorer expF (sh, TopAbs_FACE);
        Standard_Integer ifa = 0;
        for (; expF.More(); expF.Next()) {
          aShape = expF.Current();
          ifa++;
        }

        if (ifa == 1) {
          TopoDS_Shell ss;
          B.MakeShell(ss);
          B.Add(ss,aShape);
          aShape = ss;
        }
        else {
          aShape = aSewing.SewedShape();
        }
      }
    }

  }
  else if (aType == SOLID_SHELLS) {
    anExpectedType = TopAbs_SOLID;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();
    Standard_Integer ish = 0;
    BRepBuilderAPI_MakeSolid aMkSolid;

    // add shapes
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShapeShell = aRefShape->GetValue();
      if (aShapeShell.IsNull()) {
        Standard_NullObject::Raise("Shell for solid construction is null");
      }
      if (aShapeShell.ShapeType() == TopAbs_COMPOUND) {
        TopoDS_Iterator It (aShapeShell, Standard_True, Standard_True);
        if (It.More()) aShapeShell = It.Value();
      }
      if (aShapeShell.ShapeType() == TopAbs_SHELL) {
        aMkSolid.Add(TopoDS::Shell(aShapeShell));
        ish++;
      }
    }
    if (ish == 0 || !aMkSolid.IsDone()) return 0;

    TopoDS_Solid Sol = aMkSolid.Solid();
    BRepClass3d_SolidClassifier SC (Sol);
    SC.PerformInfinitePoint(Precision::Confusion());
    if (SC.State() == TopAbs_IN)
      aShape = Sol.Reversed();
    else
      aShape = Sol;
  }
  else if (aType == COMPOUND_SHAPES) {
    anExpectedType = TopAbs_COMPOUND;

    Handle(TColStd_HSequenceOfTransient) aShapes = aCI.GetShapes();
    unsigned int ind, nbshapes = aShapes->Length();

    // add shapes
    TopoDS_Compound C;
    B.MakeCompound(C);
    for (ind = 1; ind <= nbshapes; ind++) {
      Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(aShapes->Value(ind));
      TopoDS_Shape aShape_i = aRefShape->GetValue();
      if (aShape_i.IsNull()) {
        Standard_NullObject::Raise("Shape for compound construction is null");
      }
      B.Add(C, aShape_i);
    }

    aShape = C;

  }
  else if (aType == EDGE_WIRE) {
    anExpectedType = TopAbs_EDGE;

    Handle(GEOM_Function) aRefBase = aCI.GetBase();
    TopoDS_Shape aWire = aRefBase->GetValue();
    Standard_Real LinTol = aCI.GetTolerance();
    Standard_Real AngTol = aCI.GetAngularTolerance();
    if (aWire.IsNull()) Standard_NullObject::Raise("Argument Wire is null");

    aShape = MakeEdgeFromWire(aWire, LinTol, AngTol);
  }
  else if (aType == EDGE_CURVE_LENGTH) {
    anExpectedType = TopAbs_EDGE;

    GEOMImpl_IVector aVI (aFunction);

    // RefCurve
    Handle(GEOM_Function) aRefCurve = aVI.GetPoint1();
    if (aRefCurve.IsNull()) Standard_NullObject::Raise("Argument Curve is null");
    TopoDS_Shape aRefShape1 = aRefCurve->GetValue();
    if (aRefShape1.ShapeType() != TopAbs_EDGE) {
      Standard_TypeMismatch::Raise
        ("Edge On Curve creation aborted : curve shape is not an edge");
    }
    TopoDS_Edge aRefEdge = TopoDS::Edge(aRefShape1);
    TopoDS_Vertex V1, V2;
    TopExp::Vertices(aRefEdge, V1, V2, Standard_True);

    // RefPoint
    TopoDS_Vertex aRefVertex;
    Handle(GEOM_Function) aRefPoint = aVI.GetPoint2();
    if (aRefPoint.IsNull()) {
      aRefVertex = V1;
    }
    else {
      TopoDS_Shape aRefShape2 = aRefPoint->GetValue();
      if (aRefShape2.ShapeType() != TopAbs_VERTEX) {
        Standard_TypeMismatch::Raise
          ("Edge On Curve creation aborted : start point shape is not a vertex");
      }
      aRefVertex = TopoDS::Vertex(aRefShape2);
    }
    gp_Pnt aRefPnt = BRep_Tool::Pnt(aRefVertex);

    // Length
    Standard_Real aLength = aVI.GetParameter();
    //Standard_Real aCurveLength = IntTools::Length(aRefEdge);
    //if (aLength > aCurveLength) {
    //  Standard_ConstructionError::Raise
    //    ("Edge On Curve creation aborted : given length is greater than edges length");
    //}
    if (fabs(aLength) < Precision::Confusion()) {
      Standard_ConstructionError::Raise
        ("Edge On Curve creation aborted : given length is smaller than Precision::Confusion()");
    }

    // Check orientation
    Standard_Real UFirst, ULast;
    Handle(Geom_Curve) EdgeCurve = BRep_Tool::Curve(aRefEdge, UFirst, ULast);
    Handle(Geom_Curve) ReOrientedCurve = EdgeCurve;

    Standard_Real dU = ULast - UFirst;
    Standard_Real par1 = UFirst + 0.1 * dU;
    Standard_Real par2 = ULast  - 0.1 * dU;

    gp_Pnt P1 = EdgeCurve->Value(par1);
    gp_Pnt P2 = EdgeCurve->Value(par2);

    if (aRefPnt.SquareDistance(P2) < aRefPnt.SquareDistance(P1)) {
      ReOrientedCurve = EdgeCurve->Reversed();
      UFirst = EdgeCurve->ReversedParameter(ULast);
    }

    // Get the point by length
    GeomAdaptor_Curve AdapCurve = GeomAdaptor_Curve(ReOrientedCurve);
    GCPnts_AbscissaPoint anAbsPnt (AdapCurve, aLength, UFirst);
    Standard_Real aParam = anAbsPnt.Parameter();

    if (AdapCurve.IsClosed() && aLength < 0.0) {
      Standard_Real aTmp = aParam;
      aParam = UFirst;
      UFirst = aTmp;
    }

    BRepBuilderAPI_MakeEdge aME (ReOrientedCurve, UFirst, aParam);
    if (aME.IsDone())
      aShape = aME.Shape();
  } else if (aType == SHAPE_ISOLINE) {
    GEOMImpl_IIsoline     aII (aFunction);
    Handle(GEOM_Function) aRefFace = aII.GetFace();
    TopoDS_Shape          aShapeFace = aRefFace->GetValue();

    if (aShapeFace.ShapeType() == TopAbs_FACE) {
      TopoDS_Face   aFace  = TopoDS::Face(aShapeFace);
      bool          isUIso = aII.GetIsUIso();
      Standard_Real aParam = aII.GetParameter();
      Standard_Real U1,U2,V1,V2;

      // Construct a real geometric parameter.
      aFace.Orientation(TopAbs_FORWARD);
      ShapeAnalysis::GetFaceUVBounds(aFace,U1,U2,V1,V2);

      if (isUIso) {
        aParam = U1 + (U2 - U1)*aParam;
      } else {
        aParam = V1 + (V2 - V1)*aParam;
      }

      aShape = MakeIsoline(aFace, isUIso, aParam);
    } else {
      Standard_NullObject::Raise
        ("Shape for isoline construction is not a face");
    }
  }
  else {
  }

  if (aShape.IsNull()) return 0;

  // Check shape validity
  BRepCheck_Analyzer ana (aShape, false);
  if (!ana.IsValid()) {
    //Standard_ConstructionError::Raise("Algorithm have produced an invalid shape result");
    // For Mantis issue 0021772: EDF 2336 GEOM: Non valid face created from two circles
    Handle(ShapeFix_Shape) aSfs = new ShapeFix_Shape (aShape);
    aSfs->Perform();
    aShape = aSfs->Shape();
  }

  // Check if the result shape type is compatible with the expected.
  const TopAbs_ShapeEnum aShType = aShape.ShapeType();

  if (anExpectedType != TopAbs_SHAPE && anExpectedType != aShType) {
    Standard_ConstructionError::Raise("Result type check failed");
  }

  aFunction->SetValue(aShape);

  log.SetTouched(Label());

  if (!aWarning.IsEmpty())
    Standard_Failure::Raise(aWarning.ToCString());

  return 1;
}

TopoDS_Wire GEOMImpl_ShapeDriver::MakeWireFromEdges(const Handle(TColStd_HSequenceOfTransient)& theEdgesFuncs,
                                                    const Standard_Real theTolerance)
{
  BRep_Builder B;

  TopoDS_Wire aWire;
  B.MakeWire(aWire);

  // add edges
  for (unsigned int ind = 1; ind <= theEdgesFuncs->Length(); ind++) {
    Handle(GEOM_Function) aRefShape = Handle(GEOM_Function)::DownCast(theEdgesFuncs->Value(ind));
    TopoDS_Shape aShape_i = aRefShape->GetValue();
    if (aShape_i.IsNull()) {
      Standard_NullObject::Raise("Shape for wire construction is null");
    }
    if (aShape_i.ShapeType() == TopAbs_EDGE || aShape_i.ShapeType() == TopAbs_WIRE) {
      TopExp_Explorer exp (aShape_i, TopAbs_EDGE);
      for (; exp.More(); exp.Next())
        B.Add(aWire, TopoDS::Edge(exp.Current()));
    } else {
      Standard_TypeMismatch::Raise
        ("Shape for wire construction is neither an edge nor a wire");
    }
  }

  // fix edges order
  Handle(ShapeFix_Wire) aFW = new ShapeFix_Wire;
  aFW->Load(aWire);
  aFW->FixReorder();

  if (aFW->StatusReorder(ShapeExtend_FAIL1)) {
    Standard_ConstructionError::Raise("Wire construction failed: several loops detected");
  }
  else if (aFW->StatusReorder(ShapeExtend_FAIL)) {
    Standard_ConstructionError::Raise("Wire construction failed");
  }
  else {
  }

  // IMP 0019766: Building a Wire from unconnected edges by introducing a tolerance
  aFW->ClosedWireMode() = Standard_False;
  aFW->FixConnected(theTolerance);
  if (aFW->StatusConnected(ShapeExtend_FAIL)) {
    Standard_ConstructionError::Raise("Wire construction failed: cannot build connected wire");
  }
  // IMP 0019766
  if (aFW->StatusConnected(ShapeExtend_DONE3)) {
    // Confused with <prec> but not Analyzer.Precision(), set the same
    aFW->FixGapsByRangesMode() = Standard_True;
    if (aFW->FixGaps3d()) {
      Handle(ShapeExtend_WireData) sbwd = aFW->WireData();
      Handle(ShapeFix_Edge) aFe = new ShapeFix_Edge;
      for (Standard_Integer iedge = 1; iedge <= sbwd->NbEdges(); iedge++) {
        TopoDS_Edge aEdge = TopoDS::Edge(sbwd->Edge(iedge));
        aFe->FixVertexTolerance(aEdge);
        aFe->FixSameParameter(aEdge);
      }
    }
    else if (aFW->StatusGaps3d(ShapeExtend_FAIL)) {
      Standard_ConstructionError::Raise("Wire construction failed: cannot fix 3d gaps");
    }
  }
  aWire = aFW->WireAPIMake();

  return aWire;
}

TopoDS_Edge GEOMImpl_ShapeDriver::MakeEdgeFromWire(const TopoDS_Shape& aWire,
                                                   const Standard_Real LinTol,
                                                   const Standard_Real AngTol)
{
    TopoDS_Edge ResEdge;

    BRepLib::BuildCurves3d(aWire);
    Handle(ShapeFix_Shape) Fixer = new ShapeFix_Shape(aWire);
    Fixer->SetPrecision(LinTol);
    Fixer->SetMaxTolerance(LinTol);
    Fixer->Perform();
    TopoDS_Wire theWire = TopoDS::Wire(Fixer->Shape());

    TColGeom_SequenceOfCurve CurveSeq;
    TopTools_SequenceOfShape LocSeq;
    TColStd_SequenceOfReal FparSeq;
    TColStd_SequenceOfReal LparSeq;
    TColStd_SequenceOfReal TolSeq;
    GeomAbs_CurveType CurType;
    TopoDS_Vertex FirstVertex, LastVertex;
    Standard_Real aPntShiftDist = 0.;

    BRepTools_WireExplorer wexp(theWire) ;
    for (; wexp.More(); wexp.Next())
    {
      TopoDS_Edge anEdge = wexp.Current();
      Standard_Real fpar, lpar;
      TopLoc_Location aLoc;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aLoc, fpar, lpar);
      if (aCurve.IsNull())
        continue;

      BRepAdaptor_Curve BAcurve(anEdge);
      GeomAbs_CurveType aType = BAcurve.GetType();

      Handle(Geom_Curve) aBasisCurve = BAcurve.Curve().Curve();

      if (aBasisCurve->IsPeriodic())
        ElCLib::AdjustPeriodic(aBasisCurve->FirstParameter(), aBasisCurve->LastParameter(),
                               Precision::PConfusion(), fpar, lpar);

      if (CurveSeq.IsEmpty())
      {
        CurveSeq.Append(aCurve);
        TopoDS_Shape aLocShape;
        aLocShape.Location(aLoc);
        aLocShape.Orientation(wexp.Orientation());
        LocSeq.Append(aLocShape);
        FparSeq.Append(fpar);
        LparSeq.Append(lpar);
        CurType = aType;
        FirstVertex = wexp.CurrentVertex();
      }
      else
      {
        Standard_Boolean Done = Standard_False;
        Standard_Real NewFpar, NewLpar;
        GeomAdaptor_Curve GAprevcurve(CurveSeq.Last());
        TopoDS_Vertex CurVertex = wexp.CurrentVertex();
        TopoDS_Vertex CurFirstVer = TopExp::FirstVertex(anEdge);
        TopAbs_Orientation ConnectByOrigin = (CurVertex.IsSame(CurFirstVer))? TopAbs_FORWARD : TopAbs_REVERSED;
        if (aCurve == CurveSeq.Last())
        {
          NewFpar = fpar;
          NewLpar = lpar;
          if (aBasisCurve->IsPeriodic())
          {
            if (NewLpar < NewFpar)
              NewLpar += aBasisCurve->Period();
            if (ConnectByOrigin == TopAbs_FORWARD)
              ElCLib::AdjustPeriodic(FparSeq.Last(),
                                     FparSeq.Last() + aBasisCurve->Period(),
                                     Precision::PConfusion(), NewFpar, NewLpar);
            else
              ElCLib::AdjustPeriodic(FparSeq.Last() - aBasisCurve->Period(),
                                     FparSeq.Last(),
                                     Precision::PConfusion(), NewFpar, NewLpar);
          }
          Done = Standard_True;
        }
        else if (aType == CurType &&
                 aType != GeomAbs_BezierCurve &&
                 aType != GeomAbs_BSplineCurve &&
                 aType != GeomAbs_OtherCurve)
        {
          switch (aType)
          {
          case GeomAbs_Line:
            {
              gp_Lin aLine    = BAcurve.Line();
              gp_Lin PrevLine = GAprevcurve.Line();
              if (aLine.Contains(PrevLine.Location(), LinTol) &&
                  aLine.Direction().IsParallel(PrevLine.Direction(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aLine);
                gp_Pnt P2 = ElCLib::Value(lpar, aLine);
                NewFpar = ElCLib::Parameter(PrevLine, P1);
                NewLpar = ElCLib::Parameter(PrevLine, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevLine);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevLine);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Circle:
            {
              gp_Circ aCircle    = BAcurve.Circle();
              gp_Circ PrevCircle = GAprevcurve.Circle();
              if (aCircle.Location().Distance(PrevCircle.Location()) <= LinTol &&
                  Abs(aCircle.Radius() - PrevCircle.Radius()) <= LinTol &&
                  aCircle.Axis().IsParallel(PrevCircle.Axis(), AngTol))
              {
                const Standard_Boolean isFwd = ConnectByOrigin == TopAbs_FORWARD;

                if (aCircle.Axis().Direction() * PrevCircle.Axis().Direction() < 0.)
                {
                  Standard_Real memfpar = fpar;
                  fpar = lpar;
                  lpar = memfpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                gp_Pnt P1 = ElCLib::Value(fpar, aCircle);
                gp_Pnt P2 = ElCLib::Value(lpar, aCircle);
                NewFpar = ElCLib::Parameter(PrevCircle, P1);
                NewLpar = ElCLib::Parameter(PrevCircle, P2);

                // Compute shift
                if (isFwd) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevCircle);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevCircle);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                  NewLpar += 2.*M_PI;
                //Standard_Real MemNewFpar = NewFpar, MemNewLpar =  NewLpar;
                if (ConnectByOrigin == TopAbs_FORWARD)
                  ElCLib::AdjustPeriodic(FparSeq.Last(),
                                         FparSeq.Last() + 2.*M_PI,
                                         Precision::PConfusion(), NewFpar, NewLpar);
                else
                  ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
                                         FparSeq.Last(),
                                         Precision::PConfusion(), NewFpar, NewLpar);
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Ellipse:
            {
              gp_Elips anEllipse   = BAcurve.Ellipse();
              gp_Elips PrevEllipse = GAprevcurve.Ellipse();
              if (anEllipse.Focus1().Distance(PrevEllipse.Focus1()) <= LinTol &&
                  anEllipse.Focus2().Distance(PrevEllipse.Focus2()) <= LinTol &&
                  Abs(anEllipse.MajorRadius() - PrevEllipse.MajorRadius()) <= LinTol &&
                  Abs(anEllipse.MinorRadius() - PrevEllipse.MinorRadius()) <= LinTol &&
                  anEllipse.Axis().IsParallel(PrevEllipse.Axis(), AngTol))
              {
                const Standard_Boolean isFwd = ConnectByOrigin == TopAbs_FORWARD;

                if (anEllipse.Axis().Direction() * PrevEllipse.Axis().Direction() < 0.)
                {
                  Standard_Real memfpar = fpar;
                  fpar = lpar;
                  lpar = memfpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                gp_Pnt P1 = ElCLib::Value(fpar, anEllipse);
                gp_Pnt P2 = ElCLib::Value(lpar, anEllipse);
                NewFpar = ElCLib::Parameter(PrevEllipse, P1);
                NewLpar = ElCLib::Parameter(PrevEllipse, P2);

                // Compute shift
                if (isFwd) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevEllipse);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevEllipse);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                  NewLpar += 2.*M_PI;
                if (ConnectByOrigin == TopAbs_FORWARD)
                  ElCLib::AdjustPeriodic(FparSeq.Last(),
                                         FparSeq.Last() + 2.*M_PI,
                                         Precision::PConfusion(), NewFpar, NewLpar);
                else
                  ElCLib::AdjustPeriodic(FparSeq.Last() - 2.*M_PI,
                                         FparSeq.Last(),
                                         Precision::PConfusion(), NewFpar, NewLpar);
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Hyperbola:
            {
              gp_Hypr aHypr    = BAcurve.Hyperbola();
              gp_Hypr PrevHypr = GAprevcurve.Hyperbola();
              if (aHypr.Focus1().Distance(PrevHypr.Focus1()) <= LinTol &&
                  aHypr.Focus2().Distance(PrevHypr.Focus2()) <= LinTol &&
                  Abs(aHypr.MajorRadius() - PrevHypr.MajorRadius()) <= LinTol &&
                  Abs(aHypr.MinorRadius() - PrevHypr.MinorRadius()) <= LinTol &&
                  aHypr.Axis().IsParallel(PrevHypr.Axis(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aHypr);
                gp_Pnt P2 = ElCLib::Value(lpar, aHypr);
                NewFpar = ElCLib::Parameter(PrevHypr, P1);
                NewLpar = ElCLib::Parameter(PrevHypr, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevHypr);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevHypr);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          case GeomAbs_Parabola:
            {
              gp_Parab aParab    = BAcurve.Parabola();
              gp_Parab PrevParab = GAprevcurve.Parabola();
              if (aParab.Location().Distance(PrevParab.Location()) <= LinTol &&
                  aParab.Focus().Distance(PrevParab.Focus()) <= LinTol &&
                  Abs(aParab.Focal() - PrevParab.Focal()) <= LinTol &&
                  aParab.Axis().IsParallel(PrevParab.Axis(), AngTol))
              {
                gp_Pnt P1 = ElCLib::Value(fpar, aParab);
                gp_Pnt P2 = ElCLib::Value(lpar, aParab);
                NewFpar = ElCLib::Parameter(PrevParab, P1);
                NewLpar = ElCLib::Parameter(PrevParab, P2);

                // Compute shift
                if (ConnectByOrigin == TopAbs_FORWARD) {
                  gp_Pnt aNewP2 = ElCLib::Value(NewLpar, PrevParab);

                  aPntShiftDist += P2.Distance(aNewP2);
                } else {
                  gp_Pnt aNewP1 = ElCLib::Value(NewFpar, PrevParab);

                  aPntShiftDist += P1.Distance(aNewP1);
                }

                if (NewLpar < NewFpar)
                {
                  Standard_Real MemNewFpar = NewFpar;
                  NewFpar = NewLpar;
                  NewLpar = MemNewFpar;
                  ConnectByOrigin = TopAbs::Reverse(ConnectByOrigin);
                }
                Done = Standard_True;
              }
              break;
            }
          } //end of switch (aType)
        } // end of else if (aType == CurType && ...
        if (Done)
        {
          if (NewFpar < FparSeq.Last())
            FparSeq(FparSeq.Length()) = NewFpar;
          else
            LparSeq(LparSeq.Length()) = NewLpar;
        }
        else
        {
          CurveSeq.Append(aCurve);
          TopoDS_Shape aLocShape;
          aLocShape.Location(aLoc);
          aLocShape.Orientation(wexp.Orientation());
          LocSeq.Append(aLocShape);
          FparSeq.Append(fpar);
          LparSeq.Append(lpar);
          TolSeq.Append(aPntShiftDist + BRep_Tool::Tolerance(CurVertex));
          aPntShiftDist = 0.;
          CurType = aType;
        }
      } // end of else (CurveSeq.IsEmpty()) -> not first time
    } // end for (; wexp.More(); wexp.Next())

    LastVertex = wexp.CurrentVertex();
    TolSeq.Append(aPntShiftDist + BRep_Tool::Tolerance(LastVertex));

    FirstVertex.Orientation(TopAbs_FORWARD);
    LastVertex.Orientation(TopAbs_REVERSED);

    if (!CurveSeq.IsEmpty())
    {
      Standard_Integer nb_curve = CurveSeq.Length();   //number of curves
      TColGeom_Array1OfBSplineCurve tab(0,nb_curve-1);                    //array of the curves
      TColStd_Array1OfReal tabtolvertex(0,nb_curve-1); //(0,nb_curve-2);  //array of the tolerances

      Standard_Integer i;

      if (nb_curve > 1)
      {
        for (i = 1; i <= nb_curve; i++)
        {
          if (CurveSeq(i)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
            CurveSeq(i) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(i))))->BasisCurve();

          Handle(Geom_TrimmedCurve) aTrCurve = new Geom_TrimmedCurve(CurveSeq(i), FparSeq(i), LparSeq(i));
          tab(i-1) = GeomConvert::CurveToBSplineCurve(aTrCurve);
          tab(i-1)->Transform(LocSeq(i).Location().Transformation());
          GeomConvert::C0BSplineToC1BSplineCurve(tab(i-1), Precision::Confusion());
          if (LocSeq(i).Orientation() == TopAbs_REVERSED)
            tab(i-1)->Reverse();

          //Temporary
          //char* name = new char[100];
          //sprintf(name, "c%d", i);
          //DrawTrSurf::Set(name, tab(i-1));

          if (i > 1)
            tabtolvertex(i-2) = TolSeq(i-1);
        } // end for (i = 1; i <= nb_curve; i++)
        tabtolvertex(nb_curve-1) = TolSeq(TolSeq.Length());

        Standard_Boolean closed_flag = Standard_False;
        Standard_Real closed_tolerance = 0.;
        if (FirstVertex.IsSame(LastVertex) &&
            GeomLProp::Continuity(tab(0), tab(nb_curve-1),
                                  tab(0)->FirstParameter(),
                                  tab(nb_curve-1)->LastParameter(),
                                  Standard_False, Standard_False, LinTol, AngTol) >= GeomAbs_G1)
        {
          closed_flag = Standard_True ;
          closed_tolerance = BRep_Tool::Tolerance(FirstVertex);
        }

        Handle(TColGeom_HArray1OfBSplineCurve)  concatcurve;     //array of the concatenated curves
        Handle(TColStd_HArray1OfInteger)        ArrayOfIndices;  //array of the remining Vertex
        GeomConvert::ConcatC1(tab,
                              tabtolvertex,
                              ArrayOfIndices,
                              concatcurve,
                              closed_flag,
                              closed_tolerance);   //C1 concatenation

        if (concatcurve->Length() > 1)
        {
          GeomConvert_CompCurveToBSplineCurve Concat(concatcurve->Value(concatcurve->Lower()));

          for (i = concatcurve->Lower()+1; i <= concatcurve->Upper(); i++)
            Concat.Add( concatcurve->Value(i), LinTol, Standard_True );

          concatcurve->SetValue(concatcurve->Lower(), Concat.BSplineCurve());
        }
        // rnc : prevents the driver from building an edge without C1 continuity
        if (concatcurve->Value(concatcurve->Lower())->Continuity()==GeomAbs_C0){
          Standard_ConstructionError::Raise("Construction aborted : The given Wire has sharp bends between some Edges, no valid Edge can be built");
        }

        Standard_Boolean isValidEndVtx = Standard_True;

        if (closed_flag) {
          // Check if closed curve is reordered.
          Handle(Geom_Curve) aCurve  = concatcurve->Value(concatcurve->Lower());
          Standard_Real      aFPar   = aCurve->FirstParameter();
          gp_Pnt             aPFirst;
          gp_Pnt             aPntVtx = BRep_Tool::Pnt(FirstVertex);
          Standard_Real      aTolVtx = BRep_Tool::Tolerance(FirstVertex);

          aCurve->D0(aFPar, aPFirst);

          if (!aPFirst.IsEqual(aPntVtx, aTolVtx)) {
            // The curve is reordered. Find the new first and last vertices.
            TopTools_IndexedMapOfShape aMapVtx;
            TopExp::MapShapes(theWire, TopAbs_VERTEX, aMapVtx);

            const Standard_Integer aNbVtx = aMapVtx.Extent();
            Standard_Integer       iVtx;

            for (iVtx = 1; iVtx <= aNbVtx; iVtx++) {
              const TopoDS_Vertex aVtx = TopoDS::Vertex(aMapVtx.FindKey(iVtx));
              const gp_Pnt        aPnt = BRep_Tool::Pnt(aVtx);
              const Standard_Real aTol = BRep_Tool::Tolerance(aVtx);

              if (aPFirst.IsEqual(aPnt, aTol)) {
                // The coinsident vertex is found.
                FirstVertex = aVtx;
                LastVertex  = aVtx;
                FirstVertex.Orientation(TopAbs_FORWARD);
                LastVertex.Orientation(TopAbs_REVERSED);
                break;
              }
            }

            if (iVtx > aNbVtx) {
              // It is necessary to create new vertices.
              isValidEndVtx = Standard_False;
            }
          }
        }

        if (isValidEndVtx) {
          ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
                                     FirstVertex, LastVertex,
                                     concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                                     concatcurve->Value(concatcurve->Lower())->LastParameter());
        } else {
          ResEdge = BRepLib_MakeEdge(concatcurve->Value(concatcurve->Lower()),
                                     concatcurve->Value(concatcurve->Lower())->FirstParameter(),
                                     concatcurve->Value(concatcurve->Lower())->LastParameter());
        }
      }
      else
      {
        if (CurveSeq(1)->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
          CurveSeq(1) = (*((Handle(Geom_TrimmedCurve)*)&(CurveSeq(1))))->BasisCurve();

        Handle(Geom_Curve) aNewCurve =
          Handle(Geom_Curve)::DownCast(CurveSeq(1)->Copy());

        aNewCurve->Transform(LocSeq(1).Location().Transformation());

        if (LocSeq(1).Orientation() == TopAbs_REVERSED) {
          const TopoDS_Vertex aVtxTmp = FirstVertex;

          FirstVertex = LastVertex;
          LastVertex  = aVtxTmp;
          FirstVertex.Orientation(TopAbs_FORWARD);
          LastVertex.Orientation(TopAbs_REVERSED);
        }

        ResEdge = BRepLib_MakeEdge(aNewCurve,
                                   FirstVertex, LastVertex,
                                   FparSeq(1), LparSeq(1));

        if (LocSeq(1).Orientation() == TopAbs_REVERSED) {
          ResEdge.Reverse();
        }
      }
    }

    return ResEdge;
}

//=============================================================================
/*!
 * \brief Returns an isoline for a face.
 */
//=============================================================================

TopoDS_Shape GEOMImpl_ShapeDriver::MakeIsoline
                            (const TopoDS_Face &theFace,
                             const bool         IsUIso,
                             const double       theParameter) const
{
  TopoDS_Shape          aResult;
  GEOMUtils::Hatcher    aHatcher(theFace);
  const GeomAbs_IsoType aType = (IsUIso ? GeomAbs_IsoU : GeomAbs_IsoV);

  aHatcher.Init(aType, theParameter);
  aHatcher.Perform();

  if (!aHatcher.IsDone()) {
    Standard_ConstructionError::Raise("MakeIsoline : Hatcher failure");
  }

  const Handle(TColStd_HArray1OfInteger) &anIndices =
    (IsUIso ? aHatcher.GetUIndices() : aHatcher.GetVIndices());

  if (anIndices.IsNull()) {
    Standard_ConstructionError::Raise("MakeIsoline : Null hatching indices");
  }

  const Standard_Integer anIsoInd = anIndices->Lower();
  const Standard_Integer aHatchingIndex = anIndices->Value(anIsoInd);

  if (aHatchingIndex == 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Invalid hatching index");
  }

  const Standard_Integer aNbDomains =
    aHatcher.GetNbDomains(aHatchingIndex);

  if (aNbDomains < 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Invalid number of domains");
  }

  // The hatching is performed successfully. Create the 3d Curve.
  Handle(Geom_Surface) aSurface   = BRep_Tool::Surface(theFace);
  Handle(Geom_Curve)   anIsoCurve = (IsUIso ?
    aSurface->UIso(theParameter) : aSurface->VIso(theParameter));
  Handle(Geom2d_Curve) aPIsoCurve =
    aHatcher.GetHatching(aHatchingIndex);
  const Standard_Real  aTol = Precision::Confusion();
  Standard_Integer     anIDom = 1;
  Standard_Real        aV1;
  Standard_Real        aV2;
  BRep_Builder         aBuilder;
  Standard_Integer     aNbEdges = 0;

  for (; anIDom <= aNbDomains; anIDom++) {
    if (aHatcher.GetDomain(aHatchingIndex, anIDom, aV1, aV2)) {
      // Check first and last parameters.
      if (!aHatcher.IsDomainInfinite(aHatchingIndex, anIDom)) {
        // Create an edge.
        TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(anIsoCurve, aV1, aV2);

        // Update it with a parametric curve on face.
        aBuilder.UpdateEdge(anEdge, aPIsoCurve, theFace, aTol);
        aNbEdges++;

        if (aNbEdges > 1) {
          // Result is a compond.
          if (aNbEdges == 2) {
            // Create a new compound.
            TopoDS_Compound aCompound;

            aBuilder.MakeCompound(aCompound);
            aBuilder.Add(aCompound, aResult);
            aResult = aCompound;
          }

          // Add an edge to the compound.
          aBuilder.Add(aResult, anEdge);
        } else {
          // Result is the edge.
          aResult = anEdge;
        }
      }
    }
  }

  if (aNbEdges == 0) {
    Standard_ConstructionError::Raise("MakeIsoline : Empty result");
  }

  return aResult;
}

//================================================================================
/*!
 * \brief Returns a name of creation operation and names and values of creation parameters
 */
//================================================================================

bool GEOMImpl_ShapeDriver::
GetCreationInformation(std::string&             theOperationName,
                       std::vector<GEOM_Param>& theParams)
{
  if (Label().IsNull()) return 0;
  Handle(GEOM_Function) function = GEOM_Function::GetFunction(Label());

  GEOMImpl_IShapes aCI( function );
  Standard_Integer aType = function->GetType();

  switch ( aType ) {
  case WIRE_EDGES:
    theOperationName = "WIRE";
    AddParam( theParams, "Wires/edges", aCI.GetShapes() );
    AddParam( theParams, "Tolerance", aCI.GetTolerance() );
    break;
  case FACE_WIRE:
    theOperationName = "FACE";
    AddParam( theParams, "Wire/edge", aCI.GetBase() );
    AddParam( theParams, "Is planar wanted", aCI.GetIsPlanar() );
    break;
  case FACE_WIRES:
    theOperationName = "FACE";
    AddParam( theParams, "Wires/edges", aCI.GetShapes() );
    AddParam( theParams, "Is planar wanted", aCI.GetIsPlanar() );
    break;
  case SHELL_FACES:
    theOperationName = "SHELL";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case SOLID_SHELLS:
    theOperationName = "SOLID";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case COMPOUND_SHAPES:
    theOperationName = "COMPOUND";
    AddParam( theParams, "Objects", aCI.GetShapes() );
    break;
  case EDGE_WIRE:
    theOperationName = "EDGE";
    AddParam( theParams, "Wire", aCI.GetBase() );
    AddParam( theParams, "Linear Tolerance", aCI.GetTolerance() );
    AddParam( theParams, "Angular Tolerance", aCI.GetAngularTolerance() );
    break;
  case EDGE_CURVE_LENGTH:
    theOperationName = "EDGE";
    {
      GEOMImpl_IVector aCI( function );
      AddParam( theParams, "Edge", aCI.GetPoint1() );
      AddParam( theParams, "Start point", aCI.GetPoint2() );
      AddParam( theParams, "Length", aCI.GetParameter() );
    }
    break;
  case SHAPES_ON_SHAPE:
  {
    theOperationName = "GetShapesOnShapeAsCompound";
    Handle(TColStd_HSequenceOfTransient) shapes = aCI.GetShapes();
    if ( !shapes.IsNull() && shapes->Length() > 0 )
      AddParam( theParams, "Check shape", shapes->Value(1) );
    if ( !shapes.IsNull() && shapes->Length() > 1 )
      AddParam( theParams, "Shape", shapes->Value(2) );
    AddParam( theParams, "Shape type", TopAbs_ShapeEnum( aCI.GetSubShapeType() ));
    AddParam( theParams, "State", TopAbs_State((int) aCI.GetTolerance() ));
    break;
  }
  case SHAPE_ISOLINE:
  {
    GEOMImpl_IIsoline aII (function);

    theOperationName = "ISOLINE";
    AddParam(theParams, "Face", aII.GetFace());
    AddParam(theParams, "Isoline type", (aII.GetIsUIso() ? "U" : "V"));
    AddParam(theParams, "Parameter", aII.GetParameter());
    break;
  }
  default:
    return false;
  }

  return true;
}

IMPLEMENT_STANDARD_HANDLE (GEOMImpl_ShapeDriver,GEOM_BaseDriver);
IMPLEMENT_STANDARD_RTTIEXT (GEOMImpl_ShapeDriver,GEOM_BaseDriver);

//modified by NIZNHY-PKV Wed Dec 28 13:48:31 2011f
#include <TopoDS_Iterator.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <ShapeAnalysis_FreeBounds.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_MapOfOrientedShape.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Wire.hxx>

//=======================================================================
//function : KeepEdgesOrder
//purpose  : 
//=======================================================================
/*
void KeepEdgesOrder(const Handle(TopTools_HSequenceOfShape)& aEdges,
                    const Handle(TopTools_HSequenceOfShape)& aWires)
{
  Standard_Integer aNbWires, aNbEdges;
  // 
  if (aEdges.IsNull()) {
    return;
  }
  //
  if (aWires.IsNull()) {
    return;
  }
  //
  aNbEdges=aEdges->Length();
  aNbWires=aWires->Length();
  if (!aNbEdges || !aNbWires) {
    return;
  }
  //-----
  Standard_Boolean bClosed;
  Standard_Integer i, j;
  TopoDS_Wire aWy;
  TopoDS_Iterator aIt;
  BRep_Builder aBB;
  TopTools_MapOfOrientedShape aMEx;
  //
  for (i=1; i<=aNbWires; ++i) {
    const TopoDS_Shape& aWx=aWires->Value(i);
    //
    aMEx.Clear();
    aIt.Initialize (aWx);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aEx=aIt.Value();
      aMEx.Add(aEx);
    }
    // aWy
    aBB.MakeWire (aWy);
    for (j=1; j<=aNbEdges; ++j) {
      const TopoDS_Shape& aE=aEdges->Value(j);
      if (aMEx.Contains(aE)) {
        aBB.Add(aWy, aE);
      }
    }
    //
    bClosed=aWx.Closed();
    aWy.Closed(bClosed);
    //
    aWires->Append(aWy);
  }// for (i=1; i<=aNbWires; ++i) {
  //
  aWires->Remove(1, aNbWires);
}
*/
//modified by NIZNHY-PKV Wed Dec 28 13:48:34 2011t
