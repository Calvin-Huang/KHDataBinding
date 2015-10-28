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
#import "EGORefreshTableHeaderView.h"
#import "EGORefreshTableFooterView.h"
#import "KHObservableArray.h"

//@protocol HelperEventDelegate
//
//- (void)tableViewEvent:(nonnull const NSString*)event userInfo:( nullable id)userInfo;
//
//@end

@protocol TableViewHelperDelegate

@optional

- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(nonnull NSIndexPath *)indexPath;

- (void)refreshTrigger:(UITableView*)tableView;

- (void)loadMoreTrigger:(UITableView*)tableView;

@end

@interface KHTableViewBindHelper : NSObject <UITableViewDelegate, UITableViewDataSource, KHObserveArrayDelegate, EGORefreshTableDelegate, UIScrollViewDelegate >
{
    //  記錄 CKHObserverArray
    NSMutableArray *_sectionArray;
    
    //  監聽 helper 發出事件的監聽者，主要是聽取 cell 所發出的 ui event
//    NSMutableArray* _listeners;
    
//    id _target;
//    SEL _action;
    // 處理 cell 被按到時候呼叫，會固定呼叫  tableView:didSelectedRowAtIndexPath:
//    NSInvocation *_cellSelectedInvocation;
    
    //  因為有很多個 cell ，且是 reuse 的
    //  所以把每個 cell 裡的 ui control 轉為用一個 key 代替
    //  在 controller 的時候，就對那個 key 做觸發事件的指定
    
    //  記錄每個 ui tag 的 invocation，結構是 tag / NSMutableDictionary，作為 value 的 dictionary
    //  key 是 event / value 是 NSInvocation
    NSMutableDictionary *_invocationDic;
    
    //  
    NSMutableDictionary *_uiDic;
    
    
    NSArray *_titles;
    
    //  圖片快取
    NSMutableDictionary *_imageCache;
    NSMutableDictionary *_imageNamePlist;
    NSMutableArray *_imageDownloadTag;
    NSString *plistPath;
    
    //  EGO Header
    EGORefreshTableHeaderView *_refreshHeader;
    EGORefreshTableFooterView *_refreshFooter;
    BOOL _isRefresh;
}

@property (nonatomic) UITableView* tableView;
@property (nonatomic) BOOL enableRefreshHeader;
@property (nonatomic) BOOL enableRefreshFooter;
@property (nonatomic) id delegate;

- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView;

- (void)setHeaderTitles:(nullable NSArray*)titles;

//-------------------------------------------------

- (nonnull KHObservableArray*)createBindArray;

- (nonnull KHObservableArray*)createBindArrayFromNSArray:(nullable NSArray*)array;

// 順便把 model 與 identifier 的 mapping 傳入
- (void)bindArray:(nonnull KHObservableArray*)array;

- (nullable KHObservableArray*)getArray:(NSInteger)section;

//-------------------------------------------------

//- (void)addEventListener:(nonnull id)listener;
//
//- (void)removeListener:(nonnull id)listener;
//
//- (void)notify:(nonnull const NSString*)event userInfo:(nullable id)userInfo;

//-------------------------------------------------


//-------------------------------------------------

//  記錄要監聽的 ui
- (void)tagUIControl:(nonnull UIControl*)control tag:(nonnull NSString*)tag;

//  設定當 cell 裡的 ui control 被按下發出事件時，觸發的 method
//  UI Event  SEL 跟原本的不同，要求要 :(id)sender :(id)model
- (void)addTarget:(nonnull id)target action:(nonnull SEL)action event:(UIControlEvents)event forTag:(nonnull NSString*)tag;

//
- (void)removeTarget:(nonnull id)target action:(nullable SEL)action forTag:(nonnull NSString*)tag;

//
- (void)removeTarget:(nonnull id)target forTag:(nonnull NSString*)tag;

//
- (nullable id)getTargetByAction:(nonnull SEL)action forTag:(nonnull NSString*)tag;;

//--------------------------------------------------

/* 目前還沒有想到用什麼方式再更抽象化這種非同步的工作，所以就先針對目的來寫，直接表示是要處理圖片相關的
 */
- (void)loadImageURL:(nonnull NSString*)urlString completed:(nonnull void (^)(UIImage *))completed;

- (void)clearCache:(NSString*)key;

- (void)clearAllCache;

- (void)saveToCache:(nonnull UIImage*)image key:(NSString*)key;

- (nullable UIImage*)getImageFromCache:(NSString*)key;

//--------------------------------------------------

- (void)refreshCompleted;

@end

