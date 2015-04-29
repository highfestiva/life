
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "ListViewController.h"
#include "../UiCure/Include/UiCure.h"
#include "../Cure/Include/RuntimeVariable.h"
#import "CreateNewViewController.h"
#import "EditViewController.h"
#import "FileHelper.h"
#import "SettingsViewController.h"



@interface ListViewController () <UITableViewDelegate, UITableViewDataSource>
{
@private
	bool _allowUpdateSelection;
}

@property (nonatomic, strong) UITableView* tableView;
@property (nonatomic, strong) NSMutableArray* files;
@property (nonatomic, strong) NSMutableArray* loc;
@property (nonatomic, strong) UIPopoverController* createNewPopover;

@end



@implementation ListViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

	_allowUpdateSelection = true;

	UIBarButtonItem* settingsButton = [[UIBarButtonItem alloc] initWithTitle:@"\u2699" style:UIBarButtonItemStylePlain target:self action:@selector(settings)];
	UIFont* cogWheelFont = [UIFont fontWithName:@"Helvetica" size:24.0];
	NSDictionary* fontDict = @{NSFontAttributeName: cogWheelFont};
	[settingsButton setTitleTextAttributes:fontDict forState:UIControlStateNormal];
	[self.navigationItem setLeftBarButtonItem:settingsButton];
	UIBarButtonItem* createButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCompose target:self action:@selector(createNew)];
	[self.navigationItem setRightBarButtonItem:createButton];

	UITableView* tableView = [[UITableView alloc] initWithFrame:self.view.bounds];
	tableView.autoresizingMask = UIViewAutoresizingFlexibleWidth|UIViewAutoresizingFlexibleHeight;
	tableView.delegate = self;
	tableView.dataSource = self;

	self.tableView = tableView;

	[self.view addSubview:tableView];

	[self reloadPrototypes];
}

- (void) viewWillAppear:(BOOL)animated {
	NSIndexPath* indexPath = [self.tableView indexPathForSelectedRow];
	if (indexPath) {
		NSString* loc = [FileHelper countLoc:self.editController.title];
		[self.loc setObject:loc atIndexedSubscript:indexPath.row];
		[self.tableView deselectRowAtIndexPath:indexPath animated:YES];
 	}
}

- (NSInteger) tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section {
	return [self.files count];
}

- (UITableViewCell*) tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath {
	static NSString* identifier = @"CellId";
	UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:identifier];
	if (cell == nil)
	{
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:identifier];
	}
	cell.textLabel.text = [self.files objectAtIndex:indexPath.row];
	cell.detailTextLabel.text = [self.loc objectAtIndex:indexPath.row];
	return cell;
}

-(void) tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
	[self showFile:[self.files objectAtIndex:indexPath.row]];
}

-(void) showFile:(NSString*)filename
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		_allowUpdateSelection = false;
		[self.editController saveIfChanged];
		_allowUpdateSelection = true;
		self.editController.title = filename;
		[self.editController updateEditor];
	}
	else
	{
		EditViewController* editController = [EditViewController new];
		editController.title = filename;
		editController.listController = self;
		[self.navigationController pushViewController:editController animated:YES];
	}
}

-(void) reloadPrototypes
{
	[self doReloadPrototypes];
	if ([self.files count] == 0) {
		[FileHelper copySamples];
		[self doReloadPrototypes];
	}
}

-(void) popCreateNew:(NSString*)filename
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		[self.createNewPopover dismissPopoverAnimated:YES];
		self.createNewPopover = nil;
	}
	else
	{
		[self.navigationController popViewControllerAnimated:(filename==nil)];
	}
	if (filename != nil)
	{
		[self showFile:filename];
	}
}

-(void) popDeleteFile
{
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		[self.createNewPopover dismissPopoverAnimated:YES];
		self.createNewPopover = nil;
	}
	else
	{
		[self.navigationController popViewControllerAnimated:YES];
	}
}

-(void) doReloadPrototypes
{
	self.files = [NSMutableArray new];
	self.loc = [NSMutableArray new];
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:path];
	NSString* filename;
	while ((filename = [dirEnum nextObject])) {
		[self.files addObject:filename];
		NSString* loc = [FileHelper countLoc:filename];
		[self.loc addObject:loc];
		[dirEnum skipDescendents];
	}
	[self.tableView reloadData];
	if ([self.editController.title length] > 0)
	{
		[self.editController updateEditor];
		if ([self.editController.title length] > 0)
		{
			int lFileIndex = [self.files indexOfObject:self.editController.title];
			[self.tableView selectRowAtIndexPath:[NSIndexPath indexPathForRow:lFileIndex inSection:0] animated:NO scrollPosition:UITableViewScrollPositionNone];
		}
	}
}

-(void) updateLoc
{
	if ([self.editController.title length] <= 0)
	{
		return;
	}
	NSString* loc = [FileHelper countLoc:self.editController.title];
	int lFileIndex = [self.files indexOfObject:self.editController.title];
	[self.loc setObject:loc atIndexedSubscript:lFileIndex];
	NSIndexPath* indexPath = [NSIndexPath indexPathForRow:lFileIndex inSection:0];
	[self.tableView reloadRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationBottom];
	if (_allowUpdateSelection)
	{
		[self.tableView selectRowAtIndexPath:indexPath animated:NO scrollPosition:UITableViewScrollPositionNone];
	}
}

-(void) popoverControllerDidDismissPopover:(UIPopoverController*)popoverController
{
	const bool lAllowRemoteSync = self.settingsController.allowRemote.isOn;
	v_override(UiCure::GetSettings(), "Simulator.AllowRemoteSync", lAllowRemoteSync);
	self.createNewPopover = nil;
	self.settingsController = nil;
}

-(void) doneiPhoneSettings
{
	const bool lAllowRemoteSync = self.settingsController.allowRemote.isOn;
	v_override(UiCure::GetSettings(), "Simulator.AllowRemoteSync", lAllowRemoteSync);
	[self.navigationController dismissViewControllerAnimated:YES completion:nil];
	self.settingsController = nil;
}

- (void)settings
{
	self.settingsController = [SettingsViewController new];
	self.settingsController.title = @"Settings";
	UINavigationController* nav = [[UINavigationController alloc] initWithRootViewController:self.settingsController];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		self.createNewPopover = [[UIPopoverController alloc] initWithContentViewController:nav];
		self.createNewPopover.popoverContentSize = CGSizeMake(320,280);
		self.createNewPopover.delegate = self;
		[self.createNewPopover presentPopoverFromBarButtonItem:self.navigationItem.leftBarButtonItem permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	}
	else
	{
		UIBarButtonItem* doneButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemDone target:self action:@selector(doneiPhoneSettings)];
		self.settingsController.navigationItem.rightBarButtonItem = doneButton;
		nav.modalTransitionStyle = UIModalTransitionStyleFlipHorizontal;
		[self.navigationController presentViewController:nav animated:YES completion:nil];
	}
}

- (void)createNew
{
	CreateNewViewController* newController = [CreateNewViewController new];
	newController.title = @"New Prototype";
	newController.parent = self;
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{
		UINavigationController* nav = [[UINavigationController alloc] initWithRootViewController:newController];
		self.createNewPopover = [[UIPopoverController alloc] initWithContentViewController:nav];
		self.createNewPopover.popoverContentSize = CGSizeMake(320,216);
		[self.createNewPopover presentPopoverFromBarButtonItem:self.navigationItem.rightBarButtonItem permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
	}
	else
	{
		[self.navigationController pushViewController:newController animated:YES];
	}
}

@end

#endif // iOS
