//
//  KHDataBinder.h
//
//  Created by GevinChen on 2015/9/26.
//  Copyright (c) 2015年 GevinChen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "KVCModel.h"
#import "KHCell.h"
#import "NSMutableArray+KHSwizzle.h"
#import "KHImageDownloader.h"

/**
 *  Data binder
 *  使用上有三個角色
 *  Data Model : 純粹資料物件
 *  Cell Adapter : cell 的介接物件，可以當作是 cell 的代理人，因為 cell 是 reuse，所以一些設定資料不能放在 cell，因此會放在 cell adapter
 *  UITableViewCell or UICollectionViewCell
 *
 *
 */
#define weakRef( var ) __weak typeof(var) weak_##var = var

@protocol KHTableViewDelegate
@optional
- (void)tableView:(nonnull UITableView*)tableView didSelectRowAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)tableViewRefreshHead:(nonnull UITableView*)tableView;
- (void)tableViewRefreshFoot:(nonnull UITableView*)tableView;
@end

@protocol KHCollectionViewDelegate
@optional
- (void)collectionView:(nonnull UICollectionView*)collectionView didSelectItemAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)collectionViewRefreshHead:(nonnull UICollectionView*)collectionView;
- (void)collectionViewRefreshFoot:(nonnull UICollectionView*)collectionView;
@end


@interface KHDataBinder : NSObject < KHArrayObserveDelegate >
{
    //  記錄 CKHObserverArray
    NSMutableArray *_sectionArray;
    
    //  記錄 cell - model 介接物件，linker 的數量會跟 model 一樣
    NSMutableDictionary *_linkerDic;
    
    //  記錄 model bind cell
    NSMutableDictionary *_cellClassDic;
    
    //  因為有很多個 cell ，且是 reuse 的
    //  所以把每個 cell 裡的 ui control 轉為用一個 key 代替
    //  在 controller 的時候，就對那個 key 做觸發事件的指定

    //  KHCellEventHandleData 的 array
    NSMutableArray *_cellUIEventHandlers;
    
    //  refresh
    UIScrollView *refreshScrollView;
    NSAttributedString *refreshTitle1;
    NSAttributedString *refreshTitle2;
    int refreshState;

    
}
// pull down to refresh
@property (nonatomic,copy) NSString *headTitle;
@property (nonatomic,copy) NSString *footTitle;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshHeadControl;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshFootControl;
@property (nonatomic) BOOL refreshHeadEnabled;
@property (nonatomic) BOOL refreshFootEnabled;
@property (nonatomic) NSTimeInterval lastUpdate;

#pragma mark - UIRefreshControl

- (void)endRefreshing;


#pragma mark - Bind Array

//  生成一個已綁定的 array
- (nonnull NSMutableArray*)createBindArray;

//  生成一個已綁定的 array，並且把資料填入
- (nonnull NSMutableArray*)createBindArrayFromNSArray:(nullable NSArray*)array;

//  綁定一個 array
- (void)bindArray:(nonnull NSMutableArray*)array;

//  解綁定一個array
- (void)deBindArray:(nonnull NSMutableArray*)array;

//  取得一個已綁定的 array
- (nullable NSMutableArray*)getArray:(NSInteger)section;

//  取得有幾個 section (array)
- (NSInteger)sectionCount;

//  告訴 bind helper，遇到什麼 model，要用什麼 cell  來顯示
//- (void)bindModel:(nonnull Class)modelClass cell:(nonnull Class)cellClass;
- (void)registerCell:(nonnull Class)cellClass;

//  用  model class 來找對應的 cell class
- (nullable NSString*)getCellName:(nonnull Class)modelClass;

//  透過 model 取得 cell
- (nullable id)getCellByModel:(nonnull id)model;

//  取得某個 model 的 cell 介接物件
//- (nullable KHModelCellLinker*)cellLinkerWithModel:(nonnull id)model;

//  透過 cell 取得 data model
- (nullable id)getDataModelWithCell:(nonnull id)cell;

//  取得某 model 的 index
- (nullable NSIndexPath*)indexPathOfModel:(nonnull id)model;

//  取得某 cell 的 index
- (nullable NSIndexPath*)indexPathOfCell:(nonnull id)cell;

//  更新 model
- (void)updateModel:(nonnull id)model;

//  重載 // override by subclass
- (void)reloadData;

#pragma mark - UIControl Handle

//  設定當 cell 裡的 ui control 被按下發出事件時，觸發的 method
//  UI Event  SEL 跟原本的不同，要求要 :(id)sender :(id)model
- (void)addEvent:(UIControlEvents)event cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pname handler:(void(^)(id sender, id model))eventHandleBlock;

//
- (void)removeEvent:(UIControlEvents)event cell:(nonnull Class)cellClass propertyName:(nonnull NSString*)pName;

@end

@interface KHTableDataBinder : KHDataBinder <UITableViewDelegate, UITableViewDataSource, UIScrollViewDelegate>
{
    //
    BOOL _hasInit;
    
    NSString *_cellHeightKeyword;
    
    NSMutableArray *_headerTitles;

    NSMutableArray *_headerViews;
    
    NSMutableArray *_footerTitles;
    
    NSMutableArray *_footerViews;
    
}

@property (nullable,nonatomic) UITableView* tableView;
@property (nullable,nonatomic) id delegate;

//  header
@property (nullable,nonatomic) UIColor *headerBgColor;
@property (nullable,nonatomic) UIColor *headerTextColor;
@property (nullable,nonatomic) UIFont  *headerFont;
//@property (nullable,nonatomic) NSArray *headerTitles;
@property (nonatomic) float    headerHeight;
//@property (nullable,nonatomic) NSArray *headerViews;

//  footer
@property (nullable,nonatomic) UIColor *footerBgColor;
@property (nullable,nonatomic) UIColor *footerTextColor;
@property (nullable,nonatomic) UIFont  *footerFont;
//@property (nullable,nonatomic) NSArray *footerTitles;
@property (nonatomic) float    footerHeight;
//@property (nullable,nonatomic) NSArray *footerViews;

//- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView;
- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView delegate:(nullable id)delegate registerClass:(nullable Class)cellClass,...;;

//  table view cell height
- (float)getCellHeightWithModel:(nonnull id)model;
- (void)setCellHeight:(float)cellHeight model:(nonnull id)model;

//  設定 header title
- (void)setHeaderTitle:(nonnull NSString *)headerTitle atSection:(NSUInteger)section;

//  設定 header view
- (void)setHeaderView:(nonnull UIView*)view atSection:(NSUInteger)section;
- (void)setHeaderTitles:(NSArray*)titles;
- (void)setHeaderViews:(NSArray*)views;

//  設定 footer title
- (void)setFooterTitle:(nonnull NSString *)footerTitle atSection:(NSUInteger)section;

//  設定 footer view
- (void)setFooterView:(nonnull UIView*)view atSection:(NSUInteger)section;
- (void)setFooterTitles:(NSArray*)titles;
- (void)setFooterViews:(NSArray*)views;



/*
 Gevin note :
 原本打算讓 table view binder 可以設定預設的 cell height accessoryType 之類的屬性值
 但是想到說，一個 table 裡可能會有兩種以上的 cell ，所以預設值其實不實用
 
 另外想到一點就是，最原本用 delegate 來定義 cell 的寫法，其實應該也是在那個 delegate 裡去設定
 dequeue 的 cell 的屬性值
 相對於我的寫法，就是把那一段移到 onload 裡
 但是 cell height 的問題比較大，因為 cell height 會在 onload 之前先執行
 
 */

@end



@interface KHCollectionDataBinder : KHDataBinder <UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout, UIScrollViewDelegate>
{

    //  用來判斷說是否已經初始完成，不然在初始前就做 insert 的動畫，會掛掉
    BOOL _hasInit;
    
    NSString *_cellSizeKeyword;
    
    NSMutableArray *_headerModelList;
    
    NSMutableArray *_footerModelList;
    
    //  以 model name 為 key
    NSMutableDictionary *_reusableViewDic;
}

@property (nonnull,nonatomic) UICollectionView *collectionView;
@property (nullable,nonatomic) id delegate;
@property (nullable,nonatomic) UICollectionViewFlowLayout *layout;

- (nonnull instancetype)initWithCollectionView:(nonnull UICollectionView*)collectionView delegate:(nullable id)delegate registerClass:(nullable Class)cellClass,...;

- (CGSize)getCellSizeWithModel:(nonnull id)model;
- (void)setCellSize:(CGSize)cellSize model:(nonnull id)model;


- (void)registerReusableView:(nonnull Class)reusableViewClass kind:(NSString*)kind;
- (void)setHeaderModel:(nonnull id)headerModel atIndex:(NSInteger)sectionIndex;
- (void)setHeaderModels:(nonnull NSArray*)headerModels;

- (void)setFooterModel:(nonnull id)headerModel atIndex:(NSInteger)sectionIndex;
- (void)setFooterModels:(nonnull NSArray*)headerModels;

@end

