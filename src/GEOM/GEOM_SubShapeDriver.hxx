//  File   : GEOMImpl_ShapeDriver.ixx
//  Module : GEOM

#ifndef _GEOM_SubShapeDriver_HeaderFile
#define _GEOM_SubShapeDriver_HeaderFile

#ifndef _TColStd_SequenceOfExtendedString_HeaderFile
#include <TColStd_SequenceOfExtendedString.hxx>
#endif
#ifndef _Standard_TypeMismatch_HeaderFile
#include <Standard_TypeMismatch.hxx>
#endif

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif
#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_GUID_HeaderFile
#include <Standard_GUID.hxx>
#endif 

#ifndef _Handle_TFunction_Driver_HeaderFile
#include <Handle_TFunction_Driver.hxx>
#endif

class Standard_Transient;
class Handle_Standard_Type;
class Handle(TFunction_Driver);
class GEOM_SubShapeDriver;

Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_SubShapeDriver);

class Handle(GEOM_SubShapeDriver) : public Handle(TFunction_Driver) {
  public:
    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }

    Handle(GEOM_SubShapeDriver)():Handle(TFunction_Driver)() {} 
    Handle(GEOM_SubShapeDriver)(const Handle(GEOM_SubShapeDriver)& aHandle) : Handle(TFunction_Driver)(aHandle) 
     {
     }

    Handle(GEOM_SubShapeDriver)(const GEOM_SubShapeDriver* anItem) : Handle(TFunction_Driver)((TFunction_Driver *)anItem) 
     {
     }

    Handle(GEOM_SubShapeDriver)& operator=(const Handle(GEOM_SubShapeDriver)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_SubShapeDriver)& operator=(const GEOM_SubShapeDriver* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_SubShapeDriver* operator->() 
     {
      return (GEOM_SubShapeDriver *)ControlAccess();
     }

    GEOM_SubShapeDriver* operator->() const 
     {
      return (GEOM_SubShapeDriver *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_SubShapeDriver)() {};
 
   Standard_EXPORT static const Handle(GEOM_SubShapeDriver) DownCast(const Handle(Standard_Transient)& AnObject);
};

#ifndef _TFunction_Driver_HeaderFile
#include <TFunction_Driver.hxx>
#endif
#ifndef _TFunction_Logbook_HeaderFile
#include <TFunction_Logbook.hxx>
#endif
#ifndef _Standard_CString_HeaderFile
#include <Standard_CString.hxx>
#endif

class TColStd_SequenceOfExtendedString;


class GEOM_SubShapeDriver : public TFunction_Driver {

public:

    inline void* operator new(size_t,void* anAddress) 
      {
        return anAddress;
      }
    inline void* operator new(size_t size) 
      { 
        return Standard::Allocate(size); 
      }
    inline void  operator delete(void *anAddress) 
      { 
        if (anAddress) Standard::Free((Standard_Address&)anAddress); 
      }

 // Methods PUBLIC
 // 
Standard_EXPORT GEOM_SubShapeDriver();
Standard_EXPORT virtual  Standard_Integer Execute(TFunction_Logbook& log) const; 
Standard_EXPORT virtual void Validate(TFunction_Logbook&) const {}
Standard_EXPORT Standard_Boolean MustExecute(const TFunction_Logbook&) const { return Standard_True; }
Standard_EXPORT static const Standard_GUID& GetID();
Standard_EXPORT ~GEOM_SubShapeDriver() {};


 // Type management
 //
Standard_EXPORT friend Handle_Standard_Type& GEOM_SubShapeDriver_Type_();
Standard_EXPORT const Handle(Standard_Type)& DynamicType() const  { return STANDARD_TYPE(GEOM_SubShapeDriver) ; }
Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)& AType) const { return (STANDARD_TYPE(GEOM_SubShapeDriver) == AType || TFunction_Driver::IsKind(AType)); } 


};

#endif