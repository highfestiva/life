
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS

#import "EditViewController.h"
#import "AnimatedApp.h"
#import "FileHelper.h"
#import "ListViewController.h"
#include "PythonRunner.h"
#import "PythonTextView.h"
#import "WebViewController.h"



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
@property (nonatomic, strong) UIBarButtonItem* manageButton;
@property (nonatomic, strong) PythonTextView* textView;
@property (nonatomic, strong) UIScrollView* scrollView;
@end



@implementation EditViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

	_smartIndent = [NSMutableString new];

	self.manageButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAction target:self action:@selector(manageFile)];
	UIBarButtonItem* executeButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemPlay target:self action:@selector(execute)];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		((UISplitViewController*)self.view.window.rootViewController).maximumPrimaryColumnWidth = 0;
		[self toggleiPadSidebar];
		self.navigationItem.rightBarButtonItem = executeButton;
	}
	else
	{
		self.navigationItem.rightBarButtonItems = [NSArray arrayWithObjects:self.manageButton, executeButton, nil];
		[self.navigationItem setRightBarButtonItem:executeButton];
	}
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willResignActive:) name:UIApplicationWillResignActiveNotification object:nil];

	self.view.backgroundColor = [UIColor whiteColor];

	PythonTextView* textView = [[PythonTextView alloc] initWithFrame:self.view.bounds];
	textView.autoresizingMask = UIViewAutoresizingFlexibleRightMargin|UIViewAutoresizingFlexibleBottomMargin;
	[textView setScrollEnabled:NO];
	textView.editable = YES;
	textView.delegate = self;
	UIScrollView* scrollView = [[RestrictedScrollView alloc] initWithFrame:self.view.bounds];
	scrollView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	[scrollView setScrollEnabled:YES];
	scrollView.bounces = NO;

	self.textView = textView;
	self.scrollView = scrollView;

	[scrollView addSubview:textView];
	[self.view addSubview:scrollView];

	[self updateEditor];
}

-(void) updateEditor
{
	dispatch_async(dispatch_get_main_queue(), ^{
		if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
		{
			// Pop any covering web view.
			if ([self.navigationController.viewControllers count] > 1)
			{
				[self.navigationController popToRootViewControllerAnimated:YES];
			}
		}
		[self setEditing:NO animated:YES];
		[self.scrollView setContentOffset:CGPointMake(0, -self.scrollView.contentInset.top)];
		[self.scrollView setContentSize:CGSizeMake(300,300)];
		NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString* path = [paths objectAtIndex:0];
		NSString* filename = [path stringByAppendingPathComponent:self.title];
		NSError* error = nil;
		NSString* text = [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:&error];
		if (text != nil) {
			text = [text stringByReplacingOccurrencesOfString:@"\t" withString:@"    "];	// Tabs wrap line when entered to the right of screen width.
			[self.textView setText:text];
		} else if (error) {
			self.title = @"";
			[self.textView setText:@""];
		}
	});
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
	CGSize fit = [self.textView fitTextSize];
	CGRect r = CGRectUnion(CGRectMake(0,0,fit.width,fit.height), self.scrollView.bounds);
	r.size.height += r.origin.y;
	r.origin.y = 0;
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

- (void) alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 1)
	{
		UITextField* input = [alertView textFieldAtIndex:0];
		NSString* newFilename = input.text;
		if (![FileHelper fileExists:newFilename])
		{
			NSString* oldFilename = [FileHelper fullPath:self.title];
			NSString* newFullFilename = [FileHelper fullPath:newFilename];
			if ([[NSFileManager defaultManager] moveItemAtPath:oldFilename toPath:newFullFilename error:nil])
			{
				self.title = newFilename;
				dispatch_async(dispatch_get_main_queue(), ^{
					ListViewController* listController = (ListViewController*)self.listController;
					[listController reloadPrototypes];
				});
			}
		}
	}
}

- (void) actionSheet:(UIActionSheet*)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0)	// Delete!
	{
		NSString* filename = self.title;
		self.title = @"";
		self.textView.text = @"";
		if ([FileHelper fileExists:filename])
		{
			filename = [FileHelper fullPath:filename];
			[[NSFileManager defaultManager] removeItemAtPath:filename error:nil];
			dispatch_async(dispatch_get_main_queue(), ^{
				ListViewController* listController = (ListViewController*)self.listController;
				[listController reloadPrototypes];
				[listController popDeleteFile];
			});
		}
	}
	else if (buttonIndex == 1)	// Rename!
	{
		UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Rename prototype"
								message:@"Enter new name:"
							       delegate:self
						      cancelButtonTitle:@"Cancel"
						      otherButtonTitles:@"Rename", nil];
		alert.alertViewStyle = UIAlertViewStylePlainTextInput;
		UITextField* input = [alert textFieldAtIndex:0];
		input.keyboardType = UIKeyboardTypeASCIICapable;
		input.text = self.title;
		[alert show];
	}
	else if (buttonIndex == 2)	// Restore or API docs.
	{
		if ([FileHelper hasOriginal:self.title])	// Button index 2 is restore if we have it.
		{
			[FileHelper restoreSample:self.title];
			[self updateEditor];
			[(ListViewController*)self.listController updateLoc];
		}
		else
		{
			[self showApiDox];
		}
	}
	else if (buttonIndex == 3)	// API dox or Cancel button.
	{
		if ([FileHelper hasOriginal:self.title])	// Button index 3 is API dox if we have it.
		{
			[self showApiDox];
		}
	}
}

-(void) showApiDox
{
	WebViewController* webView = [WebViewController new];
	webView.title = @"Trabant API";
	webView.filename = @"trabant_py_api";
	[self.navigationController pushViewController:webView animated:YES];
}

- (void) toggleiPadSidebar
{
	int maxWidth;
	UIBarButtonItem* toggleFullscreenButton;
	if (((UISplitViewController*)self.view.window.rootViewController).maximumPrimaryColumnWidth == 0)
	{
		maxWidth = 5000;
		toggleFullscreenButton = [[UIBarButtonItem alloc] initWithTitle:@"<" style:UIBarButtonItemStylePlain target:self action:@selector(toggleiPadSidebar)];
	}
	else
	{
		maxWidth = 0;
		toggleFullscreenButton = [[UIBarButtonItem alloc] initWithTitle:@">" style:UIBarButtonItemStylePlain target:self action:@selector(toggleiPadSidebar)];
	}
	UIFont* cogWheelFont = [UIFont fontWithName:@"Helvetica" size:24.0];
	NSDictionary* fontDict = @{NSFontAttributeName: cogWheelFont};
	[toggleFullscreenButton setTitleTextAttributes:fontDict forState:UIControlStateNormal];
	self.navigationItem.leftBarButtonItems = [NSArray arrayWithObjects:toggleFullscreenButton, self.manageButton, nil];

	((UISplitViewController*)self.view.window.rootViewController).maximumPrimaryColumnWidth = maxWidth;
}

- (void) manageFile
{
	if ([self.title length] <= 0)
	{
		return;
	}

	bool restorable = [FileHelper hasOriginal:self.title];
	NSString* button2 = @"Restore";
	NSString* button3 = @"Trabant API pydoc";
	if (!restorable)
	{
		button2 = button3;
		button3 = nil;
	}
	UIActionSheet* sheet = [[UIActionSheet alloc] initWithTitle:@"Manage prototype" delegate:self
						  cancelButtonTitle:@"Cancel"
					     destructiveButtonTitle:@"Delete"
						  otherButtonTitles:@"Rename", button2, button3, nil];
	[sheet showInView:self.view];
}

- (void)execute
{
	if ([self.title length] <= 0)
	{
		return;
	}

	[self.view endEditing:YES];
	[self saveIfChanged];

	TrabantSim::TrabantSim::mApp->UnfoldSimulator();

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
	TrabantSim::PythonRunner::ClearStdOut();
}

-(bool) saveIfChanged
{
	if ([self.title length] <= 0)
	{
		return false;
	}
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
			[(ListViewController*)self.listController updateLoc];
			return true;
		}
	}
	return false;
}

@end

#endif // iOS
