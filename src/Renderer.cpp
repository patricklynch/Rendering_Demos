#include "Renderer.h"

#include "cinder/Camera.h"
#include "cinder/gl/gl.h"
#include "cinder/app/App.h"

using namespace ly;

using namespace ci;
using namespace ci::app;

ly::Renderer* ly::Renderer::sInstance = NULL;

ly::Renderer* ly::Renderer::get()
{
	if ( !sInstance ) sInstance = new ly::Renderer();
	return sInstance;
}

ly::Renderer::Renderer()
{
	mDepthShader		= gl::GlslProg( loadResource( "depthmap.vert"),			loadResource( "depthmap.frag" ) );
	mDebugDepthSahder	= gl::GlslProg( loadResource( "passthru_post.vert"),	loadResource( "debug_depth.frag" ) );
}

ci::gl::Fbo* ly::Renderer::createDepthMap( int width, int height )
{
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_R32F );
	gl::Fbo* fbo = new gl::Fbo( width, height, format );
	fbo->bindFramebuffer();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
	glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
	fbo->unbindFramebuffer();
	return fbo;
}

void ly::Renderer::drawDepthMap( ci::Vec2f size, ci::gl::Fbo* depthMap, std::vector<Node*>& nodes, GLenum cullingMode )
{
	if ( mCamera == NULL ) return;
	
	gl::enableDepthWrite();
	gl::enableDepthRead();
	
	gl::setMatricesWindow( size, true );
	
	depthMap->bindFramebuffer();
	mDepthShader.bind();
	gl::clear( ColorA( 0, 0, 0, 0 ) );
	glEnable( cullingMode );
	glCullFace( GL_BACK );
	glClear( GL_DEPTH_BUFFER_BIT );
	glPushAttrib( GL_VIEWPORT_BIT );
	glViewport( 0, 0, depthMap->getWidth(), depthMap->getHeight() );
	mDepthShader.uniform( "projectionMatrix", mCamera->getProjectionMatrix() );
	mDepthShader.uniform( "modelViewMatrix", mCamera->getModelViewMatrix() );
	drawNodes( nodes, &mDepthShader, cullingMode );
	glDisable( GL_CULL_FACE );	glPopAttrib();
	mDepthShader.unbind();
	depthMap->unbindFramebuffer();
}

void ly::Renderer::drawNodes( std::vector<Node*>& nodes, ci::gl::GlslProg* currentShader, GLenum cullingMode )
{
	if ( mCamera == NULL || currentShader == NULL ) return;
	
	for( std::vector<Node*>::iterator iter = nodes.begin(); iter != nodes.end(); iter++ )
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
		if ( node->inverted ) {
			if ( cullingMode == GL_FRONT ) glCullFace( GL_BACK );
			glCullFace( GL_FRONT );
		}
		else glCullFace( cullingMode );
		node->draw();
		glDisable( GL_CULL_FACE );
		
		if ( node->texture() )
			node->texture()->unbind( 0 );
	}
}

void ly::Renderer::debugDrawDepthMap( ci::gl::Fbo* depthMap )
{
	gl::GlslProg* currentShader = &mDebugDepthSahder;
	currentShader->bind();
	gl::setMatricesWindow( app::getWindowSize(), false );
	depthMap->bindDepthTexture( 0 );
	currentShader->uniform( "depthBufferTexture", 0 );
	currentShader->uniform( "rangeMultiplier", 2.0f );
	gl::drawSolidRect( Rectf( app::getWindowBounds() ) );
	depthMap->unbindTexture();
	currentShader->unbind();
}