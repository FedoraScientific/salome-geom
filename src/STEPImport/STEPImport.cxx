// File:	STEPImport.cxx
// Created:	Wed May 19 14:41:10 2004
// Author:	Pavel TELKOV
//		<ptv@mutex.nnov.opencascade.com>

#include "utilities.h"

#include <BRep_Builder.hxx>

#include <IFSelect_ReturnStatus.hxx>

#include <STEPControl_Reader.hxx>

#include <TCollection_AsciiString.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC

//=============================================================================
/*!
 *  Import()
 */
//=============================================================================

extern "C"
{
#ifdef WNT
  __declspec(__dllexport)
#endif
  TopoDS_Shape Import (const TCollection_AsciiString& theFileName,
                       TCollection_AsciiString&       theError)
  {
    MESSAGE("Import STEP model from file " << theFileName.ToCString());
    TopoDS_Shape aResShape;
    //VRV: OCC 4.0 migration
    STEPControl_Reader aReader;
    //VRV: OCC 4.0 migration
    TopoDS_Compound compound;
    BRep_Builder B;
    B.MakeCompound( compound );
    try {
      IFSelect_ReturnStatus status = aReader.ReadFile(theFileName.ToCString());

      if (status == IFSelect_RetDone) {
	Standard_Boolean failsonly = Standard_False ;
	aReader.PrintCheckLoad (failsonly, IFSelect_ItemsByEntity);
	/* Root transfers */
	Standard_Integer nbr = aReader.NbRootsForTransfer();
	aReader.PrintCheckTransfer (failsonly, IFSelect_ItemsByEntity);

	for ( Standard_Integer n=1; n <= nbr; n++) {
	  Standard_Boolean ok = aReader.TransferRoot(n);
	  /* Collecting resulting entities */
	  Standard_Integer nbs = aReader.NbShapes();
	  if (!ok || nbs == 0)
	  {
	    // THROW_SALOME_CORBA_EXCEPTION("Exception catched in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM);
	    continue; // skip empty root
	  }
	  /* For a single entity */
	  else if (nbr == 1 && nbs == 1) {
	    aResShape = aReader.Shape(1);
	    break;
	  }

	  for ( Standard_Integer i=1; i<=nbs; i++ ) {
	    TopoDS_Shape aShape = aReader.Shape(i);
	    if ( aShape.IsNull() ) {
	      // THROW_SALOME_CORBA_EXCEPTION("Null shape in GEOM_Gen_i::ImportStep", SALOME::BAD_PARAM) ;
	      //return aResShape;
	      continue;
	    }
	    else {
	      B.Add( compound, aShape ) ;
	    }
	  }
	}
	if ( aResShape.IsNull() )
	  aResShape = compound;

      } else {
//        switch (status) {
//        case IFSelect_RetVoid:
//          theError = "Nothing created or No data to process";
//          break;
//        case IFSelect_RetError:
//          theError = "Error in command or input data";
//          break;
//        case IFSelect_RetFail:
//          theError = "Execution was run, but has failed";
//          break;
//        case IFSelect_RetStop:
//          theError = "Execution has been stopped. Quite possible, an exception was raised";
//          break;
//        default:
//          break;
//        }
        theError = "Wrong format of the imported file. Can't import file.";
	aResShape.Nullify();
      }
    }
    catch (Standard_Failure) {
      Handle(Standard_Failure) aFail = Standard_Failure::Caught();
      theError = aFail->GetMessageString();
      aResShape.Nullify();
    }
    return aResShape;
  }
}