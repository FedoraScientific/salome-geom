// File generated by CPPExt (Value)
//
//                     Copyright (C) 1991 - 2000 by  
//                      Matra Datavision SA.  All rights reserved.
//  
//                     Copyright (C) 2001 - 2004 by
//                     Open CASCADE SA.  All rights reserved.
// 
// This file is part of the Open CASCADE Technology software.
//
// This software may be distributed and/or modified under the terms and
// conditions of the Open CASCADE Public License as defined by Open CASCADE SA
// and appearing in the file LICENSE included in the packaging of this file.
//  
// This software is distributed on an "AS IS" basis, without warranty of any
// kind, and Open CASCADE SA hereby disclaims all such warranties,
// including without limitation, any warranties of merchantability, fitness
// for a particular purpose or non-infringement. Please see the License for
// the specific terms and conditions governing rights and limitations under the
// License.

#ifndef _GEOMAlgo_Tools_HeaderFile
#define _GEOMAlgo_Tools_HeaderFile

#ifndef _Standard_Integer_HeaderFile
#include <Standard_Integer.hxx>
#endif
#ifndef _Standard_Real_HeaderFile
#include <Standard_Real.hxx>
#endif
#ifndef _Standard_Boolean_HeaderFile
#include <Standard_Boolean.hxx>
#endif
#ifndef _Handle_Geom_Surface_HeaderFile
#include <Handle_Geom_Surface.hxx>
#endif
class GEOMAlgo_IndexedDataMapOfPassKeyListOfShape;
class IntTools_Context;
class TopTools_ListOfShape;
class TopTools_IndexedDataMapOfShapeListOfShape;
class TopoDS_Shape;
class gp_Pnt;
class TopoDS_Edge;
class TopoDS_Face;
class Geom_Surface;


#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

class GEOMAlgo_Tools  {

public:

    void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }
 // Methods PUBLIC
 // 
Standard_EXPORT static  Standard_Integer RefineSDShapes(GEOMAlgo_IndexedDataMapOfPassKeyListOfShape& aMSD,const Standard_Real aTol,IntTools_Context& aCtx) ;
Standard_EXPORT static  Standard_Integer FindSDShapes(const TopTools_ListOfShape& aLE,const Standard_Real aTol,TopTools_IndexedDataMapOfShapeListOfShape& aMEE,IntTools_Context& aCtx) ;
Standard_EXPORT static  Standard_Integer FindSDShapes(const TopoDS_Shape& aE1,const TopTools_ListOfShape& aLE,const Standard_Real aTol,TopTools_ListOfShape& aLESD,IntTools_Context& aCtx) ;
Standard_EXPORT static  Standard_Boolean ProjectPointOnShape(const gp_Pnt& aP1,const TopoDS_Shape& aS,gp_Pnt& aP2,IntTools_Context& aCtx) ;
Standard_EXPORT static  void PointOnShape(const TopoDS_Shape& aS,gp_Pnt& aP3D) ;
Standard_EXPORT static  void PointOnEdge(const TopoDS_Edge& aE,gp_Pnt& aP3D) ;
Standard_EXPORT static  void PointOnEdge(const TopoDS_Edge& aE,const Standard_Real aT,gp_Pnt& aP3D) ;
Standard_EXPORT static  void PointOnFace(const TopoDS_Face& aF,gp_Pnt& aP3D) ;
Standard_EXPORT static  void PointOnFace(const TopoDS_Face& aF,const Standard_Real aU,const Standard_Real aV,gp_Pnt& aP3D) ;
Standard_EXPORT static  void RefinePCurveForEdgeOnFace(const TopoDS_Edge& aE,const TopoDS_Face& aF,const Standard_Real aU1,const Standard_Real aU2) ;
Standard_EXPORT static  Standard_Boolean IsUPeriodic(const Handle(Geom_Surface)& aS) ;





protected:

 // Methods PROTECTED
 // 


 // Fields PROTECTED
 //


private: 

 // Methods PRIVATE
 // 


 // Fields PRIVATE
 //


};





// other Inline functions and methods (like "C++: function call" methods)
//


#endif