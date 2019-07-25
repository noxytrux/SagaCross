//
//  SCGameWrapper.m
//  SagaCross
//
//  Created by Marcin Małysz on 25/07/2019.
//  Copyright © 2019 Marcin Małysz. All rights reserved.
//

#import "SCGameWrapper.h"
#include "SCApplication.h"

using namespace sc;

@interface SCGameWrapper() {

    std::shared_ptr<SCApplication> _application;
}

@end

@implementation SCGameWrapper

- (void)initializeEngine:(CGSize)screenSize {

    //get bundle path here:

    //construction on mobile should get screensize

    _application = std::make_shared<SCApplication>();
}

@end
