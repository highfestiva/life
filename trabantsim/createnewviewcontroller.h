
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>
#import "listviewcontroller.h"

@interface CreateNewViewController : UIViewController <UITextFieldDelegate>
@property (nonatomic, strong) ListViewController* parent;
@property (nonatomic, strong) UITextField* filename;
@property (nonatomic, strong) UISwitch* createComments;
@property (nonatomic, strong) UISwitch* createBoilerplate;
@end
