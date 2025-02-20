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

#ifndef __ND_MULTIBODY_VEHICLE_H__
#define __ND_MULTIBODY_VEHICLE_H__

#include "ndNewtonStdafx.h"
#include "ndModel.h"
#include "ndIkSolver.h"

class ndWorld;
class ndMultiBodyVehicleMotor;
class ndMultiBodyVehicleGearBox;
class ndMultiBodyVehicleTireJoint;
class ndMultiBodyVehicleTorsionBar;
class ndMultiBodyVehicleDifferential;
class ndMultiBodyVehicleTireJointInfo;
class ndMultiBodyVehicleDifferentialAxle;

#define dRadPerSecToRpm ndFloat32(9.55f)

class ndMultiBodyVehicle: public ndModel
{
	public:
	class ndTireContactPair
	{
		public:
		ndContact* m_contact;
		ndMultiBodyVehicleTireJoint* m_tireJoint;
	};

	class ndDownForce
	{
		public:
		class ndSpeedForcePair
		{
			public:
			ndFloat32 m_speed;
			ndFloat32 m_forceFactor;
			ndFloat32 m_aerodynamicDownforceConstant;
			friend class ndDownForce;
		};

		ndDownForce();
		ndFloat32 GetDownforceFactor(ndFloat32 speed) const;

		void Load(const nd::TiXmlNode* const xmlNode);
		void Save(nd::TiXmlNode* const xmlNode) const;

		private:
		ndFloat32 CalculateFactor(const ndSpeedForcePair* const entry) const;
	
		ndFloat32 m_gravity;
		ndFloat32 m_suspensionStiffnessModifier;
		ndSpeedForcePair m_downForceTable[5];
		friend class ndMultiBodyVehicle;
		friend class ndMultiBodyVehicleTireJoint;
	};

	D_CLASS_REFLECTION(ndMultiBodyVehicle);
	D_NEWTON_API ndMultiBodyVehicle(const ndLoadSaveBase::ndLoadDescriptor& desc);
	D_NEWTON_API ndMultiBodyVehicle(const ndVector& frontDir, const ndVector& upDir);
	D_NEWTON_API virtual ~ndMultiBodyVehicle ();

	D_NEWTON_API ndFloat32 GetSpeed() const;
	D_NEWTON_API ndShapeInstance CreateTireShape(ndFloat32 radius, ndFloat32 width) const;

	D_NEWTON_API void AddChassis(ndSharedPtr<ndBodyKinematic>& chassis);
	D_NEWTON_API ndMultiBodyVehicleMotor* AddMotor(ndFloat32 mass, ndFloat32 radius);
	D_NEWTON_API ndMultiBodyVehicleGearBox* AddGearBox(ndMultiBodyVehicleDifferential* const differential);
	D_NEWTON_API ndMultiBodyVehicleTireJoint* AddTire(const ndMultiBodyVehicleTireJointInfo& desc, ndSharedPtr<ndBodyKinematic>& tire);
	D_NEWTON_API ndMultiBodyVehicleTireJoint* AddAxleTire(const ndMultiBodyVehicleTireJointInfo& desc, ndSharedPtr<ndBodyKinematic>& tire, ndSharedPtr<ndBodyKinematic>& axleBody);
	D_NEWTON_API ndMultiBodyVehicleDifferential* AddDifferential(ndFloat32 mass, ndFloat32 radius, ndMultiBodyVehicleTireJoint* const leftTire, ndMultiBodyVehicleTireJoint* const rightTire, ndFloat32 slipOmegaLock);
	D_NEWTON_API ndMultiBodyVehicleDifferential* AddDifferential(ndFloat32 mass, ndFloat32 radius, ndMultiBodyVehicleDifferential* const leftDifferential, ndMultiBodyVehicleDifferential* const rightDifferential, ndFloat32 slipOmegaLock);
	D_NEWTON_API ndMultiBodyVehicleTorsionBar* AddTorsionBar(ndBodyKinematic* const sentinel);

	D_NEWTON_API void AddExtraBody(ndSharedPtr<ndBodyKinematic>& body);
	D_NEWTON_API void AddExtraJoint(ndSharedPtr<ndJointBilateralConstraint>& joint);

	D_NEWTON_API void SetVehicleSolverModel(bool hardJoint);

	D_NEWTON_API ndMultiBodyVehicle* GetAsMultiBodyVehicle();

	// left undefined intentionally
	//ndMultiBodyVehicleTireJoint* AddTire(const ndMultiBodyVehicleTireJointInfo& desc, ndBodyKinematic* const tire);

	private:
	void ApplyAerodynamics();
	void ApplyAligmentAndBalancing();
	void ApplyTireModel(ndFloat32 timestep);
	ndBodyKinematic* CreateInternalBodyPart(ndFloat32 mass, ndFloat32 radius) const;
	void ApplyTireModel(ndFloat32 timestep, ndTireContactPair* const tires, ndInt32 tireCount);
	void ApplyVehicleDynamicControl(ndFloat32 timestep, ndTireContactPair* const tires, ndInt32 tireCount);

	void CalculateNormalizedAlgningTorque(ndMultiBodyVehicleTireJoint* const tire, ndFloat32 sideSlipTangent) const;
	void CoulombTireModel(ndMultiBodyVehicleTireJoint* const tire, ndContactMaterial& contactPoint, ndFloat32 timestep) const;
	void BrushTireModel(ndMultiBodyVehicleTireJoint* const tire, ndContactMaterial& contactPoint, ndFloat32 timestep) const;
	void PacejkaTireModel(ndMultiBodyVehicleTireJoint* const tire, ndContactMaterial& contactPoint, ndFloat32 timestep) const;
	void CoulombFrictionCircleTireModel(ndMultiBodyVehicleTireJoint* const tire, ndContactMaterial& contactPoint, ndFloat32 timestep) const;

	protected:
	virtual void ApplyInputs(ndWorld* const world, ndFloat32 timestep);
	D_NEWTON_API virtual void Debug(ndConstraintDebugCallback& context) const;
	D_NEWTON_API virtual void Update(ndWorld* const world, ndFloat32 timestep);
	D_NEWTON_API virtual void PostUpdate(ndWorld* const world, ndFloat32 timestep);
	D_NEWTON_API virtual void Save(const ndLoadSaveBase::ndSaveDescriptor& desc) const;

	ndMatrix m_localFrame;
	ndBodyKinematic* m_chassis;
	ndIkSolver m_invDynamicsSolver;
	ndMultiBodyVehicleMotor* m_motor;
	ndShapeChamferCylinder* m_tireShape;
	ndMultiBodyVehicleGearBox* m_gearBox;
	ndMultiBodyVehicleTorsionBar* m_torsionBar;
	ndList<ndMultiBodyVehicleTireJoint*> m_tireList;
	ndList<ndBodyKinematic*> m_extraBodiesAttachmentList;
	ndList<ndMultiBodyVehicleDifferentialAxle*> m_axleList;
	ndList<ndMultiBodyVehicleDifferential*> m_differentialList;
	ndList<ndJointBilateralConstraint*> m_extraJointsAttachmentList;
	ndDownForce m_downForce;
	
	friend class ndMultiBodyVehicleMotor;
	friend class ndMultiBodyVehicleGearBox;
	friend class ndMultiBodyVehicleTireJoint;
	friend class ndMultiBodyVehicleTorsionBar;
};

inline void ndMultiBodyVehicle::ApplyInputs(ndWorld* const, ndFloat32)
{
}

#endif