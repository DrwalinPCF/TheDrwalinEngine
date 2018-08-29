
#ifndef ENGINE_CPP
#define ENGINE_CPP

#include <Engine.h>

void Engine::UpdateObjects( const float deltaTime )
{
	//float time = al_get_time();
	
	while( !objectsQueuedToDestroy.empty() )
	{
		DeleteObject( objectsQueuedToDestroy.front() );
		objectsQueuedToDestroy.pop();
	}
	
	for( auto it = object.begin(); it != object.end(); ++it )
	{
		if( it->second )
		{
			it->second->Tick( deltaTime );
		}
	}
	
	//updateObjectTime = al_get_time() - time;
}

void Engine::QueueObjectToDestroy( SmartPtr<Object> ptr )
{
	if( ptr )
		objectsQueuedToDestroy.push( ptr->GetName() );
}

void Engine::QueueObjectToDestroy( const std::string & name )
{
	objectsQueuedToDestroy.push( name );
}

float Engine::GetDeltaTime()
{
	if( window )
	{
		return window->GetDeltaTime();
	}
	return 1.0f/60.0f;
}

CollisionShapeManager * Engine::GetCollisionShapeManager()
{
	return collisionShapeManager;
}

World * Engine::GetWorld()
{
	return world;
}

Window * Engine::GetWindow()
{
	return window;
}

void Engine::PauseSimulation()
{
	pausePhysics = true;
}

void Engine::ResumeSimulation()
{
	pausePhysics = false;
}

int Engine::CalculateNumberOfSimulationsPerFrame( const float deltaTime )
{
	return 1;
	float fps = 1.0 / deltaTime;
	if( fps >= 57.0 )
		return 100;
	if( fps >= 50.0 )
		return 90;
	if( fps >= 40.0 )
		return 60;
	if( fps >= 30.0 )
		return 30;
	if( fps >= 20.0 )
		return 5;
	return 1;
}

void Engine::ParallelToDrawTick( const float deltaTime )
{
	Tick( deltaTime );
}

void Engine::Tick( const float deltaTime )
{
	UpdateObjects( deltaTime );
	
	float time = al_get_time();
	if( window->IsParallelToDrawTickInUse() == false )
	{
		if( !pausePhysics )
		{
			world->Tick( deltaTime, CalculateNumberOfSimulationsPerFrame( deltaTime ) );		/////////////////////////////////////////////////////////////////////////
		}
	}
	physicsSimulationTime = al_get_time() - time;
}

Camera * Engine::GetCamera()
{
	return window->camera;
}

std::string Engine::GetAvailableObjectName( std::string name )
{
	if( object.find( name ) == object.end() )
		return name;
	for( int i = 0;; ++i )
		if( object.find( name+std::to_string(i) ) == object.end() )
			return name+std::to_string(i);
	return name;
}

void Engine::AttachCameraToObject( std::string name, btVector3 location )
{
	auto it = object.find( name );
	if( it != object.end() )
	{
		cameraParent = it->second;
	}
	else
	{
		cameraParent = NULL;
	}
	GetCamera()->SetPos( location );
}

void Engine::DrawBox( ALLEGRO_COLOR color, btTransform transform, btVector3 size )
{
	ALLEGRO_VERTEX vtx[8] = {
		/*   x   y   z   u   v  c  */
		{ -size.x(), -size.y(), -size.z(), 64, 64, color },
		{ -size.x(), -size.y(),  size.z(), 64,  0, color },
		{  size.x(), -size.y(), -size.z(),  0, 64, color },
		{  size.x(), -size.y(),  size.z(), 64,  0, color },
		{ -size.x(),  size.y(), -size.z(), 64, 64, color },
		{ -size.x(),  size.y(),  size.z(), 64,  0, color },
		{  size.x(),  size.y(), -size.z(),  0, 64, color },
		{  size.x(),  size.y(),  size.z(),  0,  0, color },
	};
	
	int indices[] = {
		0, 1, 2,
		1, 2, 3,
		
		4, 5, 6,
		5, 6, 7,
		
		0, 1, 4,
		1, 4, 5,
		
		2, 3, 6,
		3, 6, 7,
		
		0, 2, 4,
		2, 4, 6,
		
		1, 3, 5,
		3, 5, 7
	};
	
	window->camera->SetWorldTransform( transform, btVector3(1,1,1) );
	SmartPtr<Texture> tex = GetTexture("media/Textures/DebugTexturte.png");
	al_draw_indexed_prim( vtx, NULL, tex ? tex->GetBitmapPtr() : NULL, indices, 6*3*2, ALLEGRO_PRIM_TRIANGLE_LIST );
}

bool Engine::SetCustomModelName( std::string name, SmartPtr<Model> mdl )
{
	auto it = model.find( name );
	if( it == model.end() )
	{
		model[name] = mdl;
		return true;
	}
	return false;
}

SmartPtr<Model> Engine::LoadModel( std::string name )
{
	auto it = model.find( name );
	if( it != model.end() )
	{
		if( it->second )
			return it->second;
		else
			model.erase( it );
	}
	else
	{
		SmartPtr<Model> mdl;
		mdl = new Model;
		if( mdl->LoadFromFile( this, name ) == false )
		{
			mdl.Delete();
			return SmartPtr<Model>();
		}
		else
		{
			model[name] = mdl;
			return mdl;
		}
	}
	return SmartPtr<Model>();
}

SmartPtr<Texture> Engine::GetTexture( std::string name )
{
	auto it = texture.find( name );
	if( it != texture.end() )
	{
		if( it->second )
		{
			return it->second;
		}
		else
			texture.erase( it );
	}
	else
	{
		SmartPtr<Texture> tex;
		tex = new Texture;
		if( tex->Load( name, Texture::LINEAR | Texture::MIPMAP ) == false )
		{
			DEBUG( name + ": not done" );
			tex.Delete();
			return SmartPtr<Texture>();
		}
		else
		{
			texture[name] = tex;
			return tex;
		}
	}
	DEBUG( name + ": not done" );
	return SmartPtr<Texture>();
}

SmartPtr<Model> Engine::GetModel( std::string name )
{
	auto it = model.find( name );
	if( it != model.end() )
	{
		if( it->second )
			return it->second;
		else
			model.erase( it );
	}
	else
	{
		SmartPtr<Model> mdl;
		mdl = new Model;
		if( mdl->LoadFromFile( this, name ) == false )
		{
			mdl.Delete();
			return SmartPtr<Model>();
		}
		else
		{
			model[name] = mdl;
			return mdl;
		}
	}
	SmartPtr<Model> ret;
	return ret;
}

SmartPtr<Object> Engine::GetObject( std::string name )
{
	auto it = object.find( name );
	if( it != object.end() )
	{
		if( it->second )
		{
			return it->second;
		}
		else
		{
			object.erase( it );
		}
	}
	SmartPtr<Object> ret;
	return ret;
}

void Engine::DeleteObject( std::string name )
{
	auto it = object.find( name );
	if( it != object.end() )
	{
		if( it->second == cameraParent )
			cameraParent = NULL;
		
		if( it->second )
		{
			world->RemoveBody( name );
			it->second.Delete();
		}
		
		object.erase( it );
	}
}



void Engine::DrawCrosshair()
{
	float width, height;
	width = window->GetWidth();
	height = window->GetHeight();
	
	float midx = width/2.0, midy = height/2.0;
	
	ALLEGRO_COLOR color = al_map_rgba( 230, 110, 5, 128 );
	
	float a = 2, b = 3;
	
	al_draw_line( midx-a, midy-b, midx+a, midy-b, color, 2.0 );
	al_draw_line( midx-a, midy+b, midx+a, midy+b, color, 2.0 );
	
	al_draw_line( midx-b, midy-a, midx-b, midy+a, color, 2.0 );
	al_draw_line( midx+b, midy-a, midx+b, midy+a, color, 2.0 );
}

void Engine::Draw2D()
{
	float time = al_get_time();
	
	/*
	output.SetColor( al_map_rgb( 255, 255, 255 ) );
	output.SetWorkSpace( 20, 20, this->GetMaxBorderWidth()-20, this->GetMaxBorderHeight()-20 );
	output.Goto( 1, 1 );
	output.Print( this->GetCurrentStringToEnter().c_str() );
	*/
	
	window->output->SetWorkSpace( 5, 10, 80, 80 );
	window->output->Goto( 5, 10 );
	window->output->SetColor( al_map_rgb( 0, 255, 255 ) );
	window->output->Print( "DeltaTime: " );
	window->output->Print( window->GetDeltaTime() );
	
	
	//window->output->SetWorkSpace( 5, 10, 80, 80 );
	//window->output->Goto( 5, 10 );
	//window->output->SetColor( al_map_rgb( 0, 255, 255 ) );
	window->output->Print( "\nFPS: " );
	window->output->Print( window->GetSmoothFps() );
	/*
	window->output->Print( "\n" );
	window->output->Print( (int)al_get_time() );
	
	window->output->Print( "\n\nCamera position: " );
	window->output->Print( window->camera->GetLocation().x() );
	window->output->Print( " : " );
	window->output->Print( window->camera->GetLocation().y() );
	window->output->Print( " : " );
	window->output->Print( window->camera->GetLocation().z() );
	*/
	window->output->Print( "\nObjects: " );
	window->output->Print( int(object.size()) );
	
	if( false )
	{
		window->output->Print( "\n\nPointing at object: " );
		SmartPtr<Object> player = this->GetObject("Player");
		btVector3 begin, end, point, normal;
		
		begin = this->GetCamera()->GetLocation();
		end = begin + ( this->GetCamera()->GetForwardVector() * 100.0 );
		
		SmartPtr<Object> temp = this->RayTrace( begin, end, Engine::RayTraceChannel::COLLIDING, point, normal, { player } );
		
		if( temp )
		{
			window->output->Print( temp->GetName() );
		}
		else
		{
			window->output->Print( "(NULL)" );
		}
	}
	
	if( false )
	{
		window->output->Print( "\ncollisionShape: " );
		window->output->Print( int(collisionShapeManager->collisionShape.size()) );
		window->output->Print( "\ncollisionShapeRev: " );
		window->output->Print( int(collisionShapeManager->collisionShapeRev.size()) );
		window->output->Print( "\nnumberOfReferencesToShape: " );
		window->output->Print( int(collisionShapeManager->numberOfReferencesToShape.size()) );
		
		window->output->Print( "\n\nmodelPointerCustomCollisionData: " );
		window->output->Print( int(collisionShapeManager->modelPointerCustomCollisionData.size()) );
		window->output->Print( "\nmodelCustomCollisionData: " );
		window->output->Print( int(collisionShapeManager->modelCustomCollisionData.size()) );
		window->output->Print( "\ncustomCollisionShapeData: " );
		window->output->Print( int(collisionShapeManager->customCollisionShapeData.size()) );
		window->output->Print( "\ncustomCollisionShape: " );
		window->output->Print( int(collisionShapeManager->customCollisionShape.size()) );
		window->output->Print( "\ncustomCollisionShapeName: " );
		window->output->Print( int(collisionShapeManager->customCollisionShapeName.size()) );
	}
	
	if( false )
	{
		window->output->SetWorkSpace( 5, 2, 80, 80 );
		
		float skippedTime = window->GetSkippedTime();
		float sumTime = /*guiDrawTime + sceneDrawTime + physicsSimulationTime +*/ window->GetDeltaTime();
		float otherTime = sumTime - ( guiDrawTime + sceneDrawTime + physicsSimulationTime + skippedTime );
		float step = sumTime / 40;
		float t;
		
		window->output->SetColor( al_map_rgb( 255, 0, 0 ) );
		window->output->Goto( 5, 2 );
		for( t = 0.0f; t < guiDrawTime; t += step )
			window->output->Print( "#" );
		
		window->output->SetColor( al_map_rgb( 0, 255, 0 ) );
		for( t = 0.0f; t < sceneDrawTime; t += step )
			window->output->Print( "#" );
		
		window->output->SetColor( al_map_rgb( 0, 0, 255 ) );
		for( t = 0.0f; t < physicsSimulationTime; t += step )
			window->output->Print( "#" );
		
		window->output->SetColor( al_map_rgb( 255, 255, 255 ) );
		for( t = 0.0f; t < skippedTime; t += step )
			window->output->Print( "#" );
		
		window->output->SetColor( al_map_rgb( 128, 128, 128 ) );
		for( t = 0.0f; t < otherTime; t += step )
			window->output->Print( "#" );
		
		window->output->SetColor( al_map_rgb( 255, 0, 0 ) );
		window->output->Print( "\n guiDrawTime: " );
		window->output->Print( guiDrawTime );
		
		window->output->SetColor( al_map_rgb( 0, 255, 0 ) );
		window->output->Print( "\n sceneDrawTime: " );
		window->output->Print( sceneDrawTime );
		
		window->output->SetColor( al_map_rgb( 0, 0, 255 ) );
		window->output->Print( "\n physicsSimulationTime: " );
		window->output->Print( physicsSimulationTime );
		
		window->output->SetColor( al_map_rgb( 255, 255, 255 ) );
		window->output->Print( "\n skippedTime: " );
		window->output->Print( skippedTime );
		
		window->output->SetColor( al_map_rgb( 128, 128, 128 ) );
		window->output->Print( "\n otherTime: " );
		window->output->Print( otherTime );
	}
	
	DrawCrosshair();
	
	guiDrawTime = al_get_time() - time;
}

void Engine::Draw3D()
{
	float time = al_get_time();
	
	if( cameraParent )
		GetCamera()->SetCameraTransform( cameraParent->GetTransform() );
	
	GetCamera()->UpdateViewPlanes();
	
	for( auto it = object.begin(); it != object.end(); ++it )
	{
		if( it->second )
		{
			if( GetCamera()->IsObjectInView( it->second ) )
			{
				it->second->Draw();
			}
		}
	}
	
	sceneDrawTime = al_get_time() - time;
}

void Engine::BeginLoop()
{
	pausePhysics = false;
	window->BeginLoop();
}

void Engine::Init( const char * windowName, const char * iconFile, int width, int height, bool fullscreen )
{
	Destroy();
	event = new Event;
	world = new World;
	window = new Window;
	world->Init();
	window->Init( this, windowName, iconFile, width, height, fullscreen );
	window->SetEventResponser( event );
	event->Init();
	event->SetEngine( this );
	
	window->HideMouse();
	window->LockMouse();
	
	collisionShapeManager = new CollisionShapeManager;
	
	//window->UseParallelThreadToDraw();
}

void Engine::Destroy()
{
	cameraParent = NULL;
	
	for( auto it = object.begin(); it != object.end(); ++it )
	{
		if( it->second )
		{
			assert( it->second );
			world->RemoveBody( it->first );
			it->second.Delete();
		}
	}
	object.clear();
	
	for( auto it = model.begin(); it != model.end(); ++it )
	{
		if( it->second )
		{
			assert( it->second );
			it->second->Destroy();
			it->second.Delete();
		}
	}
	model.clear();
	
	for( auto it = texture.begin(); it != texture.end(); ++it )
	{
		if( it->second )
		{
			assert( it->second );
			it->second->Destroy();
			it->second.Delete();
		}
	}
	texture.clear();
	
	if( window )
	{
		assert( window != NULL );
		window->Destroy();
		delete window;
		window = NULL;
	}
	
	if( world )
	{
		assert( world != NULL );
		world->Destroy();
		delete world;
		world = NULL;
	}
	
	if( event )
	{
		assert( event != NULL );
		delete event;
		event = NULL;
	}
	
	if( collisionShapeManager )
	{
		assert( collisionShapeManager != NULL );
		collisionShapeManager->Destroy();
		delete collisionShapeManager;
		collisionShapeManager = NULL;
	}
	
	pausePhysics = false;
}

Engine::Engine()
{
	event = NULL;
	world = NULL;
	window = NULL;
	pausePhysics = true;
	collisionShapeManager = NULL;
	
	guiDrawTime = 0.01;
	sceneDrawTime = 0.01;
	physicsSimulationTime = 0.01;
}

Engine::~Engine()
{
	Destroy();
}

#endif

