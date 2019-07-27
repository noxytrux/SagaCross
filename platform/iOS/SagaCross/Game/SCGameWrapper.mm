//
//  SCGameWrapper.m
//  SagaCross
//
//  Created by Marcin Małysz on 25/07/2019.
//  Copyright © 2019 Marcin Małysz. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

#import "SCGameWrapper.h"
#include "SCApplication.h"
#include "SCMobileInput.h"

using namespace sc;

@interface SCGameWrapper() {

    std::shared_ptr<SCApplication> _application;
    std::shared_ptr<SCSettings> _settings;
}

@end

@implementation SCGameWrapper

- (void)initializeEngine:(CGSize)screenSize {

    //setup audio in iOS
    [self setupAudioSession];

    //get settings save path
    NSURL *documents = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory
                                                               inDomains:NSUserDomainMask] lastObject];
    NSString *path = [documents path];
    path = [path stringByAppendingPathComponent:@"settings.bin"];

    _settings = std::make_shared<SCSettings>(path.UTF8String);

    if (!_settings->load()) {

        _settings->save();
    }

    _settings->setWidht(screenSize.width);
    _settings->setHeight(screenSize.height);

    //get bundle path
    std::string rootPath = [[NSBundle mainBundle] resourcePath].UTF8String;
    rootPath += "/resource/";

    _application = std::make_shared<SCApplication>(_settings, rootPath);
    _application->setShouldUseRunLoop(false);
}

- (void)renderGame {

    if (!_application) {
        return;
    }

    _application->run();
}

- (void)handleTouch:(CGPoint)position selected:(BOOL)selected {

    auto input = std::dynamic_pointer_cast<SCMobileInput>(_application->getInput());

    input->mousePos = xVec2(position.x, position.y);
    input->selected = selected;    
}

- (void)handleMovement:(CGPoint)direction angle:(CGFloat)angle {

    auto input = std::dynamic_pointer_cast<SCMobileInput>(_application->getInput());

    input->movementCallback(xVec2(direction.x, direction.y), angle);
}

- (void)fireBullet {

    auto input = std::dynamic_pointer_cast<SCMobileInput>(_application->getInput());

    input->mouseCallback(0, 1, xVec2(0));
}

- (void)dropMine {

    auto input = std::dynamic_pointer_cast<SCMobileInput>(_application->getInput());

    input->mouseCallback(1, 1, xVec2(0));
}

- (void)setupAudioSession {

    double rate = 44100.0;
    int blockSize = 512;
    long channels = 2;
    BOOL success = false;

    AVAudioSession *session = [AVAudioSession sharedInstance];

    // Make our category 'solo' for the best chance at getting our desired settings
    // Use AVAudioSessionCategoryPlayAndRecord if you need microphone input
    success = [session setCategory:AVAudioSessionCategorySoloAmbient error:nil];
    assert(success);

    // Set our preferred rate and activate the session to test it
    success = [session setPreferredSampleRate:rate error:nil];
    assert(success);
    success = [session setActive:TRUE error:nil];
    assert(success);

    // Query the actual supported rate and max channels
    rate = [session sampleRate];
    channels = [session respondsToSelector:@selector(maximumOutputNumberOfChannels)] ? [session maximumOutputNumberOfChannels] : 2;

    // Deactivate the session so we can change parameters without route changes each time
    success = [session setActive:FALSE error:nil];
    assert(success);

    // Set the duration and channels based on known supported values
    success = [session setPreferredIOBufferDuration:blockSize / rate error:nil];
    assert(success);
    if ([session respondsToSelector:@selector(setPreferredOutputNumberOfChannels:error:)])
    {
        success = [session setPreferredOutputNumberOfChannels:channels error:nil];
        assert(success);
    }

    /*
     Set up some observers for various notifications
     */
    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionInterruptionNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         bool began = [[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] intValue] == AVAudioSessionInterruptionTypeBegan;
         NSLog(@"Interruption %@", began ? @"Began" : @"Ended");

         if (!began)
         {
             [[AVAudioSession sharedInstance] setActive:TRUE error:nil];
         }

         //TODO: interruption
     }];

    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionSilenceSecondaryAudioHintNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         bool began = [[notification.userInfo valueForKey:AVAudioSessionSilenceSecondaryAudioHintTypeKey] intValue] == AVAudioSessionSilenceSecondaryAudioHintTypeBegin;
         NSLog(@"Silence secondary audio %@", began ? @"Began" : @"Ended");
     }];

    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionRouteChangeNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSNumber * reason = [[notification userInfo] valueForKey:AVAudioSessionRouteChangeReasonKey];

         AVAudioSessionRouteDescription * desc = [[notification userInfo] valueForKey:AVAudioSessionRouteChangePreviousRouteKey];

         AVAudioSessionPortDescription * oldOutput = [desc outputs][0];
         AVAudioSessionPortDescription * newOutput = [[[AVAudioSession sharedInstance] currentRoute] outputs][0];

         const char *reasonString = NULL;
         switch ([reason intValue])
         {
             case AVAudioSessionRouteChangeReasonNewDeviceAvailable:         reasonString = "New Device Available";              break;
             case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:       reasonString = "Old Device Unavailable";            break;
             case AVAudioSessionRouteChangeReasonCategoryChange:             reasonString = "Category Change";                   break;
             case AVAudioSessionRouteChangeReasonOverride:                   reasonString = "Override";                          break;
             case AVAudioSessionRouteChangeReasonWakeFromSleep:              reasonString = "Wake From Sleep";                   break;
             case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory: reasonString = "No Suitable Route For Category";    break;
             case AVAudioSessionRouteChangeReasonRouteConfigurationChange:   reasonString = "Configuration Change";              break;
             default:                                                        reasonString = "Unknown";
         }

         NSLog(@"Output route has changed from %dch %@ to %dch %@ due to '%s'", (int)[[oldOutput channels] count], [oldOutput portName], (int)[[newOutput channels] count], [newOutput portName], reasonString);
     }];

    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereLostNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Media services were lost");
     }];

    [[NSNotificationCenter defaultCenter] addObserverForName:AVAudioSessionMediaServicesWereResetNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Media services were reset");
     }];

    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application did become active");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application will resign active");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application did enter background");
     }];
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillEnterForegroundNotification object:nil queue:nil usingBlock:^(NSNotification *notification)
     {
         NSLog(@"Application will enter foreground");
     }];


    /*
     Activate the audio session
     */
    success = [session setActive:TRUE error:nil];
    assert(success);
}

@end
