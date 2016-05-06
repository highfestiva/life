
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once
#import "editviewcontroller.h"
#import "SettingsViewController.h"

@interface ListViewController : UIViewController <UIPopoverControllerDelegate>
@property (nonatomic, strong) EditViewController* editController;
@property (nonatomic, strong) SettingsViewController* settingsController_;
-(void) reloadPrototypes;
-(void) popCreateNew:(NSString*)filename;
-(void) delete_file;
-(void) updateLoc;
@end
