#include "EightBitApp.h"
#include "cinder/ip/Hdr.h"

using namespace ly;

using namespace ci;
using namespace ci::app;
using namespace std;

void EightBitApp::prepareSettings(cinder::app::AppBasic::Settings *settings)
{
	settings->setWindowSize( 1024, 768 );
	//settings->setFrameRate( 10.0f );
}

void EightBitApp::setup()
{
	mPrevElapsedSeconds = 0.0f;
	
	// Fbo
	gl::Fbo::Format fboFormat;
	fboFormat.enableMipmapping();
	fboFormat.setMagFilter( GL_NEAREST );
	float resolution = 1.0f;
	mFbo = gl::Fbo( app::getWindowWidth() / resolution, app::getWindowHeight() / resolution, fboFormat );
	
	mDepthMap = createDepthMap( mFbo.getWidth(), mFbo.getHeight() );
	
	// Shaders
	mBgShader				= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "shader.frag" ) );
	mShader					= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "shader_lit.frag" ) );
	mToonShader				= gl::GlslProg( loadResource( "passthru.vert"),			loadResource( "toon.frag" ) );
	mDepthShader			= gl::GlslProg( loadResource( "depthmap.vert"),			loadResource( "depthmap.frag" ) );
	mOutlineShader			= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "outline.frag" ) );
	mSsaoShader				= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "ssao.frag" ) );
	mSketchShader			= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "sketch.frag" ) );
	
	// Textures
	gl::Texture::Format format;
	format.setWrap( GL_REPEAT, GL_REPEAT );
	mSketchedWord.first		= gl::Texture( loadImage( loadResource( "wood.png" ) ),			format );
	mSketchedEarth.first	= gl::Texture( loadImage( loadResource( "earth.png" ) ),		format );
	
	createSketch( mSketchedEarth, getWindowSize() );
	createSketch( mSketchedWord, getWindowSize() );
	
	mBackgroundTexture		= gl::Texture( loadImage( loadResource( "background.jpg" ) ),	format );
	
	// Camera setup
	mCamera = CameraPersp();
	
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
	//node->setTexture( &mSketchedEarth.second.getTexture() );
	node->setTexture( &mSketchedEarth.first );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA(1,1,1,1);
	mNodes.push_back( node );
	
	node = new Node( NodeTypeCube );
	node->scale = Vec3f( 300, 255, 300 );
	node->position = Vec3f( 0, 40, 0 );
	node->inverted = true;
	//node->setTexture( &mSketchedWord.second.getTexture() );
	node->setTexture( &mSketchedWord.first );
	node->colors[ MaterialSpecular ] = ColorA::white() * 0.5f;
	node->colors[ MaterialDiffuse ] = ColorA( .9, .7, .5, 1 );
	mNodes.push_back( node );
	
	// To change all colors to white ( looks nice with SSAO )
	if ( true ) {
		for( std::vector<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
			(*iter)->colors[ MaterialDiffuse ] = ColorA::white() * 0.8f;
			(*iter)->textures[ MaterialDiffuse ] = NULL;
		}
	}
	
	// To scale evertying way, way down, so that you can use a small FOV and maitain a good enough
	// near/far planes for good depth values
	if ( false ) {
		for( std::vector<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
			(*iter)->scale *= 0.01f;
			(*iter)->position *= 0.01f;
		}
	}
}

void EightBitApp::createSketch( std::pair<ci::gl::Texture, ci::gl::Fbo>& sketchPair, ci::Vec2f size )
{
	sketchPair.second = gl::Fbo( size.x, size.y );
	sketchPair.second.bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	gl::setMatricesWindow( app::getWindowSize(), false );
	mSketchShader.bind();
	sketchPair.first.bind( 0 );
	mSketchShader.uniform( "imageTexture", 0 );
	gl::drawSolidRect( Rectf( 0, 0, size.x, size.y ) );
	sketchPair.first.unbind( 0 );
	mSketchShader.unbind();
	sketchPair.second.unbindFramebuffer();
}

void EightBitApp::mouseDown( MouseEvent event ) {}

float angle = 0.0f;

void EightBitApp::update()
{
    float currentTime = app::getElapsedSeconds();
    float deltaTime = currentTime - mPrevElapsedSeconds;
    mPrevElapsedSeconds = currentTime;
	
	for( std::vector<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ ) {
		//(*iter)->rotation += Vec3f( 10, 10, 10 ) * deltaTime;
		(*iter)->update( deltaTime );
	}
	
	float rotationSpeed = 0.08f;
	mCamera.lookAt( Vec3f( ci::math<float>::sin( angle += rotationSpeed * deltaTime ) * 400,
						   ci::math<float>::sin( angle += rotationSpeed * deltaTime ) * 100,
						   ci::math<float>::cos( angle += rotationSpeed * deltaTime ) * 400 ) , Vec3f::zero(), Vec3f::yAxis() );
}

void EightBitApp::draw()
{
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	// Near and far clipping are important for depth and thus have an effect on outlines and SSAO
	//mCamera.setPerspective( 0.1f, app::getWindowAspectRatio(), 100.0f, 600.0f );
	mCamera.setPerspective( 30.0f, app::getWindowAspectRatio(), 100.0f, 2000.0f );
	gl::setMatricesWindow( mFbo.getWidth(), mFbo.getHeight(), true );
	
	gl::GlslProg* currentShader;
	
	mDepthMap->bindFramebuffer();
	currentShader = &mDepthShader;
	currentShader->bind();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	//glEnable( GL_POLYGON_OFFSET_FILL );
	//glPolygonOffset( 0.6, 0.6 );
	glClear( GL_DEPTH_BUFFER_BIT );
	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, mDepthMap->getWidth(), mDepthMap->getHeight() );
	currentShader->uniform( "projectionMatrix", mCamera.getProjectionMatrix() );
	currentShader->uniform( "modelViewMatrix", mCamera.getModelViewMatrix() );
	drawNodes( mNodes, currentShader );
	glDisable( GL_CULL_FACE );
	//glDisable( GL_POLYGON_OFFSET_FILL );
	glPopAttrib();
	currentShader->unbind();
	mDepthMap->unbindFramebuffer();
	
	//currentShader = &mToonShader;
	currentShader = &mShader;
	{
		mFbo.bindFramebuffer();
		gl::clear( Color( .3, .3, .3 ) );
		currentShader->bind();
		mDepthMap->bindDepthTexture( 1 );
		currentShader->uniform( "depthMap", 1 );
		currentShader->uniform( "modelViewMatrix", mCamera.getModelViewMatrix() );
		currentShader->uniform( "projectionMatrix", mCamera.getProjectionMatrix() );
		currentShader->uniform( "ambientColor", ColorA(1,1,1,1) * 0.3f );
		currentShader->uniform( "eyePos", mCamera.getEyePoint() );
		currentShader->uniform( "lightPos", Vec3f(.5,1,.5) * 1000.0f );
		mDepthMap->unbindTexture();
		drawNodes( mNodes, currentShader );
	}
	currentShader->unbind();
	
	/*currentShader = &mBgShader;
	currentShader->bind();
	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );
	mBackgroundTexture.bind( 0 );
	currentShader->uniform( "modelViewMatrix", mCamera.getModelViewMatrix() );
	currentShader->uniform( "projectionMatrix", mCamera.getProjectionMatrix() );
	currentShader->uniform( "tileResolution", 5.0f );
	currentShader->uniform( "mainColor", ColorA( 1, 1, 1, 1 ) * .3f );
	currentShader->uniform( "mainTexture", 0 );
	currentShader->uniform( "transformMatrix", Matrix44f::identity() );
	gl::drawCube( mCamera.getEyePoint(), Vec3f::one() * 800.0f );
	glDisable( GL_CULL_FACE );
	mBackgroundTexture.unbind();
	currentShader->unbind();*/
	
	mFbo.unbindFramebuffer();
	
	// Draw outlines
	if ( false ) {
		currentShader = &mOutlineShader;
		currentShader->bind();
		gl::setMatricesWindow( app::getWindowSize(), false );
		mFbo.bindTexture( 0 );
		currentShader->uniform( "mainTexture", 0 );
		mDepthMap->bindDepthTexture( 1 );
		currentShader->uniform( "mainColor", ColorA( 1, 1, 1, 1 ) );
		currentShader->uniform( "depthTexture", 1 );
		currentShader->uniform( "outlineThickness", 1.2f );
		currentShader->uniform( "outlineColor", ColorA(0, 0, 0, 1 ) );
		currentShader->uniform( "transformMatrix", Matrix44f::identity() );
		gl::drawSolidRect( Rectf( app::getWindowBounds() ) );
		mFbo.unbindTexture();
		mDepthMap->unbindTexture();
		currentShader->unbind();
	}
	
	// Draw SSAO
	if ( true ) {
		currentShader = &mSsaoShader;
		currentShader->bind();
		gl::setMatricesWindow( app::getWindowSize(), false );
		mFbo.bindTexture( 0 );
		currentShader->uniform( "mainTexture", 0 );
		mDepthMap->bindDepthTexture( 1 );
		currentShader->uniform( "mainColor", ColorA( 1, 1, 1, 1 ) );
		currentShader->uniform( "depthTexture", 1 );
		currentShader->uniform( "spread", 0.002f );
		currentShader->uniform( "samples", 7 );
		currentShader->uniform( "strength", 0.5f );
		currentShader->uniform( "depthRange", .5f );
		currentShader->uniform( "transformMatrix", Matrix44f::identity() );
		gl::drawSolidRect( Rectf( app::getWindowBounds() ) );
		mFbo.unbindTexture();
		mDepthMap->unbindTexture();
		currentShader->unbind();
	}
}

ci::gl::Fbo* EightBitApp::createDepthMap( int width, int height )
{
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_R32F );
	gl::Fbo* fbo = new gl::Fbo(width, height, format);
	fbo->bindFramebuffer();
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
	glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	fbo->unbindFramebuffer();
	return fbo;
}

void EightBitApp::drawNodes( std::vector<Node*>& nodes, ci::gl::GlslProg* currentShader )
{
	for( std::vector<Node*>::iterator iter = mNodes.begin(); iter != mNodes.end(); iter++ )
	{
		Node* node = *iter;
		if ( node->texture() ) {
			node->texture()->bind( 0 );
			currentShader->uniform( "mainTexture", 0 );
		}
		currentShader->uniform( "nInvert", node->inverted ? -1.0f : 1.0f );
		currentShader->uniform( "textured", node->texture() != NULL );
		currentShader->uniform( "tileResolution", 1.0f );
		currentShader->uniform( "diffuseColor", node->colors[ MaterialDiffuse ]);
		currentShader->uniform( "specularColor", node->colors[ MaterialSpecular ]);
		currentShader->uniform( "transformMatrix", node->transform() );
		glEnable( GL_CULL_FACE );
		if ( node->inverted ) glCullFace( GL_FRONT );
		else glCullFace( GL_BACK );
		node->draw();
		glDisable( GL_CULL_FACE );
		if ( node->texture() )
			node->texture()->unbind( 0 );
	}
}

CINDER_APP_BASIC( EightBitApp, RendererGl )
