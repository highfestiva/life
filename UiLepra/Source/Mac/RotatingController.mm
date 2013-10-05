
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

//#include "../../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS



#import "../../Include/Mac/RotatingController.h"
#import "../../Include/Mac/EAGLView.h"

@implementation RotatingController

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	return [[EAGLView sharedView] shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}

- (BOOL)shouldAutorotate
{
	return [[EAGLView sharedView] shouldAutorotate];
}

-(NSUInteger)supportedInterfaceOrientations
{
	return [[EAGLView sharedView] supportedInterfaceOrientations];
}

-(void) orientationDidChange:(NSNotification*)notification
{
	[[EAGLView sharedView] orientationDidChange:notification];
}

@end



#endif // iOS
