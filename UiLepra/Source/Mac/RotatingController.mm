
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#include "../../../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS



#import "../../Include/Mac/RotatingController.h"
#import "../../Include/Mac/EAGLView.h"



@implementation RotatingController


-(void) pushViewController:(UIViewController*)viewController animated:(BOOL)animated
{
	forceGameLayout = (viewController.view == [EAGLView sharedView]);
	[super pushViewController:viewController animated:animated];
	if (animated && (!forceGameLayout || ![[EAGLView sharedView] shouldAutorotateToInterfaceOrientation:self.interfaceOrientation]))
	{
		UIViewController *viewController = [[UIViewController alloc] init];
		[self presentViewController:viewController animated:NO completion:^{
			[self dismissViewControllerAnimated:NO completion:nil];

		}];
	}
	forceGameLayout = false;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView])
	{
		return [[EAGLView sharedView] shouldAutorotateToInterfaceOrientation:interfaceOrientation];
	}
	return [super shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}

- (BOOL)shouldAutorotate
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView])
	{
		return [[EAGLView sharedView] shouldAutorotate];
	}
	return [super shouldAutorotate];
}

-(NSUInteger)supportedInterfaceOrientations
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView])
	{
		return [[EAGLView sharedView] supportedInterfaceOrientations];
	}
	return [super supportedInterfaceOrientations];
}

-(UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
	if (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView])
	{
		return [EAGLView sharedView].preferredRotation;
	}
	return [super preferredInterfaceOrientationForPresentation];
}

-(BOOL) prefersStatusBarHidden
{
	return (forceGameLayout || self.visibleViewController.view == [EAGLView sharedView]);
}

@end



#endif // iOS
