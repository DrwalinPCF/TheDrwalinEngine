
#ifndef CHARACTER_EVENT_CPP
#define CHARACTER_EVENT_CPP

#include "Character.h"

#include <Engine.h>

#include <Debug.h>

void Character::EventJump()
{
	//if( !isInAir )
	{
		if( body )
		{
			body->applyCentralImpulse( btVector3( 0, GetJumpVelocity(), 0 ) );
		}
	}
}

void Character::EventCrouch()
{
	if( walkMode != Character::WalkMode::CROUCH )
	{
		previousWalkMode = walkMode;
		walkMode = Character::WalkMode::CROUCH;
		SetScale( btVector3( 1.0, 0.5, 1.0 ) );
		SetCameraLocation( btVector3( 0.0, height * 0.5 * 0.5 * 0.9, 0.0 ) );
	}
}

void Character::EventStandUp()
{
	if( walkMode == Character::WalkMode::CROUCH )
	{
		walkMode = previousWalkMode;
		previousWalkMode = Character::WalkMode::WALK;
		SetScale( btVector3( 1.0, 1.0, 1.0 ) );
		SetCameraLocation( btVector3( 0.0, height * 0.5 * 0.9, 0.0 ) );
	}
}

void Character::EventBeginRun()
{
	switch( walkMode )
	{
	case Character::WalkMode::WALK:
		previousWalkMode = Character::WalkMode::WALK;
		walkMode = Character::WalkMode::RUN;
		break;
	case Character::WalkMode::STRAVAGE:
		previousWalkMode = Character::WalkMode::STRAVAGE;
		walkMode = Character::WalkMode::RUN;
		break;
	}
}

void Character::EventStopRun()
{
	if( walkMode == Character::WalkMode::RUN )
	{
		walkMode = previousWalkMode;
		previousWalkMode = Character::WalkMode::WALK;
	}
}

void Character::EventBeginStravage()
{
	switch( walkMode )
	{
	case Character::WalkMode::WALK:
		previousWalkMode = Character::WalkMode::WALK;
		walkMode = Character::WalkMode::STRAVAGE;
		break;
	case Character::WalkMode::RUN:
		previousWalkMode = Character::WalkMode::RUN;
		walkMode = Character::WalkMode::STRAVAGE;
		break;
	}
}

void Character::EventStopStravage()
{
	if( walkMode == Character::WalkMode::STRAVAGE )
	{
		walkMode = previousWalkMode;
		previousWalkMode = Character::WalkMode::WALK;
	}
}

void Character::EventMoveInDirection( const btVector3 & direction, bool flat )
{
	btVector3 dir = direction.normalized();
	
	if( flat )
		dir.m_floats[1] = 0.0f;
	
	if( body )
	{
		float velocity = GetMovementVelocity();
		if( body->getLinearVelocity().dot( dir ) < velocity )
		{
			body->applyCentralForce( dir * 60.0f * mass );
		}
	}
}

void Character::EventRotateCameraBy( const btVector3 & rotation )
{
	cameraRotation += rotation;
	
	CorrectCameraRotation();
	
	if( camera )
	{
		camera->SetRotation( cameraRotation );
	}
}

void Character::EventRotateCameraToLookAtPoint( const btVector3 & worldPoint, bool smooth )
{
	CorrectCameraRotation();
	
	btVector3 dstCameraRotation(0,0,0);
	{
		btVector3 dstLookingDirection = (worldPoint - GetCameraLocation()).normalized();
		btVector3 forwardVector = GetForwardVector().normalized();
		btVector3 flatForward = forwardVector;
		flatForward.m_floats[1] = 0.0f;
		flatForward.normalize();
		btVector3 flatDstLookingDirection = dstLookingDirection;
		flatDstLookingDirection.m_floats[1] = 0.0f;
		flatDstLookingDirection.normalize();
		
		float dot = flatForward.normalized().dot( flatDstLookingDirection.normalized() );
		dstCameraRotation.m_floats[1] = ( ( dot <= -1 || dot >= 1 ) ? 0.001f : acos( dot ) ) * ( flatDstLookingDirection.dot( GetFlatLeftVector() ) > 0.0f ? -1.0f : 1.0f );
		dot = forwardVector.normalized().dot( ( MakeTransformFromEuler( btVector3( 0, -dstCameraRotation.y(), 0 ) ) * dstLookingDirection ).normalized() );
		dstCameraRotation.m_floats[0] = ( ( dot <= -1 || dot >= 1 ) ? 0.001f : acos( dot ) ) * ( dstLookingDirection.y() > forwardVector.y() ? -1.0f : 1.0f );
	}
	
	if( smooth )
	{
		cameraRotation += dstCameraRotation * engine->GetDeltaTime() * 3.11f * ALLEGRO_PI;
	}
	else
	{
		cameraRotation += dstCameraRotation;
	}
	
	CorrectCameraRotation();
	
	if( camera )
	{
		camera->SetRotation( cameraRotation );
	}
}

#endif

