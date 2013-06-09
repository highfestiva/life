// Xcode-generated on 2011-06-05, with manual modifications.



//#include "../../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS



#import <QuartzCore/QuartzCore.h>
#import "../../Include/Mac/EAGLView.h"
#import "../../../Lepra/Include/Posix/MacLog.h"
#import "../../../Lepra/Include/StringUtility.h"

static EAGLView* gSharedView;

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;
@synthesize canvas = _canvas;
@synthesize baseAngle = _baseAngle;
@synthesize isOpen;
@synthesize responder;
@synthesize orientationStrictness = _orientationStrictness;
@synthesize inputManager;
// UITextInputTraits protocol:
@synthesize autocapitalizationType;
@synthesize autocorrectionType;
@synthesize enablesReturnKeyAutomatically;
@synthesize keyboardAppearance;
@synthesize keyboardType;
@synthesize returnKeyType;
@synthesize secureTextEntry;
//@synthesize spellCheckingType;

// You must implement this method
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
	if (!(self = [super initWithFrame:frame]))
	{
		return nil;
	}

	CAEAGLLayer* eaglLayer = (CAEAGLLayer*)self.layer;
	eaglLayer.opaque = TRUE;
	eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
		kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];

	self.multipleTouchEnabled = YES;
	gSharedView = self;
	isOpen = false;
	responder = nil;
	_orientationStrictness = 1;
	_preResponderStrictness = -1;

	autocapitalizationType = UITextAutocapitalizationTypeWords;
	autocorrectionType = UITextAutocorrectionTypeNo;
	enablesReturnKeyAutomatically = YES;
	keyboardAppearance = UIKeyboardAppearanceDefault;
	keyboardType = UIKeyboardTypeDefault;
	returnKeyType = UIReturnKeySend;
	secureTextEntry = NO;
	//spellCheckingType = UITextSpellCheckingTypeNo;

	return self;
}

- (void)dealloc
{
	[self deleteFramebuffer];
	_canvas = 0;
	[context release];

	[super dealloc];
}

- (void)setOrientationStrictness:(int)strictness
{
	if (_preResponderStrictness <= -1)
	{
		_orientationStrictness = strictness;
	}
}

- (void)setContext:(EAGLContext *)newContext
{
	if (context != newContext)
	{
		[self deleteFramebuffer];

		[context release];
		context = [newContext retain];

		[EAGLContext setCurrentContext:nil];
	}
}

- (void)createFramebuffer
{
	if (context && !defaultFramebuffer)
	{
		[EAGLContext setCurrentContext:context];

		// Create default framebuffer object.
		glGenFramebuffers(1, &defaultFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);

		// Create color render buffer and allocate backing store.
		glGenRenderbuffers(1, &colorRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
		glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);

		// Create depth render buffer
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, framebufferWidth, framebufferHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		}
	}
}

- (void)deleteFramebuffer
{
	if (context)
	{
		[EAGLContext setCurrentContext:context];
		
		if (defaultFramebuffer)
		{
			glDeleteFramebuffers(1, &defaultFramebuffer);
			defaultFramebuffer = 0;
		}
		
		if (colorRenderbuffer)
		{
			glDeleteRenderbuffers(1, &colorRenderbuffer);
			colorRenderbuffer = 0;
		}

		if (depthRenderbuffer)
		{
			glDeleteRenderbuffers(1, &depthRenderbuffer);
			depthRenderbuffer = 0;
		}

		isOpen = false;
	}
}

+ (EAGLView*)sharedView
{
	return gSharedView;
}

- (void)setFramebuffer
{
	if (context)
	{
		[EAGLContext setCurrentContext:context];
		
		if (!defaultFramebuffer)
			[self createFramebuffer];
		
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		
		glViewport(0, 0, framebufferWidth, framebufferHeight);
		isOpen = true;
	}
}

- (BOOL)presentFramebuffer
{
	BOOL success = FALSE;
	
	if (context)
	{
		[EAGLContext setCurrentContext:context];
		
		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		
		success = [context presentRenderbuffer:GL_RENDERBUFFER];
	}
	
	return success;
}

- (void)powerUpAcc
{
	UIDevice* lDevice = [UIDevice currentDevice];
	[lDevice beginGeneratingDeviceOrientationNotifications];
	[[NSNotificationCenter defaultCenter] addObserver:self
						 selector:@selector(orientationDidChange:)
						     name:UIDeviceOrientationDidChangeNotification
						   object:nil];
}

- (void)powerDownAcc
{
	UIDevice* lDevice = [UIDevice currentDevice];
	[lDevice endGeneratingDeviceOrientationNotifications];
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if (_orientationStrictness >= 2)
	{
		return NO;
	}
	else if (_orientationStrictness == 1)
	{
		return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
			interfaceOrientation == UIInterfaceOrientationLandscapeRight);
	}
	return YES;
}

-(void) orientationDidChange:(NSNotification*)notification
{
	UIDeviceOrientation orientation = [[UIDevice currentDevice] orientation];
	if ([self shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)orientation])
	{
		if (_canvas)
		{
			int angle = _baseAngle;
			switch (orientation)
			{
				case UIDeviceOrientationLandscapeLeft:		angle = 90 + _baseAngle;	break;
				case UIDeviceOrientationLandscapeRight:		angle = -90 + _baseAngle;	break;
				case UIDeviceOrientationPortrait:		angle = 0 + _baseAngle;		break;
				case UIDeviceOrientationPortraitUpsideDown:	angle = 180 + _baseAngle;	break;
			}
			angle += (angle < -90)? 360 : 0;
			angle -= (angle > 180)? 360 : 0;
			_canvas->SetOutputRotation(angle);
		}
		[UIApplication sharedApplication].statusBarOrientation = (UIInterfaceOrientation)orientation;
	}
}

-(BOOL) becomeFirstResponder
{
	_preResponderStrictness = _orientationStrictness;
	_orientationStrictness = 2;
	return [super becomeFirstResponder];
}

-(BOOL) resignFirstResponder
{
	_orientationStrictness = _preResponderStrictness;
	_preResponderStrictness = -1;
	[self orientationDidChange:nil];
	return [super resignFirstResponder];
}

- (void)layoutSubviews
{
	// The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
	[self deleteFramebuffer];
}

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (responder)
	{
		[responder touchesMoved:touches withEvent:event];
	}
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (responder)
	{
		[responder touchesMoved:touches withEvent:event];
	}
}


- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (responder)
	{
		[responder touchesEnded:touches withEvent:event];
	}
}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	if (responder)
	{
		[responder touchesCancelled:touches withEvent:event];
	}
}

- (void)insertText:(NSString*)text
{
	Lepra::str s = Lepra::MacLog::Decode(text);
	for (size_t x = 0; x < s.length(); ++x)
	{
		inputManager->NotifyOnChar(s[x]);
	}
}

- (void)deleteBackward
{
	inputManager->NotifyOnChar('\b');
}

- (BOOL)hasText
{
    return YES;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

@end



#endif // iOS
