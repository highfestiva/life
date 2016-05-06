
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>
#import "PythonTextView.h"


@interface StdOutViewController : UIViewController
@property (nonatomic, strong) PythonTextView* textView;
@property (nonatomic, strong) UIScrollView* scrollView_;
@property (nonatomic, strong) NSString* text;
@end
