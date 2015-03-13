
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import "StdOutViewController.h"
#import "PythonTextView.h"



@interface StdOutViewController () <UITextViewDelegate>
@property (nonatomic, strong) PythonTextView* textView;
@property (nonatomic, strong) UIScrollView* scrollView;
@end



@implementation StdOutViewController

- (void)viewDidLoad
{
	[super viewDidLoad];

	self.view.backgroundColor = [UIColor whiteColor];

	self.textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	self.textView.autoresizingMask = UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleBottomMargin;
	[self.textView setScrollEnabled:YES];
	self.scrollView = [[UIScrollView alloc] initWithFrame:self.view.bounds];
	self.scrollView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	[self.scrollView addSubview:self.textView];
	[self.view addSubview:self.scrollView];

	dispatch_async(dispatch_get_main_queue(), ^{
		[self.textView setText:self.text];

		CGSize fit = [self.textView sizeThatFits:CGSizeMake(10000, 100000)];
		float w = [@"W" sizeWithFont:self.textView.boldFont].width;
		fit.width += w*3;
		fit.height += w;
		CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView.bounds);
		r.size.height += r.origin.y;
		r.origin.y = 0;
		self.scrollView.contentSize = r.size;
		self.textView.frame = r;
	});
}


@end
