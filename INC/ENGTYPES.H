#ifndef __ENGTYPES_H
#define __ENGTYPES_H

/***************************************************************************
    $Workfile:   engtypes.h  $
    $Revision:   1.83.2.0  $
        $Date:   12 Feb 1996 11:40:46  $
      $Author:   franke  $
    Contains:      Common type definitions and defines for SalesBUILDER APIs.
    Last Modified: $Modtime:   04 Jan 1996 16:39:00  $

  *************************************************************************
    Copyright (C) 1991,1992,1993 Trilogy Development Group, Inc.
    Trilogy Development Group, Inc., Confidential and Proprietary.
    All rights reserved.  This software is subject to copyright protection
    under the laws of the United States and other countries.
  *************************************************************************
    Comments:

***************************************************************************/

#include <idmtypes.h>

/*Now defined in idmtypes.h*/
/*API return values*/
/*#define SB_FAILURE 0*/
/*#define SB_SUCCESS 1*/
/*#define SBERR_UNKNOWNPROCESSID      1200*/
/*#define SBERR_UNKNOWNCONTEXTID      1201*/


/*Database Defines */
#define SBDB_FLAT			1
#define SBDB_SYBASE			2
#define SBDB_ORACLE			3
#define SBDB_ALLBASE		4

/*Error Defines*/
#define SBERR_INVALID               1000
#define SBERR_NOMEM                 1001
#define SBERR_NULLPOINTER           1002
#define SBERR_INVALIDTYPE           1003
#define SBERR_INVALIDITERATOR       1004
#define SBERR_INVALIDITERATORTYPE   1005
#define SBERR_NAMENOTFOUND          1006
#define SBERR_NOTINPRODUCTLINE      1007
#define SBERR_FILEERROR             1008
#define SBERR_ZEROLENGTHBUFFER      1009
#define SBERR_DBERROR               1010
#define SBERR_NOTTOPLEVEL           1011
#define SBERR_PRODUCTBASEMISMATCH   1012
#define SBERR_SAVEERROR				1013
#define SBERR_NOPARSER              1014
#define SBERR_IDNOTINPB             1015
#define SBERR_INSTNOTINSYSTEM       1016
#define SBERR_APPLICATIONABORTED    1017

#define SBERR_NOCLIENT              1100
#define SBERR_RPCERROR              1101

#define SBERR_MODELERROR            2000
#define SBERR_CIRCULARDEPENDS       2001
#define SBERR_EXTANTSYSTEMS         2002
#define SBERR_UNKNOWNREQUESTGROUP   2003
#define SBERR_DIVIDEBYZERO          2004
#define SBERR_BADDATE               2005

#define SBERR_INCOMPLETE            3000
#define SBERR_NOLICENSE             3100

/*Instance types*/
#define SBTYPE_INSTANCE             5000
#define SBTYPE_COMPONENTINSTANCE    5001
#define SBTYPE_CONNECTORINSTANCE    5002
#define SBTYPE_CONTAINERINSTANCE    5003
#define SBTYPE_COMPOSITEINSTANCE    5004
#define SBTYPE_PORTINSTANCE         5005
#define SBTYPE_RESOURCEINSTANCE     5006
#define SBTYPE_RESOURCEPOOLINSTANCE 5007
#define SBTYPE_PRODUCTINSTANCE      5008
#define SBTYPE_PRODUCTOPTIONINSTANCE 5009
#define SBTYPE_RESOURCEALLOCATORINSTANCE 5010
#define SBTYPE_SYSTEMINSTANCE       5011
#define SBTYPE_PACKAGEINSTANCE      5012

/* ID Types */
#define SBIDTYPE_PRODUCTBASE        5200
#define SBIDTYPE_PRODUCTLINE        5210
#define SBIDTYPE_ATTRIBUTE          5220
#define SBIDTYPE_ATTRIBUTEVALUE     5221
#define SBIDTYPE_DATATYPE           5230
#define SBIDTYPE_RESOURCE           5240
#define SBIDTYPE_COMPONENTCLASS     5250
#define SBIDTYPE_CLASSRELATION      5251
#define SBIDTYPE_COMPONENT          5260
#define SBIDTYPE_BREAKPOINT         5270
#define SBIDTYPE_PCMBREAKPOINT      5275
#define SBIDTYPE_CONSTRAINT         5280
#define SBIDTYPE_ABSTRACTCONSTRAINT 5281
#define SBIDTYPE_CONSTRAINTAPPLICATION 5282
#define SBIDTYPE_CONDITION          5290

#define SBIDTYPE_SYSTEM             5300
#define SBIDTYPE_REQUEST            5310
#define SBIDTYPE_REQUESTGROUP       5311
/* #define SBIDTYPE_INITIALIZATIONLIST 5320 */
#define SBIDTYPE_INSTANCE           5330

#define SBIDTYPE_PRODCOMPMAP           5400
#define SBIDTYPE_PRODUCT               5410
#define SBIDTYPE_PRODUCTOPTION         5411
#define SBIDTYPE_PRODUCTOPTIONGROUP    5412
#define SBIDTYPE_PARTNUMBER            5413
#define SBIDTYPE_PRODUCTATTRIBUTE      5414
#define SBIDTYPE_PRODCOMPMAPINSTANCE   5420
#define SBIDTYPE_PRODUCTINSTANCE       5430
#define SBIDTYPE_PRODUCTINSTANCEOPTION 5431
#define SBIDTYPE_PCMCOMPONENTINSTANCE  5432

#define SBIDTYPE_DEFAULTSET         5500
#define SBIDTYPE_DEFAULTKEYWORD     5510
#define SBIDTYPE_DEFAULTVALUE       5520

#define SBIDTYPE_ITERATOR           5600
#define SBIDTYPE_OBJ                5610

/* Job types */
#define JOB_CONFIGURE               6000
#define JOB_LOADPB                  6001
#define JOB_LOADSYS                 6002
#define JOB_BUNDLE                  6003
#define JOB_LOADPCM                 6004
#define JOB_SAVEPB                  6005



/*Basic "Types"*/

/* Resource Allocation Types: */
/* optionalResAlloc - The resource request is satisfied via a "greedy" algorithm.      */
/* 		The largest allocation unit less than or equal to the remaining request        */
/*      amount is used.  Allocation units greater than the remaining request are       */
/*      not used.  If the request cannot be completely satisfied, the condition        */
/*      is not reported (use maxResAlloc to report insufficient allocation).           */
/* maxResAlloc - The resource request is satisfied via a "greedy" algorithm.           */
/* 		The largest allocation unit less than or equal to the remaining request        */
/*      amount is used.  Allocation units greater than the remaining request are       */
/*      not used.  If the request cannot be completely satisfied, the condition        */
/*      is reported (use optionalResAlloc to avoid reporting insufficient allocation). */
/* minResAlloc - The resource request is satisfied by selecting the allocation         */
/*      unit nearest the remaining request size.  This allows allocation units         */
/*      greater than the remaining request size to be used.  If the request cannot     */
/*      be completely satisfied, the condition is reported.                            */
/* optionalExistingResAlloc - Existing resources are considered when attempting        */
/*      to satisfy the request.  If existing resources are insufficient, then new      */
/*      resource is allocated via the optionalResAlloc strategy.                       */
/* maxExistingResAlloc - Existing resources are considered when attempting             */
/*      to satisfy the request.  If existing resources are insufficient, then new      */
/*      resource is allocated via the maxResAlloc strategy.                            */
/* minExistingResAlloc - Existing resources are considered when attempting             */
/*      to satisfy the request.  If existing resources are insufficient, then new      */
/*      resource is allocated via the maxResAlloc strategy.                            */

typedef enum {
	optionalResAlloc,
	maxResAlloc,
	minResAlloc,
	optionalExistingResAlloc,
	maxExistingResAlloc,
	minExistingResAlloc,
	modelDefinedResAlloc
} ResourceAllocationType;

/* Visual Basic Equivalents
#define SB_OPTIONALRESALLOC 0
#define SB_MAXRESALLOC 1
#define SB_MINRESALLOC 2
#define SB_OPTIONALEXISTINGRESALLOC 3
#define SB_MAXEXISTINGRESALLOC 4
#define SB_MINEXISTINGRESALLOC 5
#define SB_MODELDEFINEDRESALLOC 6
*/

/* Trace option types:  */

typedef enum {
	basicTrace,          /* Minimum trace */
	bundlingTrace,       /* Trace bundling */
	connectionTrace,     /* Trace connection processing */
	dependencyTrace,     /* Trace dependency processing */
	implementingOnly,    /* Trace plan implementation only */
	implementingTrace,   /* Trace plan implementation */
	lookaheadTrace,      /* Trace lookahead */
	prioritizationTrace, /* Trace prioritization of all alternatives, all alternatives */
	requestQueueTrace,   /* Trace request queue status before and after configure */
	resourceTrace,       /* Trace resource goal processing */
	fullTrace            /* Maximum trace */
} EngineTraceType;

/* Visual Basic Equivalents
#define SB_BASICTRACE 0
#define SB_BUNDLINGTRACE 1
#define SB_CONNECTIONTRACE 2
#define SB_DEPENDENCYTRACE 3
#define SB_IMPLEMENTINGONLY 4
#define SB_IMPLEMENTINGTRACE 5
#define SB_LOOKAHEADTRACE 6
#define SB_PRIORITIZATIONTRACE 7
#define SB_REQUESTQUEUETRACE 8
#define SB_RESOURCETRACE 9
#define SB_FULLTRACE 10
*/

typedef enum  {
	resourcePoolStatic,
	resourcePoolDynamic,
	resourceQtyAllocator
} resourceQtyType;

/* Visual Basic Equivalents
#define SB_RESOURCEPOOLSTATIC 0
#define SB_RESOURCEPOOLDYNAMIC 1
#define SB_RESOURCEQTYALLOCATOR 2
*/

typedef enum {
	FILL_LEFTRIGHT,
	FILL_RIGHTLEFT,
	FILL_TOPBOTTOM,
	FILL_BOTTOMTOP
} fillDirectionType;

/* Visual Basic Equivalents
#define SB_FILL_LEFTRIGHT 0
#define SB_FILL_RIGHTLEFT 1
#define SB_FILL_TOPBOTTOM 2
#define SB_FILL_BOTTOMTOP 3
*/

typedef enum {
	DISPSTAT_UNKNOWN,
	DISPSTAT_DRAWN,
	DISPSTAT_LISTED,
	DISPSTAT_HIDDEN
} displayStatusType;

/* Visual Basic Equivalents
#define SB_DISPSTAT_UNKNOWN 0
#define SB_DISPSTAT_DRAWN 1
#define SB_DISPSTAT_LISTED 2
#define SB_DISPSTAT_HIDDEN 3
*/

typedef enum {
	CONNORIG_UNKNOWN,
	CONNORIG_TRUE,
	CONNORIG_FALSE
} connectionOriginType;

/* Visual Basic Equivalents
#define SB_CONNORIG_UNKNOWN 0
#define SB_CONNORIG_TRUE 1
#define SB_CONNORIG_FALSE 2
*/

typedef enum {
	resourceAvailable,
	resourceConsumed,
	resourceContiguous,
	resourceQuantity,
	resourceRequested,
	resourceShared,
	resourceAllocationComplete,
	resourceAttrModelDefined,
	resourceType,
	resourceTypeRequested
} resourceAttrType;

/* Visual Basic Equivalents
#define	SB_RESOURCEAVAILABLE 0
#define	SB_RESOURCECONSUMED 1
#define	SB_RESOURCECONTIGUOUS 2
#define	SB_RESOURCEQUANTITY 3
#define	SB_RESOURCEREQUESTED 4
#define	SB_RESOURCESHARED 5
#define	SB_RESOURCEALLOCATIONCOMPLETE 6
#define	SB_RESOURCEATTRMODELDEFINED 7
#define	SB_RESOURCETYPE 8
#define	SB_RESOURCETYPEREQUESTED 9
*/

typedef enum {
	useDefaultFailurePrefix,
	fastInstanceSets,
	considerDuplicates,
	preinstantiation
} OptionType;

/* Visual Basic Equivalents
#define SB_USEDEFAULTFAILUREPREFIX 0
#define SB_FASTINSTANCESETS 1
#define SB_CONSIDERDUPLICATES 2
#define SB_PREINSTANTIATION 3
*/

typedef ResourceAllocationType ResAllocType;
typedef EngineTraceType TraceType;


/* Constants for SelectionSet filtering options */
#define SB_SS_MAKEACTIVESELECTED      256
#define SB_SS_MAKEINACTIVEUNSELECTED  512


#if defined(__cplusplus) && !defined(MANGLE)
extern "C" {
#endif


#ifndef SB_STRONGTYPECHECK

typedef long IteratorID ;
typedef long ObjID ;

typedef long ObjectID;
typedef long ObjectIteratorID;

typedef long AttributeID;
typedef long AttributeValueID;
typedef long ComponentID;
typedef long ComponentClassID;
typedef long DataTypeID;
typedef long InitializationListID;
typedef long InstanceID;
typedef long ProdCompMapID;
typedef long ProdCompMapInstanceID;
typedef long ProductBaseID;
typedef long ProductInstanceID;
typedef long ProductInstanceOptionID;
typedef long ProductLineID;
typedef long RequestID;
typedef long RequestGroupID;
typedef long ResourceID;
typedef long SelectionSetID;
typedef long SystemID;
typedef long PartNumberID;
typedef long ProductID;
typedef long ProductOptionID;
typedef long ProductOptionGroupID;
typedef long DefaultKeywordID;
typedef long DefaultSetID;
typedef long DefaultValueID;
typedef long BundlerID;

/*Iterator IDs*/
typedef long AttributeIteratorID;
typedef long AttributeValueIteratorID;
typedef long BreakPointIteratorID;
typedef long PCMBreakPointIteratorID;
typedef long BooleanValIteratorID;
typedef long ComponentIteratorID;
typedef long ComponentClassIteratorID;
typedef long DataTypeIteratorID;
typedef long FloatValIteratorID;
typedef long DateValIteratorID;
typedef long InstanceIteratorID;
typedef long ProdCompMapIteratorID;
typedef long ProdCompMapInstanceIteratorID;
typedef long ProductBaseIteratorID;
typedef long ProductInstanceIteratorID;
typedef long ProductInstanceOptionIteratorID;
typedef long ProductLineIteratorID;
typedef long RequestIteratorID;
typedef long ResourceIteratorID;
typedef long SelectionSetIteratorID;
typedef long StringValIteratorID;
typedef long SystemIteratorID;
typedef long PartNumberIteratorID;
typedef long ProductIteratorID;
typedef long ProductOptionGroupIteratorID;
typedef long ProductOptionIteratorID;
typedef long DefaultKeywordIteratorID;
typedef long DefaultSetIteratorID;
typedef long DefaultValueIteratorID;

typedef long BlockIteratorID;

/* den 12/27/93 added for maint */
typedef long ParseErrorID;
typedef long SubComponentID;
typedef long SubComponentIteratorID;
/* den 2/3/94 added for maint */
/* den 2/5/94 added for maint */
typedef long ProductAttributeID;
typedef long ProductAttributeIteratorID;
/* den 2/16/94 added for maint */
typedef long ConstraintID;
typedef long ConstraintIteratorID;
/* den 1/24/94 added for maint */
typedef long ConstraintExpressionID;
typedef long ConstraintExpressionIteratorID;
typedef long ResourceValueID;
typedef long ResourceValueIteratorID;
/* den 2/2/94 added for maint */
typedef long RequestSessionID;
typedef long RequestSessionIteratorID;
typedef long ScriptSessionID;
typedef long ScriptSessionIteratorID;
/* den 3/5/94 added for maint debugger */
typedef long GoalID;
typedef long GoalIteratorID;
/* den 6/18/94 added for maint */
typedef long TypeID;
typedef long TypeIteratorID;
/* den 6/18/94 added for maint */
typedef long BreakPointID;
typedef long PCMBreakPointID;
/* den 11/12/94 added for maint */
typedef long ConditionID;
typedef long ConditionIteratorID;

#else

struct DummyStruct {int dummy;};
typedef DummyStruct* ObjID;

#define SB_DUMMYSTRUCT(Type) typedef struct Dummy##Type##Struct { int dummy; } *##Type##ID;

typedef void* IteratorID ;

SB_DUMMYSTRUCT(Attribute)
SB_DUMMYSTRUCT(AttributeValue)
SB_DUMMYSTRUCT(BreakPoint)
SB_DUMMYSTRUCT(Condition)
SB_DUMMYSTRUCT(PCMBreakPoint)
SB_DUMMYSTRUCT(Component)
SB_DUMMYSTRUCT(ComponentClass)
SB_DUMMYSTRUCT(DataType)
SB_DUMMYSTRUCT(InitializationList)
SB_DUMMYSTRUCT(Instance)
SB_DUMMYSTRUCT(ProdCompMap)
SB_DUMMYSTRUCT(ProdCompMapInstance)
SB_DUMMYSTRUCT(ProductBase)
SB_DUMMYSTRUCT(ProductInstance)
SB_DUMMYSTRUCT(ProductInstanceOption)
SB_DUMMYSTRUCT(ProductLine)
SB_DUMMYSTRUCT(Request)
SB_DUMMYSTRUCT(RequestGroup)
SB_DUMMYSTRUCT(Resource)
SB_DUMMYSTRUCT(SelectionSet)
SB_DUMMYSTRUCT(System)
SB_DUMMYSTRUCT(PartNumber)
SB_DUMMYSTRUCT(Product)
SB_DUMMYSTRUCT(ProductOption)
SB_DUMMYSTRUCT(ProductOptionGroup)
SB_DUMMYSTRUCT(DefaultKeyword)
SB_DUMMYSTRUCT(DefaultSet)
SB_DUMMYSTRUCT(DefaultValue)
SB_DUMMYSTRUCT(Bundler)



SB_DUMMYSTRUCT(AttributeIterator)
SB_DUMMYSTRUCT(AttributeValueIterator)
SB_DUMMYSTRUCT(BreakPointIterator)
SB_DUMMYSTRUCT(ConditionIterator)
SB_DUMMYSTRUCT(PCMBreakPointIterator)
SB_DUMMYSTRUCT(BooleanValIterator)
SB_DUMMYSTRUCT(ComponentIterator)
SB_DUMMYSTRUCT(ComponentClassIterator)
SB_DUMMYSTRUCT(DataTypeIterator)
SB_DUMMYSTRUCT(FloatValIterator)
SB_DUMMYSTRUCT(DateValIterator)
SB_DUMMYSTRUCT(InstanceIterator)
SB_DUMMYSTRUCT(ProdCompMapIterator)
SB_DUMMYSTRUCT(ProdCompMapInstanceIterator)
SB_DUMMYSTRUCT(ProductBaseIterator)
SB_DUMMYSTRUCT(ProductInstanceIterator)
SB_DUMMYSTRUCT(ProductInstanceOptionIterator)
SB_DUMMYSTRUCT(ProductLineIterator)
SB_DUMMYSTRUCT(RequestIterator)
SB_DUMMYSTRUCT(ResourceIterator)
SB_DUMMYSTRUCT(SelectionSetIterator)
SB_DUMMYSTRUCT(StringValIterator)
SB_DUMMYSTRUCT(SystemIterator)
SB_DUMMYSTRUCT(PartNumberIterator)
SB_DUMMYSTRUCT(ProductIterator)
SB_DUMMYSTRUCT(ProductOptionGroupIterator)
SB_DUMMYSTRUCT(ProductOptionIterator)
SB_DUMMYSTRUCT(DefaultKeywordIterator)
SB_DUMMYSTRUCT(DefaultSetIterator)
SB_DUMMYSTRUCT(DefaultValueIterator)

SB_DUMMYSTRUCT(BlockIterator)


/* Maint API argument types */
SB_DUMMYSTRUCT(ParseError)
SB_DUMMYSTRUCT(SubComponent)
SB_DUMMYSTRUCT(SubComponentIterator)
SB_DUMMYSTRUCT(Object)
SB_DUMMYSTRUCT(ObjectIterator)
SB_DUMMYSTRUCT(ProductAttributeIterator);
SB_DUMMYSTRUCT(Constraint);
SB_DUMMYSTRUCT(ConstraintIterator);
SB_DUMMYSTRUCT(ConstraintExpression)
SB_DUMMYSTRUCT(ConstraintExpressionIterator)
SB_DUMMYSTRUCT(ResourceValue)
SB_DUMMYSTRUCT(ResourceValueIterator)
SB_DUMMYSTRUCT(RequestSession);
SB_DUMMYSTRUCT(RequestSessionIterator);
SB_DUMMYSTRUCT(ScriptSession);
SB_DUMMYSTRUCT(ScriptSessionIterator);
SB_DUMMYSTRUCT(Goal)
SB_DUMMYSTRUCT(GoalIterator)
SB_DUMMYSTRUCT(Type)
SB_DUMMYSTRUCT(TypeIterator)



#endif	/*SB_STRONGTYPECHECK*/

typedef float Coord;

/*
	Specify the calling convention for our callback functions under OS/2.
	This is because the default calling convention that IBM CSet and
	Borland C++ generate are incompatible.  So, pass callback functions
	with APIENTRY (_System for CSet and _syscall for Borland) convention.
	Borland and IBM also expect to see the calling convention in different
	places in a typedef, thus the two defines.  All callbacks should be
	defined like:

	void TDGCB_CALLCONV myCallBackFunction(...)

	These calling convention specifications are also required under Symantec
	C++ on the Macintosh.
*/



#	if defined(__IBMCPP__) || defined(__IBMC__)
#		define TDGCB_CALLCONV1
#		define TDGCB_CALLCONV2 _System
#	else
#		define TDGCB_CALLCONV1
#		define TDGCB_CALLCONV2
#	endif


#define TDGCB_CALLCONV TDGCB_CALLCONV1 TDGCB_CALLCONV2

#ifndef IDL

typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 TICKFUNC) ();
typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 MONITORFUNC) (float current, float total, long type, const char*);
typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 TRACEFUNC) (const char *, int);
typedef void TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 DRAWFUNC) (InstanceID,InstanceID,Coord,Coord,Coord,Coord,const char *);
typedef void TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 DISPLAYFUNC) (InstanceID,const char *);
typedef void TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 APIERRORFUNC) (long,const char *);
typedef long TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 FAILFUNC)(const char *);
typedef void TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 UNDISPLAYFUNC) (InstanceID);
typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 RESOLVEPLANFUNC) (const char*, InstanceIteratorID, InstanceIteratorID, ComponentIteratorID);
typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 BREAKPOINTFUNC) (BreakPointID);
typedef int TDGCB_CALLCONV1 (* TDGCB_CALLCONV2 PCMBREAKPOINTFUNC) (PCMBreakPointID);

#else

typedef long *TICKFUNC;
typedef long *MONITORFUNC;
typedef long *TRACEFUNC;
typedef long *DRAWFUNC;
typedef long *DISPLAYFUNC;
typedef long *APIERRORFUNC;
typedef long *FAILFUNC;
typedef long *UNDISPLAYFUNC;
typedef long *RESOLVEPLANFUNC;
typedef long *BREAKPOINTFUNC;
typedef long *PCMBREAKPOINTFUNC;

#endif /* DCE */



#if defined(__cplusplus) && !defined(MANGLE)
}
#endif


#endif


