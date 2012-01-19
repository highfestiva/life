// Xcode-generated on 2011-06-05.

#import <UIKit/UIKit.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#import "../UiInput.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView <UIKeyInput>
{
@private
	EAGLContext *context;

	// The pixel dimensions of the CAEAGLLayer.
	GLint framebufferWidth;
	GLint framebufferHeight;

	// The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
	GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;

	bool isOpen;
	UIResponder* responder;
	int orientationStrictness;
}

@property (nonatomic, retain) EAGLContext *context;
@property (nonatomic, readonly) bool isOpen;
@property (nonatomic, assign) UIResponder* responder;
@property (nonatomic, assign) int orientationStrictness;
@property (nonatomic, assign) UiLepra::InputManager* inputManager;

+ (EAGLView*)sharedView;
- (void)setFramebuffer;
- (BOOL)presentFramebuffer;

@end
