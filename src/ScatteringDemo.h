#pragma once

#include "Demo.h"

namespace ly {

class ScatteringDemo : public Demo {
public:
	ScatteringDemo();
	virtual ~ScatteringDemo();
	
	virtual void draw();
	virtual void update( const float deltaTime );
	
private:
	ci::gl::Fbo* mDepthMapBack;
	ci::gl::Fbo* mDepthMapFront;
	ci::gl::GlslProg mScatterShader;
	
};

}