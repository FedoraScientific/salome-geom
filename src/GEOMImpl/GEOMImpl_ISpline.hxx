//NOTE: This is an intreface to a function for the Spline creation.


#include "GEOM_Function.hxx"

#define SPL_ARG_LENG 1
#define SPL_ARG_LAST 1

class GEOMImpl_ISpline
{
 public:

  GEOMImpl_ISpline(Handle(GEOM_Function) theFunction): _func(theFunction) {}

  void SetLength(int theLen) { _func->SetInteger(SPL_ARG_LENG, theLen); }

  void SetPoint(int theId, Handle(GEOM_Function) theP) { _func->SetReference(SPL_ARG_LAST + theId, theP); }

  int GetLength() { return _func->GetInteger(SPL_ARG_LENG); }

  Handle(GEOM_Function) GetPoint(int theId) { return _func->GetReference(SPL_ARG_LAST + theId); }

 private:

  Handle(GEOM_Function) _func;
};