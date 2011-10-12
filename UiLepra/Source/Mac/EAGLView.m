// Xcode-generated on 2011-06-05, with tiny modifications.



//#include "../../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS



#import <QuartzCore/QuartzCore.h>
#import "../../Include/Mac/EAGLView.h"

static EAGLView* gSharedView;

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;
@synthesize isOpen;
@synthesize responder;
@synthesize orientationStrictness;

// You must implement this method
+ (Class)layerClass
{
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame
{
	if ((self = [super initWithFrame:frame]))
	{
		CAEAGLLayer*eaglLayer = (CAEAGLLayer*)self.layer;
		eaglLayer.opaque = TRUE;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
		kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
	}

	self.multipleTouchEnabled = YES;
	gSharedView = self;
	isOpen = false;
	responder = nil;
	orientationStrictness = 1;

	return self;
}

- (void)dealloc
{
	[self deleteFramebuffer];	
	[context release];

	[super dealloc];
}

- (EAGLContext *)context
{
	return context;
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
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, framebufferWidth, framebufferHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
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

- (void)layoutSubviews
{
	// The framebuffer will be re-created at the beginning of the next setFramebuffer method call.
	[self deleteFramebuffer];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if (orientationStrictness >= 2)
	{
		return NO;
	}
	else if (orientationStrictness == 1)
	{
		return (interfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
			interfaceOrientation == UIInterfaceOrientationLandscapeRight);
	}
	return YES;
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

@end



#endif // iOS
