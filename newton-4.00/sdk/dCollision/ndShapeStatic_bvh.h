/* Copyright (c) <2003-2022> <Julio Jerez, Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __ND_SHAPE_STATIC_BVH__
#define __ND_SHAPE_STATIC_BVH__

#include "ndCollisionStdafx.h"
#include "ndShapeStaticMesh.h"

class ndShapeStatic_bvh: public ndShapeStaticMesh, public ndAabbPolygonSoup
{
	public:
	D_CLASS_REFLECTION(ndShapeStatic_bvh);
	D_COLLISION_API ndShapeStatic_bvh(const ndPolygonSoupBuilder& builder);
	D_COLLISION_API ndShapeStatic_bvh(const ndLoadSaveBase::ndLoadDescriptor& desc);
	D_COLLISION_API virtual ~ndShapeStatic_bvh();

	void *operator new (size_t size);
	void operator delete (void* ptr);

	protected:
	virtual ndShapeInfo GetShapeInfo() const;
	virtual ndShapeStatic_bvh* GetAsShapeStaticBVH() { return this; }
	virtual void DebugShape(const ndMatrix& matrix, ndShapeDebugNotify& debugCallback) const;
	virtual ndFloat32 RayCast(ndRayCastNotify& callback, const ndVector& localP0, const ndVector& localP1, ndFloat32 maxT, const ndBody* const body, ndContactPoint& contactOut) const;
	virtual void GetCollidingFaces(ndPolygonMeshDesc* const data) const;
	virtual void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;
	
	static ndFloat32 RayHit(void* const context, const ndFloat32* const polygon, ndInt32 strideInBytes, const ndInt32* const indexArray, ndInt32 indexCount);
	static ndIntersectStatus ShowDebugPolygon(void* const context, const ndFloat32* const polygon, ndInt32 strideInBytes, const ndInt32* const indexArray, ndInt32 indexCount, ndFloat32 hitDistance);
	static ndIntersectStatus GetTriangleCount(void* const context, const ndFloat32* const polygon, ndInt32 strideInBytes, const ndInt32* const indexArray, ndInt32 indexCount, ndFloat32 hitDistance);
	static ndIntersectStatus GetPolygon(void* const context, const ndFloat32* const polygon, ndInt32 strideInBytes, const ndInt32* const indexArray, ndInt32 indexCount, ndFloat32 hitDistance);

	private: 
	ndInt32 m_trianglesCount;

	friend class ndContactSolver;
};

inline void* ndShapeStatic_bvh::operator new (size_t size)
{
	return ndShapeStaticMesh::operator new (size);
}

inline void ndShapeStatic_bvh::operator delete (void* ptr)
{
	ndShapeStaticMesh::operator delete(ptr);
}


#endif
