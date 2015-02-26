// Xcode-generated on 2011-06-05.

#import <UIKit/UIKit.h>
#import <UIKit/UITextInputTraits.h>

#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/EAGL.h>
#import "../../../Lepra/Include/Canvas.h"
#import "../UiInput.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView <UIKeyInput, UITextInputTraits>
{
@private
	EAGLContext *context;
	Lepra::Canvas* canvas;
	int baseAngle;

	// The pixel dimensions of the CAEAGLLayer.
	GLint framebufferWidth;
	GLint framebufferHeight;

	// The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
	GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;

	bool isOpen;
	//UIResponder* responder;
	int _orientationStrictness;
	int _preResponderStrictness;
	bool isLayoutSet;
}

@property(nonatomic, retain) EAGLContext* context;
@property(nonatomic, assign) Lepra::Canvas* canvas;
@property(nonatomic, assign) int baseAngle;
@property(nonatomic, readonly) bool isOpen;
@property(nonatomic, assign) UIResponder* responder;
@property(nonatomic, assign) int orientationStrictness;
@property(nonatomic, assign) UiLepra::InputManager* inputManager;
// Mandatory under the UITextInputTraits protocol.
@property(nonatomic) UITextAutocapitalizationType autocapitalizationType;
@property(nonatomic) UITextAutocorrectionType autocorrectionType;
@property(nonatomic) BOOL enablesReturnKeyAutomatically;
@property(nonatomic) UIKeyboardAppearance keyboardAppearance;
@property(nonatomic) UIKeyboardType keyboardType;
@property(nonatomic) UIReturnKeyType returnKeyType;
@property(nonatomic, getter=isSecureTextEntry) BOOL secureTextEntry;
//@property(nonatomic) UITextSpellCheckingType spellCheckingType;

+ (EAGLView*)sharedView;
- (void)setFramebuffer;
- (BOOL)presentFramebuffer;
- (void)powerUpAcc;
- (void)powerDownAcc;

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation;
- (BOOL)shouldAutorotate;
- (NSUInteger)supportedInterfaceOrientations;
- (void)orientationDidChange:(NSNotification*)notification;

@end
