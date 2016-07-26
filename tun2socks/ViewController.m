 //
//  ViewController.m
//  tun2socks
//
//  Created by LEI on 7/24/16.
//  Copyright © 2016 TouchingApp. All rights reserved.
//

#import "ViewController.h"
#import "libtun2socks.h"

static void tun_outpub_cb(char *data, int data_len) {
    NSLog(@"1");
}

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        start_tun2socks(1, 0, tun_outpub_cb);
    });
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(1 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        char buf[80] = {
            0x45 ,0x00, 0x00 ,0x40 ,0x35 ,0xfa ,0x40 ,0x00 ,0x40 ,0x06 ,0xc4 ,0xb3 ,0x0a ,0x56 ,0xc4 ,0x53 ,
            0x44 ,0xe8, 0x2c ,0x79 ,0xd5 ,0x6e ,0x01 ,0xbb ,0xd8 ,0x5d ,0x47 ,0xcd ,0x00 ,0x00 ,0x00 ,0x00 ,
            0xb0 ,0x02, 0xff ,0xff ,0xd5 ,0x81 ,0x00 ,0x00 ,0x02 ,0x04 ,0x05 ,0xb4 ,0x01 ,0x03 ,0x03 ,0x05 ,
            0x01 ,0x01, 0x08 ,0x0a ,0x14 ,0xee ,0x15 ,0x2e ,0x00 ,0x00 ,0x00 ,0x00 ,0x04 ,0x02 ,0x00 ,0x00};
        tun_input(buf, 64);
    });
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
