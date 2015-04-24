
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>
#import "ListViewController.h"

@interface CreateNewViewController : UIViewController
@property (nonatomic, strong) ListViewController* parent;
@property (nonatomic, strong) IBOutlet UITextField* filename;
@property (nonatomic, strong) IBOutlet UISwitch* createComments;
@property (nonatomic, strong) IBOutlet UISwitch* createBoilerplate;
@end
