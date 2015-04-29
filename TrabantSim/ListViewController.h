
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine


#pragma once
#import "EditViewController.h"
#import "SettingsViewController.h"

@interface ListViewController : UIViewController <UIPopoverControllerDelegate>
@property (nonatomic, strong) EditViewController* editController;
@property (nonatomic, strong) SettingsViewController* settingsController;
-(void) reloadPrototypes;
-(void) popCreateNew:(NSString*)filename;
-(void) popDeleteFile;
-(void) updateLoc;
@end
