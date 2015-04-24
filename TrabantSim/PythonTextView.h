
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine

#import "CYRTextView.h"

@interface PythonTextView : CYRTextView

@property (nonatomic, strong) UIFont *defaultFont;
@property (nonatomic, strong) UIFont *boldFont;
@property (nonatomic, strong) UIFont *italicFont;

-(CGSize) fitTextSize;
+(CGSize) slowFitTextSize:(NSString*)text;

@end

