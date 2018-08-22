
#ifndef CUSTOM_COLLISION_SHAPE_DATA_CPP
#define CUSTOM_COLLISION_SHAPE_DATA_CPP

#include <CustomCollisionData.h>

#include <Debug.h>

btCollisionShape * CustomCollisionShapeData::GetTriangleShape()
{
	if( triangleShape == NULL )
	{
		if( triangleData == NULL )
		{
			triangleData = new btTriangleIndexVertexArray( indices.size() / 3, &indices.front(), sizeof(int) * 3, vertices.size(), vertices.front().m_floats, sizeof(btVector3) );
			if( triangleData == NULL )
			{
				return NULL;
			}
		}
		triangleShape = new btBvhTriangleMeshShape( triangleData, true, true );
	}
	return triangleShape;
}

btCollisionShape * CustomCollisionShapeData::GetConvexShape()
{
	if( convexShape == NULL )
	{
		convexShape = new btConvexHullShape( vertices.front().m_floats, vertices.size(), sizeof(btVector3) );
	}
	return convexShape;
}

void CustomCollisionShapeData::DestroyTriangleShape()
{
	if( triangleShape )
		delete triangleShape;
	triangleShape = NULL;
	if( triangleData )
		delete triangleData;
	triangleData = NULL;
}

void CustomCollisionShapeData::DestroyConvexShape()
{
	if( convexShape )
		delete convexShape;
	convexShape = NULL;
}

CustomCollisionShapeData::CustomCollisionShapeData()
{
	friction = 0.5f;
	restitution = 0.5f;
	convexShape = NULL;
	triangleData = NULL;
	triangleShape = NULL;
}

CustomCollisionShapeData::~CustomCollisionShapeData()
{
	friction = 0.0f;
	restitution = 0.0f;
	DestroyConvexShape();
	DestroyTriangleShape();
}

#endif

