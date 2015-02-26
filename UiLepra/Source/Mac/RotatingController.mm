
// Author: Jonas Bystr�m
// Copyright (c) Pixel Doctrine

//#include "../../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS



#import "../../Include/Mac/RotatingController.h"
#import "../../Include/Mac/EAGLView.h"

@implementation RotatingController


- (void) viewWillAppear:(BOOL)animated
{
	//[[UIDevice currentDevice] setOrientation:UIInterfaceOrientationLandscapeRight];
}

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

-(BOOL) prefersStatusBarHidden {
	return YES;
}

@end



#endif // iOS
