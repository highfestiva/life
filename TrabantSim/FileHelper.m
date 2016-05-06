
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#include "../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS
#import "FileHelper.h"



@implementation FileHelper

+(NSString*) fullPath:(NSString*)filename {
	NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* path = [paths objectAtIndex:0];
	return [path stringByAppendingPathComponent:filename];
}

+(bool) fileExists:(NSString*)filename {
	NSString* full = [FileHelper fullPath:filename];
	return [[NSFileManager defaultManager] fileExistsAtPath:full];
}

+(void) copySamples {
	NSString* path_ = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSDirectoryEnumerator* dirEnum = [[NSFileManager defaultManager] enumeratorAtPath:path_];
	NSString* filename;
	while ((filename = [dirEnum nextObject])) {
		if ([filename hasSuffix:@".py"]) {
			[self restoreSample:filename];
		}
	}
}

+(bool) hasOriginal:(NSString*)filename {
	NSString* path_ = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSString* file_ = [path_ stringByAppendingPathComponent:filename];
	return [[NSFileManager defaultManager] fileExistsAtPath:file_];
}

+(void) restoreSample:(NSString*)filename {
	NSString* path_ = [[[NSBundle mainBundle] bundlePath] stringByAppendingPathComponent:@"/pylib/prototypes"];
	NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	NSString* dstPath = [docPaths objectAtIndex:0];
	NSString* file_ = [path_ stringByAppendingPathComponent:filename];
	NSString* dstFile = [dstPath stringByAppendingPathComponent:filename];
	[[NSFileManager defaultManager] removeItemAtPath:dstFile error:nil];
	NSString* contents = [NSString stringWithContentsOfFile:file_ encoding:NSUTF8StringEncoding error:nil];
	contents = [contents stringByReplacingOccurrencesOfString:@"#dummycomment" withString:@"#!/usr/bin/env python3"];
	[[NSFileManager defaultManager] createFileAtPath:dstFile contents:[contents dataUsingEncoding:NSUTF8StringEncoding] attributes:nil];
}

+(NSString*) countLoc:(NSString *)filename {
	NSString* filepath = [FileHelper fullPath:filename];
	NSString* code = [NSString stringWithContentsOfFile:filepath encoding:NSUTF8StringEncoding error:nil];
	int loc = 0;
	NSCharacterSet* whitespace = [NSCharacterSet whitespaceAndNewlineCharacterSet];
	NSArray* lines = [code componentsSeparatedByCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"\n"]];
	for (NSString* line in lines) {
		NSString* strippedLine_ = [[line componentsSeparatedByCharactersInSet:whitespace] componentsJoinedByString:@""];
		if (![strippedLine_ hasPrefix:@"#"] && [strippedLine_ length] > 0) {
			++loc;
		}
	}
	return [NSString stringWithFormat:@"%i loc",loc];
}

@end

#endif // iOS
