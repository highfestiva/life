
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "../../../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS



#import "../../include/mac/rotatingcontroller.h"
#import "../../Include/Mac/EAGLView.h"



@implementation RotatingController


-(void) pushViewController:(UIViewController*)viewController animated:(BOOL)animated {
	forceGameLayout = (viewController.view == [EAGLView sharedView_]);
	[super pushViewController:viewController animated:animated];
	if (animated && (hadGameLayout || (forceGameLayout && ![[EAGLView sharedView_] shouldAutorotateToInterfaceOrientation:self.interfaceOrientation]))) {
		hadGameLayout = forceGameLayout;
		if ([[[UIDevice currentDevice] systemVersion] floatValue] >= 8.0) {
			UIViewController *viewController = [[UIViewController alloc] init];
			[self presentViewController:viewController animated:NO completion:^{
				[self dismissViewControllerAnimated:NO completion:nil];
			}];
		}
	}
	forceGameLayout = false;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation {
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView_]) {
		return [[EAGLView sharedView_] shouldAutorotateToInterfaceOrientation:interfaceOrientation];
	}
	return [super shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}

- (BOOL)shouldAutorotate_
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView_]) {
		return [[EAGLView sharedView_] shouldAutorotate_];
	}
	return [super shouldAutorotate];
}

-(NSUInteger)supportedInterfaceOrientations_
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView_]) {
		return [[EAGLView sharedView_] supportedInterfaceOrientations_];
	}
	return [super supportedInterfaceOrientations];
}

-(UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView_]) {
		return [EAGLView sharedView_].preferredRotation;
	}
	return [super preferredInterfaceOrientationForPresentation];
}

-(BOOL) prefersStatusBarHidden {
	return (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView_]);
}

@end



#endif // iOS
