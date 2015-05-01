
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "CreateNewViewController.h"
#import "EditViewController.h"
#import "FileHelper.h"



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

	NSString* fn = @"test.py";
	for (int i = 2; i < 100; ++i) {
		if (![FileHelper fileExists:fn]) {
			break;
		}
		fn = [NSString stringWithFormat:@"test%i.py", i];
	}

	self.filename = [[UITextField alloc] initWithFrame:CGRectMake(8, 72, 304, 36)];
	self.filename.delegate = self;
	self.filename.text = fn;
	self.filename.borderStyle = UITextBorderStyleRoundedRect;
	self.filename.placeholder = @"Enter filename";
	self.filename.autocorrectionType = UITextAutocorrectionTypeNo;
	self.filename.keyboardType = UIKeyboardTypeDefault;
	self.filename.returnKeyType = UIReturnKeyDone;
	self.filename.clearButtonMode = UITextFieldViewModeWhileEditing;
	self.filename.contentVerticalAlignment = UIControlContentVerticalAlignmentCenter;
	UILabel* comments = [[UILabel alloc] initWithFrame:CGRectMake(8, 116, 244, 40)];
	comments.text = @"Create comments";
	self.createComments = [[UISwitch alloc] initWithFrame:CGRectMake(252, 116, 60, 40)];
	self.createComments.on = YES;
	UILabel* boilerplate = [[UILabel alloc] initWithFrame:CGRectMake(8, 156, 244, 40)];
	boilerplate.text = @"Create boilerplate";
	self.createBoilerplate = [[UISwitch alloc] initWithFrame:CGRectMake(252, 156, 60, 40)];
	self.createBoilerplate.on = YES;

	[self.view addSubview:self.filename];
	[self.view addSubview:comments];
	[self.view addSubview:self.createComments];
	[self.view addSubview:boilerplate];
	[self.view addSubview:self.createBoilerplate];

	[self.filename becomeFirstResponder];
}

-(BOOL) textFieldShouldReturn:(UITextField*)textField
{
	return [self createFile];
}

-(void) cancel
{
	[self.parent popCreateNew:nil];
}

-(bool) createFile
{
	if (self.filename.text.length == 0 || [FileHelper fileExists:self.filename.text]) {
		return false;
	}
	NSString* full = [FileHelper fullPath:self.filename.text];
	NSString* content = @"";
	if (self.createComments.on) content = [content stringByAppendingString:@"#!/usr/bin/env python3\n# -*- coding: utf-8 -*-\n\n"];
	if (self.createBoilerplate.on) {
		content = [content stringByAppendingString:@"from trabant import *\n\n"];
		content = [content stringByAppendingString:@"floor = create_box(pos=(0,0,-15),side=20,static=True)\n"];
		content = [content stringByAppendingString:@"box = create_box()\n\n"];

		if (self.createComments.on) content = [content stringByAppendingString:@"# main game loop\n"];
		content = [content stringByAppendingString:@"while loop():\n"];
		if (self.createComments.on) content = [content stringByAppendingString:@"\n    # place box where you tap\n"];
		content = [content stringByAppendingString:@"    if taps():\n"];
		content = [content stringByAppendingString:@"        box.pos(closest_tap(box.pos()).pos3d())\n"];
	}
	NSData* rawContents = [content dataUsingEncoding:NSUTF8StringEncoding];
	[[NSFileManager defaultManager] createFileAtPath:full contents:rawContents attributes:nil];
	dispatch_async(dispatch_get_main_queue(), ^{
		[self.parent reloadPrototypes];
		[self.parent popCreateNew:self.filename.text];
	});
	return true;
}

@end

#endif // iOS
