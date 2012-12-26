#pragma once

#include "Node.h"

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"

#include <boost/function.hpp>

namespace ly {
	
class Renderer {
public:
	static Renderer* get();
	virtual ~Renderer() {}
	
	ci::gl::Fbo* createDepthMap( int width, int height );
	void drawDepthMap( ci::Vec2f size, ci::gl::Fbo* depthMap, std::vector<Node*>& nodes, GLenum cullingMode = GL_BACK );
	void drawNodes( std::vector<Node*>& nodes, ci::gl::GlslProg* currentShader, GLenum cullingMode = GL_BACK );
	void setCamera( ci::CameraPersp* camera ) { mCamera = camera; }
	void debugDrawDepthMap( ci::gl::Fbo* depthMap );
	ci::Surface32f createBlurSurface( int width, int height );
	
private:
	Renderer();
	static ly::Renderer* sInstance;
	
	ci::gl::GlslProg mDepthShader;
	ci::gl::GlslProg mDebugDepthSahder;
	
	ci::gl::Fbo* mDepthMapFbo;
	
	ci::CameraPersp* mCamera;
};

}