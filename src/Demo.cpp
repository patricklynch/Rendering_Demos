#include "Demo.h"

#include "cinder/ImageIo.h"
#include "cinder/app/App.h"

using namespace ly;
using namespace ci;

Demo::Demo()
{
	mRenderer = ly::Renderer::get();
	mCamera = ly::Camera::get();
}

Demo::~Demo()
{
	while( mNodes.size() > 0 ) {
		delete *mNodes.begin();
		mNodes.erase( mNodes.begin() );
	}
	unloadAllTextures();
}

void Demo::addNode( Node* node )
{
	mNodes.push_back( node );
}

void Demo::unloadAllTextures()
{
	for( std::map<std::string, ci::gl::Texture*>::iterator iter = mTextures.begin(); iter != mTextures.end(); iter++)
		delete iter->second;
	mTextures.clear();
}

ci::gl::Texture* Demo::loadTexture( std::string path )
{
	if ( !mTextures[ path ] )
		mTextures[ path ] = new ci::gl::Texture( ci::loadImage( ci::app::loadResource( path ) ) );
	return mTextures[ path ];
}

void Demo::removeNode( Node* node )
{
	std::vector<Node*>::iterator match = find( mNodes.begin(), mNodes.end(), node );
	if ( match != mNodes.end() ) mNodes.erase( match );
}

void Demo::addNodes( std::vector<Node*>& nodes )
{
	std::vector<Node*>::const_iterator iter;
	for( iter = nodes.begin(); iter != nodes.end(); iter++ )
		addNode( *iter );
}

void Demo::update( const float deltaTime )
{
	mCamera->update( deltaTime );
	std::vector<Node*>::const_iterator iter;
	for( iter = mNodes.begin(); iter != mNodes.end(); iter++ )
		(*iter)->update( deltaTime );
}

void Demo::draw()
{
	
}

void Demo::createSomeObjects()
{
	Node* node;
	
	node = new Node( NodeTypeSphere );
	node->scale = Vec3f::one() * 30.0f;
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(1,0,0,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeCube );
	node->scale = Vec3f::one() * 50.0f;
	node->position = Vec3f( 0, 0, 100 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(.5,.8,.8,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeCube );
	node->scale = Vec3f::one() * 50.0f;
	node->position = Vec3f( 0, 0, -100 );
	node->rotation = Vec3f( 20, 50, 0 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(.5,.5,1,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeSphere );
	node->scale = Vec3f::one() * 25.0f;
	node->position = Vec3f( 100, 0, 0 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(.7,.7,.5,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeSphere );
	node->scale = Vec3f::one() * 25.0f;
	node->position = Vec3f( -100, 0, 0 );
	node->rotation = Vec3f( 20, 50, 0 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(.5,.5,1,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeCube );
	node->scale = Vec3f::one() * 100.0f;
	node->position = Vec3f( 0, -50, 0 );
	node->rotation = Vec3f( 0, 10, -10 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(.5,1,.5,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeSphere );
	node->scale = Vec3f::one() * 100.0f;
	node->position = Vec3f( 0, -100, 0 );
	node->rotation = Vec3f( 0, 100, 100 );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(1,1,1,1);
	mNodes.push_back( node );
	mLargeSphere = node;
	
	node = new Node( NodeTypeCube );
	node->scale = Vec3f( 300, 255, 300 );
	node->position = Vec3f( 0, 40, 0 );
	node->inverted = true;
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA( .9, .7, .5, 1 );
	mNodes.push_back( node );
	mBackgroundCube = node;
}