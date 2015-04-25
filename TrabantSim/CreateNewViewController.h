
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>
#import "ListViewController.h"

@interface CreateNewViewController : UIViewController
@property (nonatomic, strong) ListViewController* parent;
@property (nonatomic, strong) UITextField* filename;
@property (nonatomic, strong) UISwitch* createComments;
@property (nonatomic, strong) UISwitch* createBoilerplate;
@end
