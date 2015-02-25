
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#import "PythonTextView.h"

#import <CoreText/CoreText.h>

#define RGB(r,g,b) [UIColor colorWithRed:r/255.0f green:g/255.0f blue:b/255.0f alpha:1.0f]

@implementation PythonTextView

#pragma mark - Initialization & Setup

- (id)initWithFrame:(CGRect)frame
{
	self = [super initWithFrame:frame];

	if (self)
	{
		[self commonSetup];
	}

	return self;
}

- (void)commonSetup
{
	_defaultFont = [UIFont systemFontOfSize:14.0f];
	_boldFont = [UIFont boldSystemFontOfSize:14.0f];
	_italicFont = [UIFont fontWithName:@"HelveticaNeue-Oblique" size:14.0f];

	self.font = _defaultFont;
	self.textColor = [UIColor blackColor];

	[self addObserver:self forKeyPath:NSStringFromSelector(@selector(defaultFont)) options:NSKeyValueObservingOptionNew context:0];
	[self addObserver:self forKeyPath:NSStringFromSelector(@selector(boldFont)) options:NSKeyValueObservingOptionNew context:0];
	[self addObserver:self forKeyPath:NSStringFromSelector(@selector(italicFont)) options:NSKeyValueObservingOptionNew context:0];

	if (_italicFont == nil && ([UIFontDescriptor class] != nil))
	{
		// This works around a bug in 7.0.3 where HelveticaNeue-Italic is not present as a UIFont option
		_italicFont = (__bridge_transfer UIFont*)CTFontCreateWithName(CFSTR("HelveticaNeue-Italic"), 14.0f, NULL);
	}

	self.tokens = [self solverTokens];
}


- (NSArray *)solverTokens
{
	NSArray *solverTokens = @[
	   [CYRToken tokenWithName:@"operator"
			expression:@"[%/\\*,\\;:=<>\\+\\-\\^!&|~]"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(245, 0, 110)
				 }],
	   [CYRToken tokenWithName:@"round_brackets"
			expression:@"[\\(\\)]"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(161, 75, 0)
				 }],
	   [CYRToken tokenWithName:@"other_brackets"
			expression:@"[\\[\\]{}]"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(105, 0, 0),
				 NSFontAttributeName : self.boldFont
				 }],
	   [CYRToken tokenWithName:@"hex"
			expression:@"0x[0-9a-fA-F]+"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(0, 0, 255)
				 }],
	   [CYRToken tokenWithName:@"binary"
			expression:@"0b[01]+"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(0, 0, 255)
				 }],
	   [CYRToken tokenWithName:@"float"
			expression:@"\\d+\\.?\\d+e[\\+\\-]?\\d+|\\d+\\.\\d+"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(0, 0, 255)
				 }],
	   [CYRToken tokenWithName:@"integer"
			expression:@"\\d+"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(0, 0, 255)
				 }],
	   [CYRToken tokenWithName:@"reserved_words"
			expression:@"\\b(False|None|True|and|as|assert|break|class|continue|def|del|elif|else|except|finally|for|from|global|if|import|in|is|lambda|nonlocal|not|or|pass|raise|return|try|while|with|yield)\\b"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(104, 0, 111),
				 NSFontAttributeName : self.boldFont
				 }],
	   [CYRToken tokenWithName:@"double_quoted_string"
			expression:@"\".*?(\"|$)"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(24, 110, 109)
				 }],
	   [CYRToken tokenWithName:@"string"
			expression:@"'.*?('|$)"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(24, 110, 109)
				 }],
	   [CYRToken tokenWithName:@"comment"
			expression:@"#.*"
			attributes:@{
				 NSForegroundColorAttributeName : RGB(31, 131, 0),
				 NSFontAttributeName : self.italicFont
				 }]
	];
	return solverTokens;
}


#pragma mark - Cleanup

- (void)dealloc
{
	[self removeObserver:self forKeyPath:NSStringFromSelector(@selector(defaultFont))];
	[self removeObserver:self forKeyPath:NSStringFromSelector(@selector(boldFont))];
	[self removeObserver:self forKeyPath:NSStringFromSelector(@selector(italicFont))];
}


#pragma mark - KVO

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
	if ([keyPath isEqualToString:NSStringFromSelector(@selector(defaultFont))] ||
		[keyPath isEqualToString:NSStringFromSelector(@selector(boldFont))] ||
		[keyPath isEqualToString:NSStringFromSelector(@selector(italicFont))])
	{
		// Reset the tokens, this will clear any existing formatting
		self.tokens = [self solverTokens];
	}
	else
	{
		[super observeValueForKeyPath:keyPath ofObject:object change:change context:context];
	}
}


#pragma mark - Overrides

- (void)setDefaultFont:(UIFont *)defaultFont
{
	_defaultFont = defaultFont;
	self.font = defaultFont;
}

@end
