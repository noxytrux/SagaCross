//
//  SCGameWrapper.h
//  SagaCross
//
//  Created by Marcin Małysz on 25/07/2019.
//  Copyright © 2019 Marcin Małysz. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>

NS_ASSUME_NONNULL_BEGIN

@interface SCGameWrapper : NSObject

- (void)initializeEngine:(CGSize)screenSize;

@end

NS_ASSUME_NONNULL_END
