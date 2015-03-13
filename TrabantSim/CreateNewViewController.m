
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "CreateNewViewController.h"
#import "EditViewController.h"



@interface CreateNewViewController () <UITextViewDelegate>
@property (nonatomic, strong) UITextField* nameField;
@property (nonatomic, strong) UIButton* boilerplateButton;
@end



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

	self.nameField = [[UITextField alloc] initWithFrame:CGRectMake(8, 60, self.view.bounds.size.width-16, 40)];
	self.nameField.text = @"test.py";
	self.nameField.autocorrectionType = UITextAutocorrectionTypeNo;
	self.boilerplateButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
	[self.boilerplateButton setTitle:@"Create boilerplate" forState:UIControlStateNormal];
	self.boilerplateButton.frame = CGRectMake(8, 108, self.view.bounds.size.width-16, 40);
	[self.view addSubview:self.nameField];
	[self.view addSubview:self.boilerplateButton];

	[self.nameField becomeFirstResponder];
}


-(void) cancel
{
	[self.navigationController popViewControllerAnimated:YES];
}

-(void) createFile
{
	if (self.nameField.text.length == 0) {
		return;
	}

	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	NSString* filename = [path stringByAppendingPathComponent:self.nameField.text];
	if ([[NSFileManager defaultManager] fileExistsAtPath:filename]) {
		return;
	}
	NSString* content = @"from trabant import *\n\n";
	if ([self.boilerplateButton isSelected]) {
		content = [@"#!/usr/bin/env python3\n# -*- coding: utf-8 -*-\n\n" stringByAppendingString:content];
		content = [content stringByAppendingString:@"floor = create_cube(pos=(0,0,-15),side=20,static=True)\n"];
		content = [content stringByAppendingString:@"box = create_cube()\n\nwhile loop():\n    pass\n"];
	}
	NSData* rawContents = [content dataUsingEncoding:NSUTF8StringEncoding];
	[[NSFileManager defaultManager] createFileAtPath:filename contents:rawContents attributes:nil];
	[self.parent reloadPrototypes];
	UINavigationController* navController = self.navigationController;
	[navController popViewControllerAnimated:NO];
	dispatch_async(dispatch_get_main_queue(), ^{
		EditViewController* editController = [EditViewController new];
		editController.title = self.nameField.text;
		[navController pushViewController:editController animated:YES];
	});
}

@end
#endif // iOS
