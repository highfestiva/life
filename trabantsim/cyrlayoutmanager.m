//
//  CYRLayoutManager.h
//
//  Version 0.4.0
//
//  Created by Illya Busigin on 01/05/2014.
//  Copyright (c) 2014 Cyrillian, Inc.
//
//  Distributed under MIT license.
//  Get the latest version from here:
//
//  https://github.com/illyabusigin/CYRTextView
//  Original implementation taken from: https://github.com/alldritt/TextKit_LineNumbers
//
// The MIT License (MIT)
//
// Copyright (c) 2014 Cyrillian, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "../lepra/include/lepratarget.h"
#ifdef LEPRA_IOS
#import "CYRLayoutManager.h"


static CGFloat kMinimumGutterWidth = 18.f;


@interface CYRLayoutManager ()

@property (nonatomic, assign) CGFloat gutter_width;
@property (nonatomic, assign) UIEdgeInsets line_area_inset;

@property (nonatomic) NSUInteger para_location;
@property (nonatomic) NSUInteger para_number;

@end

@implementation CYRLayoutManager

#pragma mark - Initialization & Setup

- (instancetype)init {
    self = [super init];

    if (self) {
        [self _commonSetup];
    }

    return self;
}

- (instancetype)initWithCoder:(NSCoder *)aDecoder {
    self = [super initWithCoder:aDecoder];

    if (self) {
        [self _commonSetup];
    }

    return self;
}

- (void)_commonSetup {
    self.gutter_width = kMinimumGutterWidth;
    self.selected_range = NSMakeRange(0, 0);

    self.line_area_inset = UIEdgeInsetsMake(0, 2, 0, 1);
    self.line_number_color = [UIColor grayColor];
    self.line_number_font = [UIFont systemFontOfSize:10.0f];
    self.selected_line_number_color = [UIColor colorWithWhite:0.9 alpha:1];
}


#pragma mark - Convenience

- (CGRect)paragraphRectForRange:(NSRange)range {
    range = [self.textStorage.string paragraphRangeForRange:range];
    range = [self glyphRangeForCharacterRange:range actualCharacterRange:NULL];

    CGRect rect_ = [self lineFragmentRectForGlyphAtIndex:range.location effectiveRange:NULL];
    CGRect endRect = [self lineFragmentRectForGlyphAtIndex:range.location + range.length - 1 effectiveRange:NULL];

    CGRect paragraphRectForRange = CGRectUnion(rect_, endRect);
    paragraphRectForRange = CGRectOffset(paragraphRectForRange, _gutter_width, 1);

    return paragraphRectForRange;
}

- (NSUInteger) _paraNumberForRange:(NSRange) charRange {
    //  NSString does not provide a means of efficiently determining the paragraph number of a range of text.  This code
    //  attempts to optimize what would normally be a series linear searches by keeping track of the last paragraph number
    //  found and uses that as the starting point for next paragraph number search.  This works (mostly) because we
    //  are generally asked for continguous increasing sequences of paragraph numbers.  Also, this code is called in the
    //  course of drawing a pagefull of text, and so even when moving back, the number of paragraphs to search for is
    //  relativly low, even in really long bodies of text.
    //
    //  This all falls down when the user edits the text, and can potentially invalidate the cached paragraph number which
    //  causes a (potentially lengthy) search from the beginning of the string.

    if (charRange.location == self.para_location)
        return self.para_number;
    else if (charRange.location < self.para_location) {
        //  We need to look backwards from the last known paragraph for the new paragraph range.  This generally happens
        //  when the text in the UITextView scrolls downward, revaling paragraphs before/above the ones previously drawn.

        NSString* s = self.textStorage.string;
        __block NSUInteger number = self.para_number;

        [s enumerateSubstringsInRange:NSMakeRange(charRange.location, self.para_location - charRange.location)
                              options:NSStringEnumerationByParagraphs |
         NSStringEnumerationSubstringNotRequired |
         NSStringEnumerationReverse
                           usingBlock:^(NSString *substring, NSRange substringRange, NSRange enclosingRange, BOOL *stop){
                               if (enclosingRange.location <= charRange.location) {
                                   *stop = YES;
                               }
                               --number;
                           }];

        self.para_location = charRange.location;
        self.para_number = number;

        return number;
    } else {
        //  We need to look forward from the last known paragraph for the new paragraph range.  This generally happens
        //  when the text in the UITextView scrolls upwards, revealing paragraphs that follow the ones previously drawn.

        NSString* s = self.textStorage.string;
        __block NSUInteger number = self.para_number;

        [s enumerateSubstringsInRange:NSMakeRange(self.para_location, charRange.location - self.para_location)
                              options:NSStringEnumerationByParagraphs | NSStringEnumerationSubstringNotRequired
                           usingBlock:^(NSString *substring, NSRange substringRange, NSRange enclosingRange, BOOL *stop){
                               if (enclosingRange.location >= charRange.location) {
                                   *stop = YES;
                               }
                               ++number;
                           }];

        self.para_location = charRange.location;
        self.para_number = number;

        return number;
    }
}


#pragma mark - Layouting

- (void)processEditingForTextStorage:(NSTextStorage *)textStorage edited:(NSTextStorageEditActions)editMask range:(NSRange)newCharRange changeInLength:(NSInteger)delta invalidatedRange:(NSRange)invalidatedCharRange {
    [super processEditingForTextStorage:textStorage edited:editMask range:newCharRange changeInLength:delta invalidatedRange:invalidatedCharRange];

    if (invalidatedCharRange.location < self.para_location) {
        //  When the backing store is edited ahead the cached paragraph location, invalidate the cache and force a complete
        //  recalculation.  We cannot be much smarter than this because we don't know how many paragraphs have been deleted
        //  since the text has already been removed from the backing store.
        self.para_location = 0;
        self.para_number = 0;
    }
}


#pragma mark - Drawing

- (void) drawBackgroundForGlyphRange:(NSRange)glyphsToShow atPoint:(CGPoint)origin {
    [super drawBackgroundForGlyphRange:glyphsToShow atPoint:origin];

    //  Draw line numbers.  Note that the background for line number gutter is drawn by the LineNumberTextView class.
    NSDictionary* atts = @{NSFontAttributeName : _line_number_font ,
                           NSForegroundColorAttributeName : _line_number_color};

    [self enumerateLineFragmentsForGlyphRange:glyphsToShow
                                   usingBlock:^(CGRect rect, CGRect usedRect, NSTextContainer *textContainer, NSRange glyphRange, BOOL *stop) {
                                       NSRange charRange = [self characterRangeForGlyphRange:glyphRange actualGlyphRange:nil];
                                       NSRange _range = [self.textStorage.string paragraphRangeForRange:charRange];

                                       BOOL cursor_rect_ = NSLocationInRange(_selected_range.location, _range);

                                       if (cursor_rect_) {
                                           CGContextRef context = UIGraphicsGetCurrentContext();
                                           CGRect cursorRect = CGRectMake(0, usedRect.origin.y + 1, _gutter_width, usedRect.size.height);

                                           CGContextSetFillColorWithColor(context, _selected_line_number_color.CGColor);
                                           CGContextFillRect(context, cursorRect);
                                       }

                                       //   Only draw line numbers for the paragraph's first line fragment.  Subsequent fragments are wrapped portions of the paragraph and don't get the line number.
                                       if (charRange.location == _range.location) {
                                           CGRect gutterRect = CGRectOffset(CGRectMake(0, rect.origin.y, _gutter_width, rect.size.height), origin.x, origin.y);
                                           NSUInteger number = [self _paraNumberForRange:charRange];
                                           NSString* ln = [NSString stringWithFormat:@"%ld", (unsigned long) number + 1];
                                           CGSize size = [ln sizeWithAttributes:atts];

                                           [ln drawInRect:CGRectOffset(gutterRect, CGRectGetWidth(gutterRect) - _line_area_inset.right - size.width - _gutter_width, (CGRectGetHeight(gutterRect) - size.height) / 2.0)
                                           withAttributes:atts];
                                       }

                                   }];
}

@end

#endif // iOS

