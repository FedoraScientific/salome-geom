#ifndef _GEOM_Object_HeaderFile
#define _GEOM_Object_HeaderFile

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
#ifndef _Handle_MMgt_TShared_HeaderFile
#include <Handle_MMgt_TShared.hxx>
#endif  
#ifndef _MMgt_TShared_HeaderFile
#include <MMgt_TShared.hxx>
#endif  
#ifndef _Standard_GUID_HeaderFile
#include <Standard_GUID.hxx>
#endif
#ifndef _TopoDS_Shape_HeaderFile
#include <TopoDS_Shape.hxx>
#endif
#ifndef _TDF_Label_HeaderFile
#include <TDF_Label.hxx>
#endif
#ifndef _TColStd_HSequenceOfTransient_HeaderFile
#include <TColStd_HSequenceOfTransient.hxx>
#endif
#ifndef _TCollection_AsciiString_HeaderFile
#include <TCollection_AsciiString.hxx>
#endif

class Handle_TColStd_HSequenceOfTransient;
class Standard_Transient;
class Handle_Standard_Type;
class Handle(MMgt_TShared);
class GEOM_Object;


Standard_EXPORT Handle_Standard_Type& STANDARD_TYPE(GEOM_Object);

class Handle(GEOM_Object) : public Handle(MMgt_TShared) {
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

    Handle(GEOM_Object)():Handle(MMgt_TShared)() {} 
    Handle(GEOM_Object)(const Handle(GEOM_Object)& aHandle) : Handle(MMgt_TShared)(aHandle) 
     {
     }

    Handle(GEOM_Object)(const GEOM_Object* anItem) : Handle(MMgt_TShared)((MMgt_TShared *)anItem) 
     {
     }

    Handle(GEOM_Object)& operator=(const Handle(GEOM_Object)& aHandle)
     {
      Assign(aHandle.Access());
      return *this;
     }

    Handle(GEOM_Object)& operator=(const GEOM_Object* anItem)
     {
      Assign((Standard_Transient *)anItem);
      return *this;
     }

    GEOM_Object* operator->() 
     {
      return (GEOM_Object *)ControlAccess();
     }

    GEOM_Object* operator->() const 
     {
      return (GEOM_Object *)ControlAccess();
     }

   Standard_EXPORT ~Handle(GEOM_Object)() {};
 
   Standard_EXPORT static const Handle(GEOM_Object) DownCast(const Handle(Standard_Transient)& AnObject);
};



#include <Standard_GUID.hxx>
#include <TDataStd_TreeNode.hxx>
#include "GEOM_Function.hxx"
#include "GEOM_Engine.hxx"


class GEOM_Object : public MMgt_TShared
{
 friend class GEOM_Engine;     

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
  
  // Type management
  //
  Standard_EXPORT friend Handle_Standard_Type& GEOM_Object_Type_();
  Standard_EXPORT const Handle(Standard_Type)& DynamicType() const  { return STANDARD_TYPE(GEOM_Object) ; }
  Standard_EXPORT Standard_Boolean IsKind(const Handle(Standard_Type)& AType) const { return (STANDARD_TYPE(GEOM_Object) == AType || MMgt_TShared::IsKind(AType)); } 

 private:
   GEOM_Object(TDF_Label& theLabel);  

 public:
  GEOM_Object(TDF_Label& theEntry, int theType);
  ~GEOM_Object() {;}

  //Finds a GEOM_Object on the label theLabel
  static Handle(GEOM_Object) GetObject(TDF_Label& theLabel);

  //Finds a GEOM_Object by a reference, stored on the label theLabel
  static Handle(GEOM_Object) GetReferencedObject(TDF_Label& theLabel);

  //Returns a GEOM_Object common GUID
  static const Standard_GUID& GetObjectID();

  //Returns a GUID associated with a sub shape object
  static const Standard_GUID& GetSubShapeID();

  //###########################################################
  //Access to properties 
  //###########################################################

  //Returns a TreeNode that presents a root of a function tree for this GEOM_Object
  Handle(TDataStd_TreeNode) GetRootNode() { return _root; } 
  
  //Returns a label of this GEOM_Object
  TDF_Label GetEntry() { return _label; }
  
  //Returns a type of this GEOM_Object (GEOM_POINT, GEOM_VECTOR...)
  int GetType();      

  //Sets the type of this GEOM_Object
  void SetType(int theType);
  
  //Returns an ID of the OCAF document where this GEOM_Object is stored
  int GetDocID(); 

  //Returns a value (as TopoDS_Shape) of this GEOM_Object 
  TopoDS_Shape GetValue();

  //Sets a name of this GEOM_Object
  void SetName(const char* theName);

  //Returns a name of this GEOM_Object
  char* GetName();

  //Sets an auxiliary data
  void SetAuxData(const char* theData);

  //Returns an auxiliary data
  char* GetAuxData();

  //###########################################################  
  // Sub shape methods
  //###########################################################
  
  //Returns false if the object is a sub shape of another object
  bool IsMainShape();

 //###########################################################  
 // CORBA related methods
 //###########################################################
 
 //Sets an IOR of CORBA GEOM_Object_i which refers to this object
 void SetIOR(TCollection_AsciiString& theIOR) { _ior = theIOR; }

 //Returns an IOR of CORBA GEOM_Object_i which refers to this object
 TCollection_AsciiString GetIOR() { return _ior; }

  //###########################################################
  //Functions methods
  //###########################################################

  //Adds a function with a driver GUID = theGUID and a type theFunctionType to the function tree of this GEOM_Object
  Handle(GEOM_Function) AddFunction(const Standard_GUID& theGUID, int theFunctionType);
  
  //Returns a number of functions of this GEOM_Object
  int GetNbFunctions(); 
  
  //Returns a function with given number theFunctionNumber
  Handle(GEOM_Function) GetFunction(int theFunctionNumber);
  
  //Return the last function of this GEOM_Object
  Handle(GEOM_Function) GetLastFunction();

  //Returns all dependencies of the object
  Handle(TColStd_HSequenceOfTransient) GetAllDependency();

  //Returns the dependencies of the last function
  Handle(TColStd_HSequenceOfTransient) GetLastDependency();

 //###########################################################  
 // Internal methods
 //###########################################################

 //Returns a label which could be used to store some additional data
 TDF_Label GetFreeLabel();
 

 private:

  Handle(TDataStd_TreeNode) _root;
  TDF_Label    _label;
  TCollection_AsciiString _ior;
};

#endif