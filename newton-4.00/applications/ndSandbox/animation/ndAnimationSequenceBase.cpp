/* Copyright (c) <2003-2016> <Newton Game Dynamics>
* 
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
* 
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely
*/

#include "ndSandboxStdafx.h"
#include "ndAnimationPose.h"
#include "ndAnimationSequenceBase.h"

ndAnimationSequenceBase::ndAnimationSequenceBase()
	:m_name("")
{
}

ndAnimationSequenceBase::~ndAnimationSequenceBase()
{
}

const ndString& ndAnimationSequenceBase::GetName() const
{
	return m_name;
}

void ndAnimationSequenceBase::SetName(const char* const name)
{
	m_name = name;
}

