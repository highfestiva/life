
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../Lepra/Include/LepraTarget.h"
#ifdef LEPRA_IOS
#import "FileHelper.h"



@implementation FileHelper

+(NSString*) fullPath:(NSString*)filename
{
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	return [path stringByAppendingPathComponent:filename];
}

+(bool) fileExists:(NSString*)filename
{
	NSString* full = [FileHelper fullPath:filename];
	return [[NSFileManager defaultManager] fileExistsAtPath:full];
}

+(void) copySamples
{
	NSString* srcPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:srcPath];
	NSString* filename;
	while ((filename = [dirEnum nextObject])) {
		if ([filename hasSuffix:@".py"]) {
			[self restoreSample:filename];
		}
	}
}

+(bool) hasOriginal:(NSString*)filename
{
	NSString* srcPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSString* srcFile = [srcPath stringByAppendingPathComponent:filename];
	return [[NSFileManager defaultManager] fileExistsAtPath:srcFile];
}

+(void) restoreSample:(NSString*)filename
{
	NSString* srcPath = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* dstPath = [docPaths objectAtIndex:0];
	NSString* srcFile = [srcPath stringByAppendingPathComponent:filename];
	NSString* dstFile = [dstPath stringByAppendingPathComponent:filename];
	NSError* error = nil;
	[[NSFileManager defaultManager] removeItemAtPath:dstFile error:nil];
	if (![[NSFileManager defaultManager] copyItemAtPath:srcFile toPath:dstFile error:&error]) {
		NSLog(@"File copy error %@: %@", error, [error userInfo]);
	}
}

@end

#endif // iOS
