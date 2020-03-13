/*
 * Copyright 2019 Side Effects Software Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Produced by:
 *	Side Effects Software Inc.
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *      Canada   M5J 2M2
 *	416-504-9876
 *
 */

#ifndef __HUSD_FindPrims_h__
#define __HUSD_FindPrims_h__

#include "HUSD_API.h"
#include "HUSD_DataHandle.h"
#include "HUSD_Utils.h"
#include <OP/OP_ItemId.h>
#include <UT/UT_BoundingBox.h>
#include <UT/UT_StringMap.h>
#include <UT/UT_StringArray.h>
#include <UT/UT_UniquePtr.h>
#include <UT/UT_VectorTypes.h>
#include <pxr/pxr.h>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

class XUSD_PathSet;
class XUSD_PathPattern;

PXR_NAMESPACE_CLOSE_SCOPE

class HUSD_TimeCode;

class HUSD_API HUSD_FindPrims
{
public:
			 HUSD_FindPrims(HUSD_AutoAnyLock &lock,
				 HUSD_PrimTraversalDemands demands =
				    HUSD_TRAVERSAL_DEFAULT_DEMANDS,
				 bool find_point_instancer_ids = false);
			 // Simple constructors when you just want to operate
			 // on one or an array of primitives and you know the
			 // paths.
			 HUSD_FindPrims(HUSD_AutoAnyLock &lock,
				 const UT_StringRef &primpath,
				 HUSD_PrimTraversalDemands demands =
				    HUSD_TRAVERSAL_DEFAULT_DEMANDS);
			 HUSD_FindPrims(HUSD_AutoAnyLock &lock,
				 const UT_StringArray &primpaths,
				 HUSD_PrimTraversalDemands demands =
				    HUSD_TRAVERSAL_DEFAULT_DEMANDS);
			 HUSD_FindPrims(HUSD_AutoAnyLock &lock,
				 const std::vector<std::string> &primpaths,
				 HUSD_PrimTraversalDemands demands =
				    HUSD_TRAVERSAL_DEFAULT_DEMANDS);
			 // Copy constructor that evaluates the source's
			 // expanded path set, and uses that as our own
			 // base path set. Also inherits other source
			 // settings like demands, base type, etc.
			 HUSD_FindPrims(const HUSD_FindPrims &src);
			~HUSD_FindPrims();

    enum BBoxContainment {
	BBOX_FULLY_INSIDE,
	BBOX_FULLY_OUTSIDE,
	BBOX_PARTIALLY_INSIDE,
	BBOX_PARTIALLY_OUTSIDE
    };

    const PXR_NS::XUSD_PathSet	&getExpandedPathSet() const;
    const PXR_NS::XUSD_PathSet	&getCollectionAwarePathSet() const;
    const PXR_NS::XUSD_PathSet	&getExcludedPathSet(bool skipdescendants) const;

    void                 setTraversalRoot(const UT_StringRef &primpath);
    void		 setBaseTypeName(const UT_StringRef &base_type_name);
    void		 setTraversalDemands(HUSD_PrimTraversalDemands demands);
    void                 setAssumeWildcardsAroundPlainTokens(bool assume);
    bool		 addPattern(const UT_StringArray &pattern_tokens,
                                int nodeid = OP_INVALID_NODE_ID);
    bool		 addPattern(const UT_StringRef &pattern,
				int nodeid,
				const HUSD_TimeCode &timecode);
    bool		 addPrimitiveType(const UT_StringRef &primtype);
    bool		 addPrimitiveKind(const UT_StringRef &primkind);
    bool		 addPrimitivePurpose(const UT_StringRef &primpurpose);
    bool		 addVexpression(const UT_StringRef &vexpression,
				int nodeid,
				const HUSD_TimeCode &timecode) const;
    bool		 addBoundingBox(const UT_BoundingBox &bbox,
				const HUSD_TimeCode &t,
				const UT_StringArray &purposes,
				BBoxContainment containment);
    bool		 addDescendants();
    bool		 addAncestors();

    const UT_StringMap<UT_Int64Array>	&getPointInstancerIds() const;
    bool		 getExcludedPointInstancerIds(
				UT_StringMap<UT_Int64Array> &excludedids,
				const HUSD_TimeCode &timecode) const;
    // Returns the array of string from getExpandedPathSet.
    void		 getExpandedPaths(UT_StringArray &paths) const;
    // Returns the array of string from getCollectionAwarePathSet.
    void		 getCollectionAwarePaths(UT_StringArray &paths) const;
    // Returns the array of string from getExcludedPathSet.
    void		 getExcludedPaths(UT_StringArray &paths,
                                bool skipdescendants) const;
    bool		 getIsEmpty() const;
    bool		 getFindPointInstancerIds() const;
    bool		 getIsTimeVarying() const;
    bool		 allowInstanceProxies() const;

    /// Returns a collection path, if only a single collection was added.
    /// Returns an empty string, if primitive target consists of more than 
    /// a single collection.
    UT_StringHolder	 getSingleCollectionPath() const;

    /// Returns the path to the most nested primitive that is the shared
    /// root primitive of every primitive in our expanded set. If the shared
    /// root prim is the absolute root, we return an empty string.
    UT_StringHolder      getSharedRootPrim() const;

    /// Return a string describing the last error generated by a failed
    /// call to this object's methods.
    const UT_StringHolder &getLastError() const
			 { return myLastError; }

private:
    bool		 addPattern(const PXR_NS::XUSD_PathPattern &pattern,
                                int nodeid);

    class husd_FindPrimsPrivate;

    UT_UniquePtr<husd_FindPrimsPrivate>	 myPrivate;
    HUSD_AutoAnyLock			&myAnyLock;
    HUSD_PrimTraversalDemands		 myDemands;
    UT_StringHolder			 myLastError;
    bool				 myFindPointInstancerIds;
    bool				 myAssumeWildcardsAroundPlainTokens;
};

#endif

