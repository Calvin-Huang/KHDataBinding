//
//  KHImageDownloader.h
//
//  Created by GevinChen on 2015/12/13.
//  Copyright © 2015年 omg. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "KHCell.h"

NS_ASSUME_NONNULL_BEGIN

@interface KHImageDownloader : NSObject
{
    //  圖片快取
    NSMutableDictionary *_imageCache;
    NSMutableDictionary *_imageNamePlist;
    NSMutableArray *_imageDownloadTag;
    NSString *plistPath;
    
    NSMutableDictionary *_listeners;
}

@property (nonatomic) BOOL debugLog;

+(KHImageDownloader*)instance;

//  下載圖片
- (void)loadImageURL:(NSString *)urlString cellLinker:(KHPairInfo*)cellLinker completed:(void (^)(UIImage *,NSError*))completed;

- (void)removeCache:(NSString*)key;

- (void)removeDiskCache:(NSString*)key;

- (void)clearAllCache;

- (void)saveToCache:(nonnull UIImage*)image key:(NSString*)key;

- (nullable UIImage*)getImageFromCache:(NSString*)key;

- (NSString*)getCachePath;
- (void)saveImageToDisk:(nonnull UIImage*)image key:(NSString*)key;

- (UIImage*)getImageFromDisk:(NSString*)key;
//  取得某網址的圖片快取路徑
- (NSString*)getImageFileName:(NSString*)key;

//  把舊的刪掉
- (void)updateImageDiskCache;

@end

NS_ASSUME_NONNULL_END
