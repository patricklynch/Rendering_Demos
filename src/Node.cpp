#include "Node.h"

using namespace ly;

using namespace ci;

static int nodeIdCounter = 0;

Node::Node( NodeType type ) : parent( NULL ), mIsUnlit( false ), mShouldFaceCamera( false ), inverted( false )
{
	mNodeType = type;
	mNodeId = nodeIdCounter++;
	mTransform = mLastTransform = Matrix44f::identity();
	scale = Vec3f::one();
	
	if ( mNodeType == NodeTypeSprite )
		mShouldFaceCamera = true;
	
	spriteSize = Vec2f( 10, 10 );
	
	// Set default colors
	colors[ MaterialDiffuse ]  = ColorA::white();
	colors[ MaterialSpecular ]  = ColorA::white();
}

Node::~Node() {}

ci::Vec3f Node::globalPosition()
{
	return Vec3f( transform().getTranslate().xyz() );
}

void Node::draw()
{
	switch( mNodeType )
	{
		case NodeTypeSphere:
			gl::drawSphere( Vec3f::zero(), 1.0f, 30 );
			break;
			
		case NodeTypeCube:
			gl::drawCube( Vec3f::zero(), Vec3f::one() );
			break;
			
		case NodeTypeSprite:
			gl::drawSolidRect( Rectf( -spriteSize.x * 0.5f, -spriteSize.y * 0.5f, spriteSize.x * 0.5f, spriteSize.y * 0.5f ) );
			break;
			
		default:break;
	}
}

void Node::faceCamera()
{
	/*Vec3f forward = mRenderer->currentCamera()->globalPosition() - globalPosition();
	mTransform *= ci::Matrix44f::alignZAxisWithTarget( forward, ci::Vec3f::yAxis() );
	mTransform.rotate( Vec3f::zAxis(), toRadians( spriteRotation ) );
	mLastTransform = mTransform;*/
}

void Node::update( const float deltaTime )
{
	mTransform = Matrix44f::identity();
	if ( parent != NULL )
		mTransform = parent->transform();
	mTransform.translate( position );
	mTransform.rotate( rotation * M_PI / 180.0f );
	mTransform.scale( scale );
	mLastTransform = mTransform;
	
	if ( mShouldFaceCamera ) faceCamera();
}

void Node::setTexture( gl::Texture* texture )
{
	textures[ MaterialDiffuse ] = texture;
}

ci::gl::Texture* Node::texture()
{
	return textures[ MaterialDiffuse ];
}

void Node::setColor( ci::ColorA color )
{
	colors[ MaterialDiffuse ] = color;
}

ci::ColorA Node::color()
{
	return colors[ MaterialDiffuse ];
}