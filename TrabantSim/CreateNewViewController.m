
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "CreateNewViewController.h"
#import "EditViewController.h"



@implementation CreateNewViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.view.backgroundColor = [UIColor whiteColor];

	UIBarButtonItem* cancelButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCancel target:self action:@selector(cancel)];
	[self.navigationItem setLeftBarButtonItem:cancelButton];
	UIBarButtonItem* createButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(createFile)];
	[self.navigationItem setRightBarButtonItem:createButton];

	[self.filename becomeFirstResponder];
}


-(void) cancel
{
	[self.parent popCreateNew:nil];
}

-(void) createFile
{
	if (self.filename.text.length == 0) {
		return;
	}

	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	NSString* filename = [path stringByAppendingPathComponent:self.filename.text];
	if ([[NSFileManager defaultManager] fileExistsAtPath:filename]) {
		return;
	}
	NSString* content = @"";
	if ([self.createComments isOn]) [content stringByAppendingString:@"#!/usr/bin/env python3\n# -*- coding: utf-8 -*-\n\n"];
	if ([self.createBoilerplate isOn]) {
		content = [content stringByAppendingString:@"from trabant import *\n\n"];
		content = [content stringByAppendingString:@"floor = create_cube(pos=(0,0,-15),side=20,static=True)\n"];
		content = [content stringByAppendingString:@"box = create_cube()\n\n"];

		if ([self.createComments isOn]) [content stringByAppendingString:@"# main game loop\n"];
		content = [content stringByAppendingString:@"while loop():\n"];
		if ([self.createComments isOn]) [content stringByAppendingString:@"\n    # print tap/click info\n"];
		content = [content stringByAppendingString:@"    if taps(): print('currently %i active taps' % len(taps))\n"];
		content = [content stringByAppendingString:@"    [print('tap at x=%f,y=%f. Tap translated to 3D space: %s.' % (tap.x,tap.y,tap.pos3d())) for tap in taps()]\n"];
		content = [content stringByAppendingString:@"    if taps():\n"];
		content = [content stringByAppendingString:@"        print('tap closest to box is at: %s' % closest_tap(box.pos()).pos3d())\n"];
	}
	NSData* rawContents = [content dataUsingEncoding:NSUTF8StringEncoding];
	[[NSFileManager defaultManager] createFileAtPath:filename contents:rawContents attributes:nil];
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.parent reloadPrototypes];
		[self.parent popCreateNew:self.filename.text];
	});
}

@end
#endif // iOS
