#pragma once

#include "Demo.h"
#include "Node.h"

#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

namespace ly {
	
class SsaoDemo : public Demo {
public:
	SsaoDemo();
	virtual ~SsaoDemo();
	
	virtual void draw();
	virtual void update( const float deltaTime );
	
private:
	ci::gl::Fbo* mDepthMap;
	ci::gl::Fbo mSsaoFbo;
	
	ci::gl::GlslProg mObjectShader;
	ci::gl::GlslProg mSsaoShader;
};

}