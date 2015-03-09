
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import "EditViewController.h"
#include "../UiLepra/Include/Mac/EAGLView.h"
#include "../UiLepra/Include/Mac/RotatingController.h"
#include "../UiLepra/Include/Mac/UiMacDisplayManager.h"
#import "AnimatedApp.h"
#include "PythonRunner.h"
#import "PythonTextView.h"



bool gBackspaceToLinefeed = false;



@interface RestrictedScrollView : UIScrollView
@end
@implementation RestrictedScrollView
-(void)scrollRectToVisible:(CGRect)rect animated:(BOOL)animated
{
	if (rect.size.width > 100) {
		if (!gBackspaceToLinefeed) {
			rect.origin.x += rect.size.width-100;
		}
		gBackspaceToLinefeed = false;
		rect.size.width = 50;
	}
	[super scrollRectToVisible:rect animated:animated];
}
@end



@interface EditViewController () <UITextViewDelegate>
{
@private
	NSMutableString* _smartIndent;
}
@property (nonatomic, strong) PythonTextView* textView;
@property (nonatomic, strong) UIScrollView* scrollView;
@end



@implementation EditViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

	_smartIndent = [NSMutableString new];

	UIBarButtonItem* executeButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemPlay target:self action:@selector(execute)];
	[self.navigationItem setRightBarButtonItem:executeButton];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActive:) name:UIApplicationWillResignActiveNotification object:nil];

	self.view.backgroundColor = [UIColor whiteColor];

	PythonTextView* textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	textView.autoresizingMask = UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleBottomMargin;
	[textView setScrollEnabled:NO];
	textView.delegate = self;
	UIScrollView* scrollView = [[RestrictedScrollView alloc] initWithFrame:self.view.bounds];
	scrollView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	[scrollView setScrollEnabled:YES];

	self.textView = textView;
	self.scrollView = scrollView;

	[scrollView addSubview:textView];
	[self.view addSubview:scrollView];

	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	NSString* filename = [path stringByAppendingPathComponent:self.title];
	NSString* text = [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:nil];
	text = [text stringByReplacingOccurrencesOfString:@"\t" withString:@"    "];	// Tabs wrap line when entered to the right of screen width.
	self.textView.text = text;
}

-(void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}


#pragma mark - Notification Handlers

-(BOOL) textView:(UITextView*)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString*)text
{
	if ([text hasPrefix:@"\n"]) {
		NSString* file = textView.text;
		NSRange found = [file rangeOfString:@"\n" options:NSBackwardsSearch range:NSMakeRange(0,range.location)];
		found.location = (found.location == NSNotFound)? 0 : found.location+1;
		int remaining = (int)range.location - (int)found.location - 1;
		int spacecnts = 0;
		for (; remaining > 0; ++found.location, --remaining) {
			unichar u = [file characterAtIndex:found.location];
			if (u == ' ') {
				++spacecnts;
				if (spacecnts >= 4) {
					spacecnts = 0;
					[_smartIndent appendString:@"    "];
				}
			} else if (u == '\t') {
				spacecnts = 0;
				[_smartIndent appendString:@"    "];
			} else {
				break;
			}
		}
		if (range.location > 0 && [file characterAtIndex:range.location-1] == ':') {
			[_smartIndent appendString:@"    "];
		}
	} else if ([text length] == 0 && range.location > 0 && [textView.text characterAtIndex:range.location-1] == '\n') {
		gBackspaceToLinefeed = true;
	} else {
		gBackspaceToLinefeed = false;
	}
	return YES;
}

-(void) textViewDidChange:(UITextView*)textView
{
	if ([_smartIndent length] > 0) {
		NSString* indent = [NSString stringWithString:_smartIndent];
		[_smartIndent setString:@""];
		NSRange selectedRange = textView.selectedRange;
		UITextRange* textRange = [textView textRangeFromPosition:textView.selectedTextRange.start toPosition:textView.selectedTextRange.start];
		[textView replaceRange:textRange withText:indent];
		[textView setSelectedRange:NSMakeRange(selectedRange.location+[indent length], 0)];
	}
	[self viewWillLayoutSubviews];
}

-(void) willResignActive:(NSNotification*)notification
{
	[self saveIfChanged];
}

-(void)viewWillAppear:(BOOL)animated
{
	[super viewWillAppear:animated];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillHide:) name:UIKeyboardWillHideNotification object:nil];
}

-(void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillShowNotification object:nil];
	[[NSNotificationCenter defaultCenter] removeObserver:self name:UIKeyboardWillHideNotification object:nil];
}

-(void) viewWillLayoutSubviews
{
	CGSize fit = [self.textView sizeThatFits:CGSizeMake(10000, 100000)];
	float w = [@"W" sizeWithFont:self.textView.boldFont].width;
	fit.width += w*3;
	fit.height += w;
	CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView.frame);
	self.scrollView.contentSize = r.size;
	self.textView.frame = r;
}

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
	CGRect newFrame = self.scrollView.frame;
	newFrame.size.height -= keyboardEndFrame.size.height * (up?1:-1);
	self.scrollView.frame = newFrame;
	[UIView commitAnimations];
}

-(BOOL) navigationShouldPopOnBackButton
{
	[self saveIfChanged];
	return YES;
}

- (void)execute
{
	[self.view endEditing:YES];
	[self saveIfChanged];
	[self.view.window setHidden:YES];

	UIWindow* window = ((UiLepra::MacDisplayManager*)TrabantSim::TrabantSim::mApp->mUiManager->GetDisplayManager())->GetWindow();
	if (!window.rootViewController) {
		RotatingController* controller = [[RotatingController alloc] init];
		controller.navigationBarHidden = YES;
		controller.view = [EAGLView sharedView];
		window.rootViewController = controller;
	}
	[window makeKeyAndVisible];
	TrabantSim::TrabantSim::mApp->mActiveCounter = 0;	// Make sure no lost event causes a halt.
	TrabantSim::TrabantSim::mApp->Resume(false);
	[[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationNone];

	wchar_t appDir[4096];
	wchar_t filename[4096];
	::memset(appDir, 0, sizeof(appDir));
	::memset(filename, 0, sizeof(filename));
	NSString* appPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/"];
	[appPath getCString:(char*)appDir maxLength:sizeof(appDir) encoding:NSUTF32LittleEndianStringEncoding];
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* filePath = [[paths objectAtIndex:0] stringByAppendingPathComponent:self.title];
	[filePath getCString:(char*)filename maxLength:sizeof(filename) encoding:NSUTF32LittleEndianStringEncoding];
	TrabantSim::PythonRunner::Run(appDir, filename);
}

-(void) saveIfChanged
{
	// Only write if we've made some changes.
	NSString* editText = self.textView.text;
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	NSString* filename = [path stringByAppendingPathComponent:self.title];
	NSString* fileText = [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:nil];
	if (![editText isEqualToString:fileText]) {
		NSFileHandle* file = [NSFileHandle fileHandleForWritingAtPath:filename];
		if (file) {
			[file truncateFileAtOffset:0];
			NSData* rawContents = [editText dataUsingEncoding:NSUTF8StringEncoding];
			[file writeData:rawContents];
			[file closeFile];
		}
	}
}

@end
