#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "IRenderingEngine.hpp"
#import "Quaternion.hpp"
#import <vector>

static const float AnimationDuration = 0.25f;

using namespace std;

struct Vertex {
    vec3 Position;
    vec4 Color;
};

struct Animation {
    Quaternion Start;
    Quaternion End;
    Quaternion Current;
    float Elapsed;
    float Duration;
};

class RenderingEngine1 : public IRenderingEngine {
public:
    RenderingEngine1();
    void Initialize(int width, int height);
    void Render() const;
	
	void getBufferImage(IplImage * buffer);
	void Clear(float r, float g, float b);
	void DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth);
private:
	vector<vector<Vertex>>cones;
    Animation m_animation;
    GLuint m_framebuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
};

IRenderingEngine* CreateRenderer1()
{
    return new RenderingEngine1();
}

RenderingEngine1::RenderingEngine1()
{
    // Create & bind the color buffer so that the caller can allocate its space.
    glGenRenderbuffersOES(1, &m_colorRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
}

vector<Vertex> getCone(int x, int y, float r, float g, float b, int depth) {
	// Allocate space for the cone vertices.
	vector<Vertex> cone((40 + 1) * 2);
	
	// Initialize the vertices of the triangle strip.
	vector<Vertex>::iterator vertex = cone.begin();
	const float dtheta = TwoPi / 40;
	for (float theta = 0; vertex != cone.end(); theta += dtheta) {
		vec4 color(r, g, b, 1); //RGBA
		
		// Apex vertex
		vertex->Position = vec3(x, y, 1);
		vertex->Color = color;
		vertex++;
		
		// Rim vertex
		vertex->Position.x = x + 480 * cos(theta);
		vertex->Position.y = y + 480 * sin(theta);
		vertex->Position.z = -5;
		vertex->Color = color;
		vertex++;
	}
	return cone;
}

void RenderingEngine1::Initialize(int width, int height)
{
	
    // Create the depth buffer.
    glGenRenderbuffersOES(1, &m_depthRenderbuffer);
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_depthRenderbuffer);
    glRenderbufferStorageOES(GL_RENDERBUFFER_OES,
                             GL_DEPTH_COMPONENT16_OES,
                             width,
                             height);
    
    // Create the framebuffer object; attach the depth and color buffers.
    glGenFramebuffersOES(1, &m_framebuffer);
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_framebuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
                                 GL_COLOR_ATTACHMENT0_OES,
                                 GL_RENDERBUFFER_OES,
                                 m_colorRenderbuffer);
    glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
                                 GL_DEPTH_ATTACHMENT_OES,
                                 GL_RENDERBUFFER_OES,
                                 m_depthRenderbuffer);
    
    // Bind the color buffer for rendering.
    glBindRenderbufferOES(GL_RENDERBUFFER_OES, m_colorRenderbuffer);
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    
    glMatrixMode(GL_PROJECTION);
	glOrthof(0, width, 0, height, -1, 5);
    
    glMatrixMode(GL_MODELVIEW);
}

void RenderingEngine1::Render() const
{
    glPushMatrix();
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
	
    mat4 rotation(m_animation.Current.ToMatrix());
    glMultMatrixf(rotation.Pointer());
	
    // Draw the cone.
	int lmt = cones.size();
	for (int i=0; i<lmt; i++) {
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &cones[i][0].Position.x);
		glColorPointer(4, GL_FLOAT, sizeof(Vertex),  &cones[i][0].Color.x);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, cones[i].size());
	}
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
	
    glPopMatrix();
}
void RenderingEngine1::getBufferImage(IplImage * buffer){
	int m_width = buffer->width;
	int m_height = buffer->height;
	glReadPixels(0, 0, m_width, m_height, GL_BGRA, GL_UNSIGNED_BYTE, buffer->imageData);
	cvFlip(buffer); // 매우 필요.
}

void RenderingEngine1::Clear(float r, float g, float b) { // test 해볼것.
	vector<vector<Vertex>>::iterator cone = cones.begin();
    while (cone != cones.end()) { //clean a memory shit.
        cone->clear();
        cone++;
    }
	cones.clear(); //remove items;
	glClearColor(r, g, b, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderingEngine1::DrawCone(int x, int y, Byte r, Byte g, Byte b, int depth) {
	cones.push_back(getCone(x,y,r/255.0f,g/255.0f,b/255.0f,depth));
}
