
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "WebViewController.h"

@implementation WebViewController

-(void)viewDidLoad
{
	UIWebView* webView = [[UIWebView alloc] initWithFrame:self.view.frame];
	webView.scalesPageToFit = YES;
	webView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
	NSURL* url = [[NSBundle mainBundle] URLForResource:self.filename withExtension:@"html"];
	[webView loadRequest:[NSURLRequest requestWithURL:url]];
	[self.view addSubview:webView];
}
@end

#endif // iOS
