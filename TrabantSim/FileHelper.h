
// Author: Jonas Byström
// Copyright (c) Pixel Doctrine



#import <UIKit/UIKit.h>

@interface FileHelper : NSObject
+(NSString*) fullPath:(NSString*)filename;
+(bool) fileExists:(NSString*)filename;
+(void) copySamples;
+(bool) hasOriginal:(NSString*)filename;
+(void) restoreSample:(NSString*)filename;
@end
