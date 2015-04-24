
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import "StdOutViewController.h"



@implementation StdOutViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.view.backgroundColor = [UIColor whiteColor];

	self.textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	self.textView.autoresizingMask = UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleBottomMargin;
	self.textView.editable = NO;
	[self.textView setScrollEnabled:YES];
	self.scrollView = [[UIScrollView alloc] initWithFrame:self.view.bounds];
	self.scrollView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	self.scrollView.bounces = NO;
	[self.scrollView addSubview:self.textView];
	[self.view addSubview:self.scrollView];

	dispatch_async(dispatch_get_main_queue(), ^{
		[self.textView setText:self.text];

		CGSize fit = [self.textView fitTextSize];
		CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView.bounds);
		r.size.height += r.origin.y;
		r.origin.y = 0;
		self.scrollView.contentSize = r.size;
		self.textView.frame = r;
	});
}

-(void) viewWillLayoutSubviews
{
	CGSize fit = [self.textView fitTextSize];
	CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView.bounds);
	r.size.height += r.origin.y;
	r.origin.y = 0;
	self.scrollView.contentSize = r.size;
	self.textView.frame = r;
}

@end
