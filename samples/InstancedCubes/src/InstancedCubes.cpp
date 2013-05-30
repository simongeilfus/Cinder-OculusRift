#include "cinder/app/AppNative.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"

#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/Utilities.h"
#include "cinder/ObjLoader.h"

#include "CameraStereoHMD.h"
#include "OculusVR.h"

using namespace ci;
using namespace ci::app;
using namespace std;


static const char* instanceVertexShader =
"attribute mat4 modelView;\n"
"\n"
"void main()\n"
"{\n"
"   gl_TexCoord[0] 	= gl_MultiTexCoord0;\n"
"   gl_FrontColor 	= gl_Color;\n"
"   gl_Position 	= gl_ModelViewProjectionMatrix * modelView * gl_Vertex;\n"
"}\n";


class OculusSDKTestApp : public AppNative {
  public:
    void prepareSettings( Settings* settings );
    
	void setup();
	void update();
	void draw();
	void keyDown( KeyEvent event );
    
    void render();
    
    ovr::DeviceRef              mOculusVR;
    ovr::DistortionHelperRef    mDistortionHelper;
    CameraStereoHMD             mCamera;
    
    float                       mTime;
    float                       mTimeInc;
    
    gl::Fbo                     mFbo;
    gl::Texture                 mTexture;
    gl::Texture                 mBakedAO;
    
	ci::gl::VboMeshRef          mVboMesh;
	ci::gl::Vbo                 mTransformsBuffer;
    ci::gl::GlslProgRef         mShader;
	GLuint                      mVAO;
    size_t                      mNumInstances;
};

void OculusSDKTestApp::prepareSettings( Settings* settings )
{
    // If more than one Display use the second
    // one for the Oculus
    if( Display::getDisplays().size() > 1 ){
        settings->setDisplay( Display::getDisplays()[1] );
    }
    
    // We need a fullscreen window for the Oculus
    settings->setFullScreen();
}
void OculusSDKTestApp::setup()
{
    
    // Create Render Target a bit bigger to compensate the distortion quality loss
    gl::Fbo::Format format;
    format.enableColorBuffer();
    format.enableDepthBuffer();
    format.setSamples( 8 );
    
    mFbo = gl::Fbo( 1600, 1000, format );
    
    // Init OVR
    mOculusVR           = ovr::Device::create();
    mDistortionHelper   = ovr::DistortionHelper::create();
    
    // Create Stereo Camera
    mCamera = CameraStereoHMD( 640, 800, mOculusVR ? mOculusVR->getFov() : 125, mOculusVR ? mOculusVR->getEyeToScreenDistance() : 10, 10000.0f );
    mCamera.setEyePoint( Vec3f::zero() );
    mCamera.setWorldUp( Vec3f( 0, 1, 0 ) );
    
    // Make the stereo a bit stronger
    mCamera.setEyeSeparation( 1.5f );
        
    
    // Create Test Scene
    mTime       	= 0.0f;
    mTimeInc        = 5.0f;
    mNumInstances   = 500;
    
    // Load Mesh
    TriMesh mesh;
    ObjLoader load( loadAsset( "cube.obj" ) );
    load.load( &mesh );
    
    mVboMesh = gl::VboMesh::create( mesh );
    
    // Load instancing shader
    try {
        mShader = gl::GlslProg::create( instanceVertexShader, NULL );
    }
    catch( gl::GlslProgCompileExc exc ){
        std::cout << "ovr::DistortionHelper Exception: " << std::endl << exc.what() << std::endl;
    }
    
    // Create temp vector with trans matrices
    vector< Matrix44f > matrices;
    for( int i = 0; i < mNumInstances; i++ ){
        Matrix44f mat;
        mat.translate( randVec3f() * randFloat( 60, 500 ) );
        mat.rotate( randVec3f() * 50.0f );
        mat.scale( Vec3f::one() * randFloat(0.1,1) * 0.25f );//* Vec3f( 0.1f, 150.0f, 0.1f ) );
        matrices.push_back( mat );
    }
    
    // Setup transform VAO
    GLint ulocation = mShader->getAttribLocation( "modelView" );
    
	if( ulocation != -1 ){
        
#if( defined GL_APPLE_vertex_array_object )
		glGenVertexArraysAPPLE( 1, &mVAO );
		glBindVertexArrayAPPLE( mVAO );
#else
		glGenVertexArrays( 1, &mVAO );
		glBindVertexArray( mVAO );
#endif
		mTransformsBuffer = gl::Vbo( GL_ARRAY_BUFFER );
        
		mTransformsBuffer.bind();
		for (unsigned int i = 0; i < 4 ; i++) {
			glEnableVertexAttribArray(ulocation + i);
			glVertexAttribPointer(ulocation + i, 4, GL_FLOAT, GL_FALSE, sizeof(Matrix44f), (const GLvoid*)(sizeof(GLfloat) * i * 4));
            
#if( defined GL_ARB_instanced_arrays )
			glVertexAttribDivisorARB(ulocation + i, 1);
#else
			glVertexAttribDivisor(ulocation + i, 1);
#endif
		}
        
		mTransformsBuffer.bufferData( matrices.size() * sizeof(Matrix44f), &matrices.front(), GL_STATIC_READ );
		mTransformsBuffer.unbind();
        
        
#if( defined GL_APPLE_vertex_array_object )
		glBindVertexArrayAPPLE(0);
#else
		glBindVertexArray(0);
#endif
        
        if( glGetError() != GL_NO_ERROR )
            cout << "VAO Init Problem" << endl;
        else cout << "All good" << endl;
	}

    // Load Textures
    gl::Texture::Format texFormat;
    texFormat.enableMipmapping();
    texFormat.setMinFilter( GL_LINEAR_MIPMAP_NEAREST );
    texFormat.setWrap( GL_REPEAT, GL_REPEAT );
    
    mTexture = gl::Texture( loadImage( loadAsset( "Grid.png" ) ), texFormat );
    mBakedAO = gl::Texture( loadImage( loadAsset( "CubeAmbient_Occlusion.png" ) ), texFormat );
    
    // Setup Extra Window
    if( Display::getDisplays().size() > 1 ){
        WindowRef secondWindow = createWindow();
        secondWindow->setSize( 1280, 800 );
    }
    else setWindowSize( 1280, 800 );
}
void OculusSDKTestApp::keyDown( KeyEvent event )
{
    if( event.getChar() == 'e' )
        mTimeInc *= 0.95f;
    else if( event.getChar() =='r' )
        mTimeInc *= 1.05f;
    else if( event.getChar() =='a' )
        mCamera.setEyeSeparation( mCamera.getEyeSeparation() - 0.1f );
    else if( event.getChar() =='s' )
        mCamera.setEyeSeparation( mCamera.getEyeSeparation() + 0.1f );
    else if( event.getChar() =='z' )
        mCamera.setProjectionCenterOffset( mCamera.getProjectionCenterOffset() - 1.1f );
    else if( event.getChar() =='x' )
        mCamera.setProjectionCenterOffset( mCamera.getProjectionCenterOffset() + 1.1f );
}
void OculusSDKTestApp::update()
{
    // Extrat Oculus Orientation and Update Camera
    Quatf orientation;
    
    if( mOculusVR ){
        orientation = mOculusVR->getOrientation();
    }
    
    mCamera.setOrientation( orientation * Quatf( Vec3f( 0, 1, 0 ), M_PI ) );
    
    // Scene animation
    mTime += mTimeInc;
    
    uint8_t *data           = mTransformsBuffer.map( GL_WRITE_ONLY );
    uint8_t stride          = sizeof(Matrix44f);
    
    Perlin p;
    for( int i = 0; i < mNumInstances; i++ ){
        Matrix44f *ptr = reinterpret_cast<Matrix44f*>( &data[0] );
        if( ptr != NULL ){
            ptr->translate( p.dfBm( Vec3f( i, mTime, -i ) * 0.001f ) * 0.5f );
            ptr->rotate(  p.dfBm( Vec3f( i, mTime, -i ) * 0.001f ) * 0.0025f );
            data += stride;
        }
    }
    
	mTransformsBuffer.unmap();
	mTransformsBuffer.unbind();
}
void OculusSDKTestApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    
    // Start Rendering to Our Side by Side RenderTarget
    mFbo.bindFramebuffer();
    
    // Clear
    gl::clear( ColorA( 1.0f, 1.0f, 1.0f, 1.0f ) );
    
    // Render Left Eye
    mCamera.enableStereoLeft();
    gl::setViewport( Area( Vec2f( 0.0f, 0.0f ), Vec2f( mFbo.getWidth() / 2.0f, mFbo.getHeight() ) ) );
    
    render();
    
    // Render Right Eye
    mCamera.enableStereoRight();
    gl::setViewport( Area( Vec2f( mFbo.getWidth() / 2.0f, 0.0f ), Vec2f( mFbo.getWidth(), mFbo.getHeight() ) ) );
    
    render();
    
    mFbo.unbindFramebuffer();
    
    
    // Back to 2d rendering
    gl::setMatricesWindow( getWindowSize(), false );
    gl::setViewport( getWindowBounds() );
    gl::disableDepthRead();
    gl::disableDepthWrite();
    
    // Send the Side by Side texture to our distortion correction shader
    mDistortionHelper->render( mFbo.getTexture(), getWindowBounds() );
    
    // Draw FPS
    gl::setMatricesWindow( getWindowSize() );
    gl::drawString( toString( (int) getAverageFps() ), Vec2f( 10, 10 ) );
}


void OculusSDKTestApp::render()
{
    
    // Enable depth testing
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    // Set camera
    gl::setMatrices( mCamera );
    
    // Add a bit of white fog
    GLfloat fogColor[4]= {1.0f, 1.0f, 1.0f, 1.0f};
    
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.35f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, 500.0f);
    glFogf(GL_FOG_END, 5000.0f);
    glEnable(GL_FOG);
    
    // Render random cubes
    Perlin p;
    gl::color( ColorA::white() );
    mBakedAO.enableAndBind();
    
    
    // Render instanced meshes
    mShader->bind();
    
#if( defined GL_APPLE_vertex_array_object )
    glBindVertexArrayAPPLE(mVAO);
#else
    glBindVertexArray(mVAO);
#endif
    
    mVboMesh->enableClientStates();
    mVboMesh->bindAllData();
    
    if( mVboMesh->getNumIndices() > 0 ){
#if( defined GL_ARB_draw_instanced )
        glDrawElementsInstancedARB( mVboMesh->getPrimitiveType(), mVboMesh->getNumIndices(), GL_UNSIGNED_INT, (GLvoid*)( sizeof(uint32_t) * (size_t)0 ), mNumInstances );
#elif( defined GL_EXT_draw_instanced )
        glDrawElementsInstancedEXT( mVboMesh->getPrimitiveType(), mVboMesh->getNumIndices(), GL_UNSIGNED_INT, (GLvoid*)( sizeof(uint32_t) * (size_t)0 ), mNumInstances );
#else
        glDrawElements( mVboMesh->getPrimitiveType(), mVboMesh->getNumIndices(), GL_UNSIGNED_INT, (GLvoid*)( sizeof(uint32_t) * startIndex ) );
#endif
    }
    else {
#if( defined GL_ARB_draw_instanced )
        glDrawArraysInstancedARB( mVboMesh->getPrimitiveType(), 0, mVboMesh->getNumVertices(), mNumInstances );
#elif( defined GL_EXT_draw_instanced )
        glDrawArraysInstancedEXT( mVboMesh->getPrimitiveType(), 0, mVboMesh->getNumVertices(), mNumInstances );
#else
        glDrawArrays( mVboMesh->getPrimitiveType(), first, mVboMesh->getNumVertices() );
#endif
    }
    
    gl::VboMesh::unbindBuffers();
    mVboMesh->disableClientStates();
#if( defined GL_APPLE_vertex_array_object )
    glBindVertexArrayAPPLE(0);
#else
    glBindVertexArray(0);
#endif
    mShader->unbind();
    mBakedAO.unbind();
    
    mTexture.enableAndBind();
    
    // Render Ground and ceiling, and offset textureCoordinate
    // to fake the travelling animation
    glBegin( GL_QUADS );
    
    float k = 1000.0f;
    float s = 5000.0f;
    float timeScl = 0.0f;
    
    // Floor
    glVertex3f( -s, -k, -s );
    glTexCoord2f( 0 + mTime * timeScl, 0 );
    
    glVertex3f( s, -k, -s );
    glTexCoord2f( 100 + mTime * timeScl, 0 );
    
    glVertex3f( s, -k, s );
    glTexCoord2f( 100 + mTime * timeScl, 100 );
    
    glVertex3f( -s, -k, s );
    glTexCoord2f( 0 + mTime * timeScl, 100 );
    
    // Ceiling
    glVertex3f( -s, k, -s );
    glTexCoord2f( 0 + mTime * timeScl, 0 );
    
    glVertex3f( s, k, -s );
    glTexCoord2f( 100 + mTime * timeScl, 0 );
    
    glVertex3f( s, k, s );
    glTexCoord2f( 100 + mTime * timeScl, 100 );
    
    glVertex3f( -s, k, s );
    glTexCoord2f( 0 + mTime * timeScl, 100 );
    
    glEnd();
    
    mTexture.unbind();
    
    
    
    glDisable(GL_FOG);
}

CINDER_APP_NATIVE( OculusSDKTestApp, RendererGl )
