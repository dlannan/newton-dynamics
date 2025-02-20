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

#include "ndCoreStdafx.h"
#include "ndTinyXmlGlue.h"

static char* FloatToString(char* const buffer, ndFloat32 value)
{
	sprintf(buffer, "%f", value);
	char* ptr = buffer + strlen(buffer);
	while (*(--ptr) == '0')
	{
		*ptr = ' ';
	}
	if (*ptr == '.')
	{
		ptr++;
		*ptr = '0';
	}
	ptr++;
	*ptr = ' ';
	ptr++;
	*ptr = 0;
	return ptr;
}

static void CleanWhiteSpace(const char* const value)
{
	size_t size = strlen(value) - 1;
	if (value[size] == ' ')
	{
		char* ptr = (char*)value;
		ptr[size] = 0;
	}
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const char* const type, const char* const value)
{
	nd::TiXmlElement* const node = new nd::TiXmlElement(name);
	rootNode->LinkEndChild(node);
	CleanWhiteSpace(value);
	node->SetAttribute(type, value);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, ndInt32 value)
{
	char buffer[1024];
	sprintf(buffer, "%d", value);
	xmlSaveParam(rootNode, name, "int32", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, ndInt64 value)
{
	char buffer[1024];
	long long x = value;
	sprintf(buffer, "%llu", x);
	xmlSaveParam(rootNode, name, "int64", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, ndFloat32 value)
{
	char buffer[1024];
	FloatToString(buffer, value);
	xmlSaveParam(rootNode, name, "float", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const ndVector& value)
{
	char buffer[1024];
	char* ptr0 = FloatToString(buffer, value.m_x);
	char* ptr1 = FloatToString(ptr0, value.m_y);
	FloatToString(ptr1, value.m_z);
	xmlSaveParam(rootNode, name, "float3", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const ndMatrix& value)
{
	ndVector euler0;
	ndVector euler1;
	value.CalcPitchYawRoll(euler0, euler1);
	euler0 = euler0.Scale(ndRadToDegree);

	char buffer[256];
	nd::TiXmlElement* const node = new nd::TiXmlElement(name);
	rootNode->LinkEndChild(node);

	sprintf(buffer, "%f %f %f", value.m_posit.m_x, value.m_posit.m_y, value.m_posit.m_z);
	node->SetAttribute("position", buffer);
	sprintf(buffer, "%f %f %f", euler0.m_x, euler0.m_y, euler0.m_z);
	node->SetAttribute("angles", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const char* const value)
{
	xmlSaveParam(rootNode, name, "string", value);
}

//void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, ndInt32 count, const ndVector* const array)
void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const ndArray<ndVector>& array)
{
	char* const buffer = ndAlloca(char, array.GetCount() * 4 * 12 + 256);
	char* ptr = buffer;
	for (ndInt32 i = 0; i < array.GetCount(); ++i)
	{
		for (ndInt32 j = 0; j < 4; ++j)
		{
			ptr = FloatToString(ptr, array[i][j]);
		}
	}
	CleanWhiteSpace(buffer);

	nd::TiXmlElement* const node = new nd::TiXmlElement(name);
	rootNode->LinkEndChild(node);
	
	node->SetAttribute("count", array.GetCount());
	node->SetAttribute("float4Array", buffer);
}

void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const ndArray<ndFloat32>& array)
{
	char* const buffer = ndAlloca(char, array.GetCount() * 12 + 256);
	char* ptr = buffer;
	for (ndInt32 i = 0; i < array.GetCount(); ++i)
	{
		ptr = FloatToString(ptr, array[i]);
	}
	CleanWhiteSpace(buffer);

	nd::TiXmlElement* const node = new nd::TiXmlElement(name);
	rootNode->LinkEndChild(node);

	node->SetAttribute("count", array.GetCount());
	node->SetAttribute("floatArray", buffer);
}

#ifdef D_NEWTON_USE_DOUBLE
void xmlSaveParam(nd::TiXmlElement* const rootNode, const char* const name, const ndArray<ndReal>& array)
{
	char* const buffer = ndAlloca(char, array.GetCount() * 12 + 256);
	char* ptr = buffer;
	for (ndInt32 i = 0; i < array.GetCount(); ++i)
	{
		ptr = FloatToString(ptr, array[i]);
	}
	CleanWhiteSpace(buffer);

	nd::TiXmlElement* const node = new nd::TiXmlElement(name);
	rootNode->LinkEndChild(node);

	node->SetAttribute("count", array.GetCount());
	node->SetAttribute("realArray", buffer);
}
#endif

ndInt32 xmlGetInt(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	ndInt32 value;
	element->Attribute("int32", &value);
	return value;
}

ndInt64 xmlGetInt64(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	const char* const data = element->Attribute("int64");

	long long int value;
	sscanf(data, "%lld", &value);
	return ndInt64 (value);
}

ndFloat32 xmlGetFloat(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	ndFloat64 value;
	element->Attribute("float", &value);
	return ndFloat32 (value);
}

#ifdef D_NEWTON_USE_DOUBLE
void xmlGetFloatArray(const nd::TiXmlNode* const rootNode, const char* const name, ndArray<ndReal>& array)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	ndInt32 count;
	element->Attribute("count", &count);
	array.Resize(count);
	array.SetCount(count);

	const char* const data = element->Attribute("floatArray");

	size_t start = 0;
	for (ndInt32 i = 0; i < count; ++i)
	{
		char x[64];
		sscanf(&data[start], "%[^ ]", x);
		start += strlen(x) + 1;

		ndFloat64 fx;
		sscanf(x, "%lf", &fx);
		array[i] = ndReal(fx);
	}
}
#endif

void xmlGetFloatArray(const nd::TiXmlNode* const rootNode, const char* const name, ndArray<ndFloat32>& array)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*)rootNode->FirstChild(name);
	ndAssert(element);
	ndInt32 count;
	element->Attribute("count", &count);
	array.Resize(count);
	array.SetCount(count);

	const char* const data = element->Attribute("floatArray");

	size_t start = 0;
	for (ndInt32 i = 0; i < count; ++i)
	{
		char x[64];
		sscanf(&data[start], "%[^ ]", x);
		start += strlen(x) + 1;

		ndFloat64 fx;
		sscanf(x, "%lf", &fx);
		array[i] = ndFloat32(fx);
	}
}

void xmlGetFloatArray3(const nd::TiXmlNode* const rootNode, const char* const name, ndArray<ndVector>& array)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	ndInt32 count;
	element->Attribute("count", &count);
	array.Resize(count);
	array.SetCount(count);

	const char* const data = element->Attribute("float4Array");
	
	size_t start = 0;
	ndVector point(ndVector::m_zero);
	for (ndInt32 i = 0; i < count; ++i)
	{
		char x[64];
		char y[64];
		char z[64];
		char w[64];
		sscanf(&data[start], "%[^ ] %[^ ] %[^ ] %[^ ]", x, y, z, w);
		start += strlen(x) + strlen(y) + strlen(z) + strlen(w) + 4;

		ndFloat64 fx;
		ndFloat64 fy;
		ndFloat64 fz;
		ndFloat64 fw;
		
		sscanf(x, "%lf", &fx);
		sscanf(y, "%lf", &fy);
		sscanf(z, "%lf", &fz);
		sscanf(w, "%lf", &fw);

		point.m_x = ndFloat32(fx);
		point.m_y = ndFloat32(fy);
		point.m_z = ndFloat32(fz);
		point.m_w = ndFloat32(fw);

		array[i] = point;
	}
}

ndVector xmlGetVector3(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);

	const char* const positData = element->Attribute("float3");

	ndFloat64 fx;
	ndFloat64 fy;
	ndFloat64 fz;
	sscanf(positData, "%lf %lf %lf", &fx, &fy, &fz);

	ndVector posit(ndVector::m_zero);
	posit.m_x = ndFloat32(fx);
	posit.m_y = ndFloat32(fy);
	posit.m_z = ndFloat32(fz);
	return posit;
}

ndMatrix xmlGetMatrix(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	
	const char* const positData = element->Attribute("position");
	const char* const angleData = element->Attribute("angles");

	ndVector posit(ndVector::m_one);
	ndVector euler(ndVector::m_zero);

	ndFloat64 fx;
	ndFloat64 fy;
	ndFloat64 fz;
	//sscanf(positData, "%f %f %f", &posit.m_x, &posit.m_y, &posit.m_z);
	sscanf(positData, "%lf %lf %lf", &fx, &fy, &fz);
	posit.m_x = ndFloat32(fx);
	posit.m_y = ndFloat32(fy);
	posit.m_z = ndFloat32(fz);

	//sscanf(angleData, "%f %f %f", &euler.m_x, &euler.m_y, &euler.m_z);
	sscanf(angleData, "%lf %lf %lf", &fx, &fy, &fz);
	euler.m_x = ndFloat32(fx);
	euler.m_y = ndFloat32(fy);
	euler.m_z = ndFloat32(fz);
	euler = euler.Scale(ndDegreeToRad);

	ndMatrix matrix (ndPitchMatrix(euler.m_x) * ndYawMatrix(euler.m_y) * ndRollMatrix(euler.m_z));
	matrix.m_posit = posit;
	return matrix;
}

const char* xmlGetString(const nd::TiXmlNode* const rootNode, const char* const name)
{
	const nd::TiXmlElement* const element = (nd::TiXmlElement*) rootNode->FirstChild(name);
	ndAssert(element);
	return element->Attribute("string");
}

const nd::TiXmlNode* xmlFind(const nd::TiXmlNode* const rootNode, const char* const name)
{
	//for (const nd::TiXmlElement* node = (nd::TiXmlElement*) rootNode->FirstChild(name); node; node = (nd::TiXmlElement*) node->NextSibling())
	//{
	//	const char* const text = node->GetText();
	//	if (!strcmp(text, name))
	//	{
	//		return node;
	//	}
	//}
	return rootNode->FirstChild(name);
}