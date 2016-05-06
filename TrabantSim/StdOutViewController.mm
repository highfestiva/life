
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS

#import "stdoutviewcontroller.h"



@implementation StdOutViewController

- (void)viewDidLoad {
	[super viewDidLoad];

	self.view.backgroundColor = [UIColor whiteColor];

	self.textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	self.textView.autoresizingMask = UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleBottomMargin;
	self.textView.editable = NO;
	[self.textView setScrollEnabled:YES];
	self.scrollView_ = [[UIScrollView alloc] initWithFrame:self.view.bounds];
	self.scrollView_.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	self.scrollView_.bounces = NO;
	[self.scrollView_ addSubview:self.textView];
	[self.view addSubview:self.scrollView_];

	dispatch_async(dispatch_get_main_queue(), ^{
		NSRegularExpression* regex = [NSRegularExpression regularExpressionWithPattern:@"File \".*pylib/" options:0 error:nil];
		NSRange fullRange = NSMakeRange(0, [self.text length]);
		NSMutableString* text = [NSMutableString stringWithString:self.text];
		[regex replaceMatchesInString:text options:0 range:fullRange withTemplate:@"File \""];
		[self.textView setText:text];

		CGSize fit = [self.textView fitTextSize];
		CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView_.bounds);
		r.size.height += r.origin.y;
		r.origin.y = 0;
		self.scrollView_.contentSize = r.size;
		self.textView.frame = r;
	});
}

-(void) viewWillLayoutSubviews {
	CGSize fit = [self.textView fitTextSize];
	CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView_.bounds);
	r.size.height += r.origin.y;
	r.origin.y = 0;
	self.scrollView_.contentSize = r.size;
	self.textView.frame = r;
}

@end

#endif // iOS
