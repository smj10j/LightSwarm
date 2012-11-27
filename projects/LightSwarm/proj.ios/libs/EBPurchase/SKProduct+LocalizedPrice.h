//
//  SKProduct+LocalizedPrice.h
//  Penguin Rescue
//
//  Created by Stephen Johnson on 11/21/12.
//  Copyright (c) 2012 Conquer LLC. All rights reserved.
//

#import <StoreKit/StoreKit.h>

@interface SKProduct (LocalizedPrice)

@property (nonatomic, readonly) NSString *localizedPrice;


@end
