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
- (void)renderGame;
- (void)handleTouch:(CGPoint)position selected:(BOOL)selected;
- (void)handleMovement:(CGPoint)direction angle:(CGFloat)angle;
- (void)fireBullet;
- (void)dropMine;

@end

NS_ASSUME_NONNULL_END
