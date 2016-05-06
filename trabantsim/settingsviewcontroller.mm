
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS
#import "SettingsViewController.h"
#include "../uicure/include/uicure.h"
#include "../cure/include/runtimevariable.h"

using namespace lepra;


@implementation SettingsViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad {
	[super viewDidLoad];

	self.view.backgroundColor = [UIColor whiteColor];

	UILabel* allowRemoteSyncLabel = [[UILabel alloc] initWithFrame:CGRectMake(8, 72, 244, 40)];
	allowRemoteSyncLabel.text = @"Allow remote sync";
	self.allowRemote = [[UISwitch alloc] initWithFrame:CGRectMake(252, 72, 60, 40)];
	bool allow_remote_sync;
	v_get(allow_remote_sync, =, UiCure::GetSettings(), "Simulator.AllowRemoteSync", false);
	self.allowRemote.on = allow_remote_sync;
	UILabel* explanationLabel = [[UILabel alloc] initWithFrame:CGRectMake(8, 112, 304, 160)];
	explanationLabel.text = @"Remote sync will allow synchronization of prototypes between the device and a computer. " \
				 "You can also run the simulation from the computer. (TCP and UDP port 2541 used).";
	explanationLabel.break_mode = NSLineBreakByWordWrapping;
	explanationLabel.numberOfLines = 0;

	[self.view addSubview:allowRemoteSyncLabel];
	[self.view addSubview:self.allowRemote];
	[self.view addSubview:explanationLabel];
}

@end

#endif // iOS
