
#ifndef _GEOM_ITransformOperations_i_HeaderFile
#define _GEOM_ITransformOperations_i_HeaderFile


#include <SALOMEconfig.h>

#include CORBA_SERVER_HEADER(GEOM_Gen)
#include "GEOM_IOperations_i.hh"
#include "GEOM_Object_i.hh"

#include "GEOMImpl_ITransformOperations.hxx"

class GEOM_ITransformOperations_i :
    public virtual POA_GEOM::GEOM_ITransformOperations,
    public virtual GEOM_IOperations_i
{
 public:
  GEOM_ITransformOperations_i (PortableServer::POA_ptr thePOA,
			       GEOM::GEOM_Gen_ptr theEngine,
			       ::GEOMImpl_ITransformOperations* theImpl);
  ~GEOM_ITransformOperations_i();

  GEOM::GEOM_Object_ptr TranslateTwoPoints (GEOM::GEOM_Object_ptr theObject,
					    GEOM::GEOM_Object_ptr thePoint1,
					    GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr TranslateTwoPointsCopy (GEOM::GEOM_Object_ptr theObject,
						GEOM::GEOM_Object_ptr thePoint1,
						GEOM::GEOM_Object_ptr thePoint2);

  GEOM::GEOM_Object_ptr TranslateDXDYDZ (GEOM::GEOM_Object_ptr theObject, CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ);


  GEOM::GEOM_Object_ptr TranslateDXDYDZCopy (GEOM::GEOM_Object_ptr theObject,  CORBA::Double theDX, CORBA::Double theDY, CORBA::Double theDZ);

  GEOM::GEOM_Object_ptr TranslateVector (GEOM::GEOM_Object_ptr theObject,
					 GEOM::GEOM_Object_ptr theVector);

  GEOM::GEOM_Object_ptr TranslateVectorCopy (GEOM::GEOM_Object_ptr theObject,
					     GEOM::GEOM_Object_ptr theVector);

  GEOM::GEOM_Object_ptr MultiTranslate1D (GEOM::GEOM_Object_ptr theObject, GEOM::GEOM_Object_ptr theVector, CORBA::Double theStep, CORBA::Long theNbTimes);

  GEOM::GEOM_Object_ptr MultiTranslate2D (GEOM::GEOM_Object_ptr theObject, GEOM::GEOM_Object_ptr theVector1, CORBA::Double theStep1, CORBA::Long theNbTimes1,
				                       GEOM::GEOM_Object_ptr theVector2, CORBA::Double theStep2, CORBA::Long theNbTimes2);

  GEOM::GEOM_Object_ptr Rotate (GEOM::GEOM_Object_ptr theObject,
                                GEOM::GEOM_Object_ptr theAxis,
				CORBA::Double theAngle);

  GEOM::GEOM_Object_ptr MultiRotate1D (GEOM::GEOM_Object_ptr theObject,
				       GEOM::GEOM_Object_ptr theVector,
				       CORBA::Long theNbTimes);

 
  GEOM::GEOM_Object_ptr MultiRotate2D (GEOM::GEOM_Object_ptr theObject,
				       GEOM::GEOM_Object_ptr theVector, 
				       CORBA::Double theAngle, 
				       CORBA::Long theNbTimes1, 
				       CORBA::Double theStep, 
				       CORBA::Long theNbTimes2);
					   
  GEOM::GEOM_Object_ptr RotateCopy (GEOM::GEOM_Object_ptr theObject,
				    GEOM::GEOM_Object_ptr theAxis,
				    CORBA::Double theAngle);  

  GEOM::GEOM_Object_ptr MirrorPlane (GEOM::GEOM_Object_ptr theObject,
				     GEOM::GEOM_Object_ptr thePlane);

  GEOM::GEOM_Object_ptr MirrorPlaneCopy (GEOM::GEOM_Object_ptr theObject,
					 GEOM::GEOM_Object_ptr thePlane);

  GEOM::GEOM_Object_ptr MirrorAxis (GEOM::GEOM_Object_ptr theObject,
				    GEOM::GEOM_Object_ptr theAxis);

  GEOM::GEOM_Object_ptr MirrorAxisCopy (GEOM::GEOM_Object_ptr theObject,
					GEOM::GEOM_Object_ptr theAxis);

  GEOM::GEOM_Object_ptr MirrorPoint (GEOM::GEOM_Object_ptr theObject,
				     GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr MirrorPointCopy (GEOM::GEOM_Object_ptr theObject,
					 GEOM::GEOM_Object_ptr thePoint);

  GEOM::GEOM_Object_ptr OffsetShape (GEOM::GEOM_Object_ptr theObject,
				     CORBA::Double theOffset);

  GEOM::GEOM_Object_ptr OffsetShapeCopy (GEOM::GEOM_Object_ptr theObject,
					 CORBA::Double theOffset);

  GEOM::GEOM_Object_ptr ScaleShape (GEOM::GEOM_Object_ptr theObject,
				    GEOM::GEOM_Object_ptr thePoint,
				    CORBA::Double theFactor);

  GEOM::GEOM_Object_ptr ScaleShapeCopy (GEOM::GEOM_Object_ptr theObject,
					GEOM::GEOM_Object_ptr thePoint,
					CORBA::Double theFactor);

  GEOM::GEOM_Object_ptr PositionShape (GEOM::GEOM_Object_ptr theObject,
				       GEOM::GEOM_Object_ptr theStartLCS,
				       GEOM::GEOM_Object_ptr theEndLCS);

  GEOM::GEOM_Object_ptr PositionShapeCopy (GEOM::GEOM_Object_ptr theObject,
					   GEOM::GEOM_Object_ptr theStartLCS,
					   GEOM::GEOM_Object_ptr theEndLCS);

  ::GEOMImpl_ITransformOperations* GetOperations() { return (::GEOMImpl_ITransformOperations*)GetImpl(); }
};

#endif