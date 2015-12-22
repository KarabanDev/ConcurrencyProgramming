//
//  ViewController.m
//  ExampleOfConcurrency
//
//  Created by Andrey Karaban on 21.12.15.
//  Copyright © 2015 Andrey Karaban. All rights reserved.
//
#import <dispatch/dispatch.h>
#import "EOCMainViewController.h"

@interface EOCMainViewController ()
{
    dispatch_queue_t myConcurrentQueue;
    dispatch_queue_t mySerialQueue;
    
     __weak IBOutlet UISlider *ibSlider;
    __weak IBOutlet UILabel *ibSliderValue;
    
    __weak IBOutlet UISwitch *ibSwitcher;
    
    IBOutletCollection(UIImageView) NSArray *ibImagesViewForURL;
    __weak IBOutlet UISegmentedControl *ibSegmentedControl;
    
    __weak IBOutlet UILabel *ibSerial_concurrentLabel;
}

- (IBAction)ibaSliderValueChanged:(id)sender;
- (IBAction)ibaStartBtnPressed:(id)sender;
- (IBAction)ibaSwitcherChanged:(id)sender;
- (IBAction)ibaSegmentChange:(id)sender;

@end

@implementation EOCMainViewController
{
    NSArray *urls;
}
- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Do any additional setup after loading the view, typically from a nib.
    
    urls = @[@"http://www.planetware.com/photos-large/F/france-paris-eiffel-tower.jpg", @"http://adriatic-lines.com/wp-content/uploads/2015/04/canal-of-Venice.jpg", @"http://algoos.com/wp-content/uploads/2015/08/ireland-02.jpg", @"http://bdo.se/wp-content/uploads/2014/01/Stockholm1.jpg"];
    
    ibSliderValue.text = [NSString stringWithFormat:@"%.2f", (ibSlider.value * 100)];
    
 }

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)ibaSliderValueChanged:(id)sender {
    
    ibSliderValue.text = [NSString stringWithFormat:@"%.2f", (ibSlider.value * 100)];
}

- (IBAction)ibaStartBtnPressed:(id)sender {
    if ([ibSegmentedControl selectedSegmentIndex])
        [self setDownloadedviaNSOperationQueuesImages];
    else
        [self setDownloadedviaGCDImages];
}

- (IBAction)ibaSwitcherChanged:(id)sender {
    
    for(UIImageView *imgView in ibImagesViewForURL)
        imgView.image = nil;
}

- (IBAction)ibaSegmentChange:(id)sender {
    
    for(UIImageView *imgView in ibImagesViewForURL)
        imgView.image = nil;
    
    ibSwitcher.hidden = [ibSegmentedControl selectedSegmentIndex];
    ibSerial_concurrentLabel.hidden = [ibSegmentedControl selectedSegmentIndex];
}

#pragma mark - DISPATCH QUEUES with GCD

- (void)setDownloadedviaGCDImages
{
    int index = 0;
    
    if (ibSwitcher.isOn)
    {
            myConcurrentQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
        for(UIImageView *imgView in ibImagesViewForURL){
            dispatch_async(myConcurrentQueue, ^{
                NSURL *url = [NSURL URLWithString:urls[index]];
                UIImage *img = [UIImage imageWithData:[NSData dataWithContentsOfURL:url]];
                dispatch_async(dispatch_get_main_queue(), ^{
                    imgView.image = img;
                });
            });
            
            if (index <[urls count])
                index++;
            else
                return;
        }
 
    }else
    {
        NSString *str = @"uniqueIdentifier";
        const char *identifier = [str cStringUsingEncoding:NSASCIIStringEncoding];
        mySerialQueue = dispatch_queue_create(identifier, DISPATCH_QUEUE_SERIAL);

        for(UIImageView *imgView in ibImagesViewForURL){
            dispatch_async(mySerialQueue, ^{
                NSURL *url = [NSURL URLWithString:urls[index]];
                UIImage *img = [UIImage imageWithData:[NSData dataWithContentsOfURL:url]];
                dispatch_async(dispatch_get_main_queue(), ^{
                    imgView.image = img;
                });
            });
            
            if (index <[urls count])
                index++;
            else
                return;

        }

    }
}

#pragma mark - Operation Queues
- (void)setDownloadedviaNSOperationQueuesImages{
    NSLog(@"%s", __PRETTY_FUNCTION__);
    NSOperationQueue *myOperationQueue = [NSOperationQueue new];
    
    for (int i = 0; i<[urls count]; i++) {
        
        NSBlockOperation *blockOperation = [NSBlockOperation blockOperationWithBlock:^{
                        NSURL *url = [NSURL URLWithString:urls[i]];
                        UIImage *downloadedImage = [UIImage imageWithData:[NSData dataWithContentsOfURL:url]];
                        [[NSOperationQueue mainQueue] addOperationWithBlock:^{
                            [ibImagesViewForURL[i] setImage:downloadedImage];

        }];
        }];
        
       // blockOperation.completionBlock = NSLog(@"Operation number %d finished", i);
        [myOperationQueue addOperation:blockOperation];
//        [myOperationQueue addOperationWithBlock:^{
//            NSURL *url = [NSURL URLWithString:urls[i]];
//            UIImage *downloadedImage = [UIImage imageWithData:[NSData dataWithContentsOfURL:url]];
//            [[NSOperationQueue mainQueue] addOperationWithBlock:^{
//                [ibImagesViewForURL[i] setImage:downloadedImage];
//            }];
//        }];
        
    }
}

@end
