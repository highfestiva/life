
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "ListViewController.h"
#import "CreateNewViewController.h"
#import "EditViewController.h"



@interface ListViewController () <UITableViewDelegate, UITableViewDataSource>

@property (nonatomic, strong) UITableView* tableView;
@property (nonatomic, strong) NSMutableArray* files;
@property (nonatomic, strong) NSMutableArray* loc;

@end



@implementation ListViewController

#pragma mark - View Lifecycle

- (void)viewDidLoad
{
	[super viewDidLoad];

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
	if ([self.tableView indexPathForSelectedRow]) {
		[self.tableView deselectRowAtIndexPath:[self.tableView indexPathForSelectedRow] animated:YES];
 	}
}

- (NSInteger) tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section {
	return [self.files count];
}

- (UITableViewCell*) tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath {
	static NSString* identifier = @"CellId";
	UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:identifier];
	if (cell == nil) {
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:identifier];
	}
	cell.textLabel.text = [self.files objectAtIndex:indexPath.row];
	cell.detailTextLabel.text = [self.loc objectAtIndex:indexPath.row];
	return cell;
}

-(void) tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath {
	EditViewController* editController = [EditViewController alloc];
	editController.title = [self.files objectAtIndex:indexPath.row];
	editController = [editController init];
	[self.navigationController pushViewController:editController animated:YES];
}

-(void) reloadPrototypes
{
	[self doReloadPrototypes];
	if ([self.files count] == 0) {
		[self copySamples];
		[self doReloadPrototypes];
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

		NSString* filepath = [path stringByAppendingPathComponent:filename];
		NSString* code = [NSString stringWithContentsOfFile:filepath encoding:NSUTF8StringEncoding error:nil];
		int loc = 0;
		NSCharacterSet* whitespace = [NSCharacterSet whitespaceAndNewlineCharacterSet];
		NSArray* lines = [code componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"\n"]];
		for (NSString* line in lines) {
			NSString* strippedLine = [[line componentsSeparatedByCharactersInSet:whitespace] componentsJoinedByString:@""];
			if (![strippedLine hasPrefix:@"#"] && [strippedLine length] > 0) {
				++loc;
			}
		}
		[self.loc addObject:[NSString stringWithFormat:@"%i loc",loc]];
		[dirEnum skipDescendents];
	}
	[self.tableView reloadData];
}

-(void) copySamples
{
	NSString* srcPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/samples"];
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:srcPath];
	NSString* filename;
	while ((filename = [dirEnum nextObject])) {
		if ([filename hasSuffix:@".py"]) {
			[self restoreSample:filename];
		}
	}
}

-(void) restoreSample:(NSString*)filename
{
	NSString* srcPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/samples"];
	NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* dstPath = [docPaths objectAtIndex:0];
	NSString* srcFile = [srcPath stringByAppendingPathComponent:filename];
	NSString* dstFile = [dstPath stringByAppendingPathComponent:filename];
	NSError* error = nil;
	if (![[NSFileManager defaultManager] copyItemAtPath:srcFile toPath:dstFile error:&error]) {
		NSLog(@"File copy error %@: %@", error, [error userInfo]);
	}
}

- (void)createNew
{
	CreateNewViewController* newController = [CreateNewViewController alloc];
	newController.title = @"New Prototype";
	newController.parent = self;
	newController = [newController init];
	[self.navigationController pushViewController:newController animated:YES];
}

@end

#endif // iOS
