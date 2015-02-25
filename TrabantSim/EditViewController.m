
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import "EditViewController.h"
#import "PythonTextView.h"



@interface EditViewController () <UITextViewDelegate>
@property (nonatomic, strong) PythonTextView *textView;
@end



@implementation EditViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

	UIBarButtonItem* executeButton = [[UIBarButtonItem alloc] initWithTitle:@"Execute" style:UIBarButtonItemStyleBordered target:self action:@selector(execute)];
	[self.navigationItem setRightBarButtonItem:executeButton];

	self.view.backgroundColor = [UIColor whiteColor];

	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];

	PythonTextView* textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	textView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	textView.delegate = self;

	self.textView = textView;

	[self.view addSubview:textView];

	self.textView.text =
		@"#!/usr/bin/env python3\n" \
		@"# As simple as it gets: bounce the ball around.\n" \
		@"from trabant import *\n" \
		@"ball = create_sphere()\n" \
		@"while loop():\n" \
		@"	tap = closest_tap(ball.pos())\n" \
		@"	if tap and (tap.pos3d()-ball.pos()).length() <= 1:\n" \
		@"		ball.vel(tap.vel3d().limit(50))\n" \
		@"		tap.invalidate()\n" \
		@"	ball.bounce_in_rect((-5,0,-3),(5,0,3))\n";
}


#pragma mark - Notification Handlers

- (void)keyboardWillShow:(NSNotification*)aNotification
{
	[self moveTextViewForKeyboard:aNotification up:YES];
}

- (void)keyboardWillHide:(NSNotification*)aNotification
{
	[self moveTextViewForKeyboard:aNotification up:NO];
}


#pragma mark - Convenience

- (void)moveTextViewForKeyboard:(NSNotification*)aNotification up:(BOOL)up
{
	NSDictionary* userInfo = [aNotification userInfo];
	CGRect keyboardEndFrame;
	[[userInfo objectForKey:UIKeyboardFrameEndUserInfoKey] getValue:&keyboardEndFrame];
	[UIView beginAnimations:nil context:nil];
	CGRect newFrame = _textView.frame;
	newFrame.size.height -= keyboardEndFrame.size.height * (up?1:-1);
	_textView.frame = newFrame;
	[UIView commitAnimations];
}

- (void)execute
{
}

@end
