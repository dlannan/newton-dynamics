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

#ifndef __ND_SKELETON_CONTAINER_H__
#define __ND_SKELETON_CONTAINER_H__

#include "ndNewtonStdafx.h"

class ndIkSolver;
class ndJointBilateralConstraint;

class ndSkeletonContainer 
{
	public:
	ndSkeletonContainer();
	~ndSkeletonContainer();

	protected:
	class ndOrdinal
	{
		public:
		ndOrdinal()
		{
			for (ndInt32 i = 0; i < ndInt32(sizeof(m_sourceJacobianIndex)); ++i)
			{
				m_sourceJacobianIndex[i] = ndInt8(i);
			}
		}
		
		ndInt8 m_sourceJacobianIndex[12];
	};

	class ndNodePair
	{
		public:
		ndInt32 m_m0;
		ndInt32 m_m1;
		const ndConstraint* m_joint;
	};

	D_MSV_NEWTON_ALIGN_32
	class ndForcePair
	{
		public:
		ndSpatialVector m_joint;
		ndSpatialVector m_body;
	} D_GCC_NEWTON_ALIGN_32;

	D_MSV_NEWTON_ALIGN_32
	class ndMatriData
	{
		public:
		ndSpatialMatrix m_jt;
		ndSpatialMatrix m_invMass;
	} D_GCC_NEWTON_ALIGN_32;

	D_MSV_NEWTON_ALIGN_32
	class ndBodyJointMatrixDataPair
	{
		public:
		ndMatriData m_body;
		ndMatriData m_joint;
	} D_GCC_NEWTON_ALIGN_32;

	class ndNode
	{
		public:
		ndNode();
		~ndNode();
		ndInt32 Factorize(const ndLeftHandSide* const leftHandSide, const ndRightHandSide* const rightHandSide, ndSpatialMatrix* const bodyMassArray, ndSpatialMatrix* const jointMassArray);

		inline void CalculateJacobianBlock();
		inline void CalculateInertiaMatrix(ndSpatialMatrix* const bodyMassArray) const;
		inline void CalculateJointDiagonal(const ndSpatialMatrix* const bodyMassArray, ndSpatialMatrix* const jointMassArray);
		inline void CalculateBodyDiagonal(ndNode* const child, ndSpatialMatrix* const bodyMassArray, const ndSpatialMatrix* const jointMassArray);
		inline void GetJacobians(const ndLeftHandSide* const leftHandSide, const ndRightHandSide* const rightHandSide, ndSpatialMatrix* const jointMassArray);

		inline void BodyDiagInvTimeSolution(ndForcePair& force);
		inline void JointDiagInvTimeSolution(ndForcePair& force);
		inline void JointJacobianTimeMassForward(ndForcePair& force);
		inline void BodyJacobianTimeSolutionBackward(ndForcePair& force) const;
		inline ndInt32 GetAuxiliaryRows(const ndRightHandSide* const rightHandSide) const;
		inline void BodyJacobianTimeMassForward(const ndForcePair& force, ndForcePair& parentForce) const;
		inline void JointJacobianTimeSolutionBackward(ndForcePair& force, const ndForcePair& parentForce) const;

		ndBodyJointMatrixDataPair m_data;
		ndBodyKinematic* m_body;
		ndJointBilateralConstraint* m_joint;
		ndNode* m_parent;
		ndNode* m_child;
		ndNode* m_sibling;
		ndInt32 m_index;
		ndOrdinal m_ordinal;
		ndInt8 m_dof;
		ndInt8 m_swapJacobianBodiesIndex;
	};

	class ndNodeList : public ndList<ndNode, ndContainersFreeListAlloc<ndSkeletonContainer::ndNode> >
	{
		public:
		ndNodeList()
			:ndList<ndSkeletonContainer::ndNode, ndContainersFreeListAlloc<ndSkeletonContainer::ndNode> >()
		{
		}
	};

	private:
	ndNode* GetRoot() const;

	void Clear();
	void CheckSleepState();
	void Init(ndBodyKinematic* const rootBody);
	ndNode* AddChild(ndJointBilateralConstraint* const joint, ndNode* const parent);
	void Finalize(ndInt32 loopJoints, ndJointBilateralConstraint** const loopJointArray);

	void InitLoopMassMatrix();
	void ClearCloseLoopJoints();
	void AddCloseLoopJoint(ndConstraint* const joint);
	void CalculateReactionForces(ndJacobian* const internalForces);
	void InitMassMatrix(const ndLeftHandSide* const matrixRow, ndRightHandSide* const rightHandSide);
	void CalculateBufferSizeInBytes();
	void ConditionMassMatrix() const;
	void SortGraph(ndNode* const root, ndInt32& index);
	void RebuildMassMatrix(const ndFloat32* const diagDamp) const;
	void CalculateLoopMassMatrixCoefficients(ndFloat32* const diagDamp);
	void FactorizeMatrix(ndInt32 size, ndInt32 stride, ndFloat32* const matrix, ndFloat32* const diagDamp) const;
	void SolveAuxiliary(ndJacobian* const internalForces, const ndForcePair* const accel, ndForcePair* const force) const;
	void SolveBlockLcp(ndInt32 size, ndInt32 blockSize, const ndFloat32* const x0, ndFloat32* const x, ndFloat32* const b, const ndFloat32* const low, const ndFloat32* const high, const ndInt32* const normalIndex) const;
	void SolveLcp(ndInt32 stride, ndInt32 size, const ndFloat32* const matrix, const ndFloat32* const x0, ndFloat32* const x, const ndFloat32* const b, const ndFloat32* const low, const ndFloat32* const high, const ndInt32* const normalIndex) const;

	inline void SolveBackward(ndForcePair* const force) const;
	inline void CalculateForce(ndForcePair* const force, const ndForcePair* const accel) const;
	inline void UpdateForces(ndJacobian* const internalForces, const ndForcePair* const force) const;
	inline void CalculateJointAccel(const ndJacobian* const internalForces, ndForcePair* const accel) const;
	inline void SolveForward(ndForcePair* const force, const ndForcePair* const accel, ndInt32 startNode) const;

	void SolveImmediate(ndIkSolver& solverInfo);
	void UpdateForcesImmediate(ndArray<ndBodyKinematic*>& bodyArray, const ndForcePair* const force) const;
	void CalculateJointAccelImmediate(const ndJacobian* const internalForces, ndForcePair* const accel) const;
	void SolveAuxiliaryImmediate(ndArray<ndBodyKinematic*>& bodyArray, ndJacobian* const internalForces, const ndForcePair* const accel, ndForcePair* const force) const;
	
	ndNode* m_skeleton;
	ndNode** m_nodesOrder;
	ndRightHandSide* m_rightHandSide;
	const ndLeftHandSide* m_leftHandSide;
	ndNodePair* m_pairs;
	ndInt32* m_frictionIndex;
	ndInt32* m_matrixRowsIndex;
	ndFloat32* m_massMatrix11;
	ndFloat32* m_massMatrix10;
	ndFloat32* m_deltaForce;

	ndNodeList m_nodeList;
	ndArray<ndConstraint*> m_loopingJoints;
	ndArray<ndInt8> m_auxiliaryMemoryBuffer;
	ndSpinLock m_lock;
	ndInt32 m_blockSize;
	ndInt32 m_rowCount;
	ndInt32 m_loopRowCount;
	ndInt32 m_auxiliaryRowCount;
	ndInt32 m_loopCount;
	ndInt32 m_dynamicsLoopCount;
	ndUnsigned8 m_isResting;

	friend class ndWorld;
	friend class ndIkSolver;
	friend class ndSkeletonList;
	friend class ndSkeletonQueue;
	friend class ndDynamicsUpdate;
	friend class ndDynamicsUpdateSoa;
	friend class ndDynamicsUpdateAvx2;
	friend class ndDynamicsUpdateCuda;
};

inline ndSkeletonContainer::ndNode* ndSkeletonContainer::GetRoot() const
{
	return m_skeleton;
}

#endif


