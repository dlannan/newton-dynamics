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

#ifndef _ND_BRAIN_MATRIX_H__
#define _ND_BRAIN_MATRIX_H__

#include "ndBrainStdafx.h"
#include "ndBrainVector.h"

class ndBrainMatrix: public ndArray<ndBrainVector>
{
	public: 
	ndBrainMatrix();
	ndBrainMatrix(ndInt32 rows, ndInt32 columns);
	ndBrainMatrix(const ndBrainMatrix& src);
	~ndBrainMatrix();

	void Init(ndInt32 rows, ndInt32 columns);

	ndInt32 GetRows() const;
	ndInt32 GetColumns() const;
	void Set(ndReal value);
	void Set(const ndBrainMatrix& src);
	void SetTranspose(const ndBrainMatrix& src);
	void Mul(const ndBrainVector& input, ndBrainVector& output) const;

	protected:
	ndUnsigned8* SetPointer(ndUnsigned8* const mem);
	ndReal* SetFloatPointers(ndReal* const mem, ndInt32 columns);
};

inline ndInt32 ndBrainMatrix::GetRows() const
{
	return GetCount();
}

inline ndInt32 ndBrainMatrix::GetColumns() const
{
	ndInt32 columns = GetCount() ? (*this)[0].GetCount() : 0;
	return columns;
}

#endif 
