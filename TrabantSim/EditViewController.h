
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>

@interface EditViewController : UIViewController <UIActionSheetDelegate, UIAlertViewDelegate>
@property (nonatomic, strong) UIViewController* listController;
-(void) updateEditor;
-(void) saveIfChanged;
@end
