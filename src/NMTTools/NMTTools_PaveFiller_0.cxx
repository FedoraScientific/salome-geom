// File:	NMTTools_PaveFiller_0.cxx
// Created:	Mon Dec  8 11:45:51 2003
// Author:	Peter KURNEV
//		<pkv@irinox>


#include <NMTTools_PaveFiller.ixx>

#include <TopAbs_ShapeEnum.hxx>
#include <BOPTools_ListOfCoupleOfInteger.hxx>
#include <NMTDS_ShapesDataStructure.hxx>
#include <BooleanOperations_OnceExplorer.hxx>

//=======================================================================
//function : SortTypes
//purpose  : 
//=======================================================================
  void NMTTools_PaveFiller::SortTypes(Standard_Integer& theWhat,
				      Standard_Integer& theWith)const 
{ 
  Standard_Boolean aReverseFlag=Standard_True;

  TopAbs_ShapeEnum aType1= myDS->GetShapeType(theWhat),
                   aType2= myDS->GetShapeType(theWith);
  
  if (aType1==aType2)
    return;
  
  if (aType1==TopAbs_EDGE && aType2==TopAbs_FACE){
    aReverseFlag=Standard_False;
  }

  if (aType1==TopAbs_VERTEX && 
      (aType2==TopAbs_FACE || aType2==TopAbs_EDGE)) {
    aReverseFlag=Standard_False;
  }
  
  Standard_Integer aWhat, aWith;
  aWhat=(aReverseFlag) ? theWith : theWhat;
  aWith=(aReverseFlag) ? theWhat : theWith;
  
  theWhat=aWhat;
  theWith=aWith;
}
//=======================================================================
// function:  ExpectedPoolLength
// purpose: 
//=======================================================================
  Standard_Integer NMTTools_PaveFiller::ExpectedPoolLength()const
{
  Standard_Integer aNbIIs;
  Standard_Real aCfPredict=.5;

  const BOPTools_ListOfCoupleOfInteger& aLC=myDSIt.ListOfCouple();
  aNbIIs=aLC.Extent();
  //
  if (aNbIIs==1) {
    return aNbIIs;
  }
  //
  aNbIIs=(Standard_Integer) (aCfPredict*(Standard_Real)aNbIIs);
  
  return aNbIIs;
}
//=======================================================================
// function:IsSuccesstorsComputed
// purpose: 
//=======================================================================
  Standard_Boolean NMTTools_PaveFiller::IsSuccesstorsComputed(const Standard_Integer aN1,
							      const Standard_Integer aN2)const
{
  Standard_Integer nSuc, n1, n2;

  BooleanOperations_OnceExplorer aExp(*myDS);
  TopAbs_ShapeEnum aType=myDS->GetShapeType(aN1);

  n1=aN1;
  n2=aN2;

  if (aType!=TopAbs_VERTEX) {
    Standard_Integer ntmp=n1;
    n1=n2;
    n2=ntmp;
  }

  aType=myDS->GetShapeType(n2);
  if (aType==TopAbs_EDGE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
  return Standard_False;
  }

  else if (aType==TopAbs_FACE) {
    aExp.Init(n2, TopAbs_VERTEX);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }

    aExp.Init(n2, TopAbs_EDGE);
    for (; aExp.More(); aExp.Next()) {
      nSuc=aExp.Current();
      if (myIntrPool->IsComputed(n1, nSuc)) {
	return Standard_True;
      }
    }
    return Standard_False;
  }

  return Standard_False;
}