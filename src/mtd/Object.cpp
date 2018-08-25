
#ifndef OBJECT_CPP
#define OBJECT_CPP

#include <Object.h>
#include <Engine.h>

void Object::SetScale( btVector3 scale )
{
	this->scale = scale;
	if( body )
	{
		body->getCollisionShape()->setLocalScaling( scale );
		engine->GetWorld()->UpdateColliderForObject( body );
		CalculateRadius();
	}
}

btVector3 Object::GetScale()
{
	return scale;
}

btTransform Object::GetTransform()
{
	btTransform transform;
	if( body )
		body->getMotionState()->getWorldTransform( transform );
	return transform;
}

btRigidBody * Object::GetBody()
{
	return body;
}

void Object::SetBody( btRigidBody * body )
{
	this->body = body;
	CalculateRadius();
}

void Object::Draw()
{
	if( model )
	{
		engine->GetCamera()->SetWorldTransform( GetTransform(), scale );
		model->Draw();
	}
}

void Object::SetModel( Model * model )
{
	this->model = model;
}

btVector3 Object::GetLocation()
{
	if( body )
	{
		btTransform transform;
		body->getMotionState()->getWorldTransform( transform );
		return transform.getOrigin();
	}
	return btVector3();
}

float Object::GetRadius()
{
	return boundingSphereRadius;
}

void Object::CalculateRadius()
{
	if( body )
	{
		btVector3 min, max, origin;
		body->getAabb( min, max );
		origin = GetLocation();
		min = (min-origin).absolute();
		max = (max-origin).absolute();
		
		for(int i = 0; i < 3; ++i )
			if( min.m_floats[0]> max.m_floats[0] )
				max.m_floats[0] = min.m_floats[0];
		
		boundingSphereRadius = max.length();
	}
	else
	{
		boundingSphereRadius = 0.01f;
	}
}

void Object::SetRayTraceChannel( int src )
{
	rayTraceChannel = src;
}

int Object::GetRayTraceChannel()
{
	return rayTraceChannel;
}

std::string Object::GetName() const
{
	return name;
}

Object::Object( Engine * engine, std::string name, btRigidBody * body )
{
	this->engine = engine;
	this->name = name;
	this->body = body;
	model = NULL;
	scale = btVector3(1,1,1);
	CalculateRadius();
	rayTraceChannel = Engine::RayTraceChannel::COLLIDING | Engine::RayTraceChannel::NOT_TRANSPARENT;
}

Object::Object()
{
	engine = NULL;
	name = "";
	body = NULL;
	model = NULL;
	scale = btVector3(1,1,1);
	boundingSphereRadius = 1.0f;
	rayTraceChannel = Engine::RayTraceChannel::COLLIDING | Engine::RayTraceChannel::NOT_TRANSPARENT;
}

Object::~Object()
{
	if( body )
	{
		if( body->getMotionState() )
		{
			assert( body->getMotionState() != NULL );
			delete body->getMotionState();
			body->setMotionState( NULL );/////////////////////////////////////////////////////////////////////////////////
		}
		if( body->getCollisionShape() )
		{
			engine->GetCollisionShapeManager()->RemoveShape( body->getCollisionShape() );
			body->setCollisionShape( NULL );/////////////////////////////////////////////////////////////////////////////////
		}
		assert( body != NULL );
		delete body;
		body = NULL;
	}
	name = "";
	engine = NULL;
	model = NULL;
	scale = btVector3(0,0,0);
	boundingSphereRadius = 0.0f;
}

#endif

