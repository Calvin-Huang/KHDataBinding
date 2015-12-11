//
//  TableViewBindHelper.h
//  DataBindTest
//
//  Created by GevinChen on 2015/9/26.
//  Copyright (c) 2015年 GevinChen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "KVCModel.h"
#import "KHTableViewCell.h"
#import "NSMutableArray+KHSwizzle.h"


@interface KHImageDownloader : NSObject
{
    //  圖片快取
    NSMutableDictionary *_imageCache;
    NSMutableDictionary *_imageNamePlist;
    NSMutableArray *_imageDownloadTag;
    NSString *plistPath;
}

+(KHImageDownloader*)instance;

//  下載圖片
- (void)loadImageURL:(nonnull NSString*)urlString cell:(id)cell completed:(nonnull void (^)(UIImage *))completed;

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


@protocol KHTableViewHelperDelegate
@optional
- (void)tableView:(nonnull UITableView*)tableView didSelectRowAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)tableViewRefreshHead:(nonnull UITableView*)tableView;
- (void)tableViewRefreshFoot:(nonnull UITableView*)tableView;
@end

@protocol KHCollectionViewHelperDelegate
@optional
- (void)collectionView:(nonnull UICollectionView*)collectionView didSelectItemAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)collectionViewRefreshHead:(nonnull UICollectionView*)collectionView;
- (void)collectionViewRefreshFoot:(nonnull UICollectionView*)collectionView;
@end


@interface KHBindHelper : NSObject < KHArrayObserveDelegate >
{
    //  記錄 CKHObserverArray
    NSMutableArray *_sectionArray;
    
    //  記錄 model bind cell
    NSMutableDictionary *_modelBindMap;
    
    //  記錄 custom block
    NSMutableDictionary *_cellCreateDic;
    NSMutableDictionary *_cellLoadDic;
    
    //  因為有很多個 cell ，且是 reuse 的
    //  所以把每個 cell 裡的 ui control 轉為用一個 key 代替
    //  在 controller 的時候，就對那個 key 做觸發事件的指定

    //  KHCellEventHandleData 的 array
    NSMutableArray *_cellUIEventHandles;
    
}


#pragma mark - Bind Array

//  生成一個已綁定的 array
- (nonnull NSMutableArray*)createBindArray;

//  生成一個已綁定的 array，並且把資料填入
- (nonnull NSMutableArray*)createBindArrayFromNSArray:(nullable NSArray*)array;

//  順便把 model 與 identifier 的 mapping 傳入
- (void)bindArray:(nonnull NSMutableArray*)array;

//  取得一個已綁定的 array
- (nullable NSMutableArray*)getArray:(NSInteger)section;

//  取得有幾個 section (array)
- (NSInteger)arrayCount;

//  告訴 bind helper，遇到什麼 model，要用什麼 cell  來顯示
- (void)bindModel:(nonnull Class)modelClass cell:(nonnull Class)cellClass;

//  自訂一個 cell 的成生方式，與cell 載入 model 的方式
//  create block 會傳入一個 model，然後回傳一個 UITableViewCell 的 sub class 或是 UICollectionViewCell 的 subclass
//  load block 會傳入一個 cell instance, model instance ，然後自己轉型，把 model 的資料填入 cell
- (void)defineCell:(nonnull Class)cellClass create:(id(^)(id model))createBlock load:(void(^)(id cell, id model))loadBlock;

//  透過 model class name 取得 model 對映的 cell class name
- (nullable NSString*)getBindCellName:(NSString*)modelName;



#pragma mark - UIControl Handle

//  設定當 cell 裡的 ui control 被按下發出事件時，觸發的 method
//  UI Event  SEL 跟原本的不同，要求要 :(id)sender :(id)model
- (void)addTarget:(nonnull id)target action:(nonnull SEL)action event:(UIControlEvents)event cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pname;

//
- (void)removeTarget:(nonnull id)target action:(nullable SEL)action cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pName;

//
- (void)removeTarget:(nonnull id)target cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pName;

//
- (nullable id)getTargetByAction:(nonnull SEL)action cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pName;

#pragma mark - Image Download

- (void)loadImageURL:(nonnull NSString*)urlString cell:(id)cell completed:(nonnull void (^)(UIImage *))completed;

@end

@interface KHTableBindHelper : KHBindHelper <UITableViewDelegate, UITableViewDataSource, UIScrollViewDelegate>
{
    //  每個 section 的 title
    NSArray *_titles;

    BOOL _hasInit;
}

@property (nullable,nonatomic) UITableView* tableView;
// pull down to refresh
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshHeadControl;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshFootControl;
@property (nonatomic) BOOL refreshHeadEnabled;
@property (nonatomic) BOOL refreshFootEnabled;

@property (nullable,nonatomic) id delegate;

//  header
@property (nullable,nonatomic) UIColor *headerBgColor;
@property (nullable,nonatomic) UIColor *headerTextColor;
@property (nullable,nonatomic) UIFont  *headerFont;
@property (nonatomic) NSInteger headerHeight;
//  footer
@property (nullable,nonatomic) UIColor *footerBgColor;
@property (nullable,nonatomic) UIColor *footerTextColor;
@property (nullable,nonatomic) UIFont  *footerFont;
@property (nonatomic) NSInteger footerHeight;

- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView;
- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView delegate:(nullable id)delegate;

- (void)setHeaderTitles:(nullable NSArray*)titles;

- (void)endRefreshing;


@end



@interface KHCollectionBindHelper : KHBindHelper <UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout, UIScrollViewDelegate>
{

    //  用來判斷說是否已經初始完成，不然在初始前就做 insert 的動畫，會掛掉
    BOOL _hasInit;
}

@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshHeadControl;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshFootControl;
@property (nonnull,nonatomic) UICollectionView *collectionView;
@property (nonatomic) BOOL refreshHeadEnabled;
@property (nonatomic) BOOL refreshFootEnabled;

@property (nullable,nonatomic) id delegate;

- (nonnull UICollectionViewFlowLayout*)layout;

- (void)endRefreshing;

@end

