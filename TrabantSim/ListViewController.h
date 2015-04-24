
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once
#import "EditViewController.h"

@interface ListViewController : UIViewController
@property (nonatomic, strong) EditViewController* editController;
-(void) reloadPrototypes;
-(void) popCreateNew:(NSString*)filename;
@end
