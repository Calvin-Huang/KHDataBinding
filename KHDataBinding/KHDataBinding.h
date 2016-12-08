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

@protocol KHDataBindingDelegate

@optional
- (void)tableView:(nonnull UITableView*)tableView didSelectRowAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)collectionView:(nonnull UICollectionView*)collectionView didSelectItemAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)bindingView:(nonnull id)bindingView didSelectItemAtIndexPath:(nonnull NSIndexPath *)indexPath;
- (void)bindingViewRefreshHead:(nonnull id)bindingView;
- (void)bindingViewRefreshFoot:(nonnull id)bindingView;

@end


@interface KHDataBinding : NSObject < KHArrayObserveDelegate >
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
@property (nonatomic,copy,nullable) NSString *headTitle;
@property (nonatomic,copy,nullable) NSString *footTitle;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshHeadControl;
@property (nonnull,nonatomic,readonly) UIRefreshControl *refreshFootControl;
@property (nonatomic) BOOL refreshHeadEnabled;
@property (nonatomic) BOOL refreshFootEnabled;
@property (nonatomic) NSTimeInterval lastUpdate;

- (nonnull instancetype)initWithView:(UIView* _Nonnull)view delegate:(id _Nullable)delegate registerClass:(NSArray<Class>* _Nullable)cellClasses;

#pragma mark - UIRefreshControl

- (void)endRefreshing;


#pragma mark - Bind Array

//  生成一個已綁定的 array
- (nonnull NSMutableArray*)createBindArray;

//  生成一個已綁定的 array，並且把資料填入
- (nonnull NSMutableArray*)createBindArrayFromNSArray:(NSArray*_Nullable)array;

//  綁定一個 array
- (void)bindArray:(NSMutableArray*_Nonnull)array;

//  解綁定一個array
- (void)deBindArray:(NSMutableArray* _Nonnull)array;

//  取得一個已綁定的 array
- (nullable NSMutableArray*)getArray:(NSInteger)section;

//  取得有幾個 section (array)
- (NSInteger)sectionCount;

//  override by subclass，把 cell 註冊至 tableView 或 collectionView
- (void)registerCell:(NSString* _Nonnull)cellName;

//  設定對映
- (void)setMappingModel:(Class _Nonnull)modelClass :(Class _Nonnull)cellClass;

//  設定對映，使用 block 處理
- (void)setMappingModel:(Class _Nonnull)modelClass block:( Class _Nullable(^ _Nonnull)(id _Nonnull model, NSIndexPath* _Nonnull index))mappingBlock;

//  用  model 來找對應的 cell class
- (nullable NSString*)getMappingCellNameWith:(nonnull id)model index:(NSIndexPath* _Nullable)index;

//  透過 model 取得 cell
- (nullable id)getCellByModel:(id _Nonnull)model;

//  取得某個 model 的 cell 介接物件
//- (nullable KHModelCellLinker*)cellLinkerWithModel:(nonnull id)model;

//  透過 cell 取得 data model
- (nullable id)getModelWithCell:(id _Nonnull)cell;

//  取得某 model 的 index
- (nullable NSIndexPath*)indexPathOfModel:(id _Nonnull)model;

//  取得某 cell 的 index
- (nullable NSIndexPath*)indexPathOfCell:(id _Nonnull)cell;

//  更新 model
- (void)updateModel:(id _Nonnull)model;

//  重載 // override by subclass
- (void)reloadData;

#pragma mark - UIControl Handle

//  設定當 cell 裡的 ui control 被按下發出事件時，觸發的 method
//  UI Event  SEL 跟原本的不同，要求要 :(id)sender :(id)model
- (void)addEvent:(UIControlEvents)event cell:(Class _Nonnull)cellClass propertyName:(NSString* _Nonnull)pname handler:(void(^_Nonnull)(id _Nonnull sender, id _Nonnull model))eventHandleBlock;

//
- (void)removeEvent:(UIControlEvents)event cell:(Class _Nonnull)cellClass propertyName:(NSString* _Nonnull)pName;

@end

@interface KHTableDataBinding : KHDataBinding <UITableViewDelegate, UITableViewDataSource, UIScrollViewDelegate>
{
    //
    BOOL _hasInit;
    
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
@property (nonatomic) float    headerHeight;

//  footer
@property (nullable,nonatomic) UIColor *footerBgColor;
@property (nullable,nonatomic) UIColor *footerTextColor;
@property (nullable,nonatomic) UIFont  *footerFont;
@property (nonatomic) float    footerHeight;


//- (nonnull instancetype)initWithTableView:(nonnull UITableView*)tableView delegate:(nullable id)delegate registerClass:(nullable NSArray<Class>*)cellClasses;

//  table view cell height
- (float)getCellHeightWithModel:(nonnull id)model;
- (void)setCellHeight:(float)cellHeight model:(nonnull id)model;
- (void)setCellHeight:(float)cellHeight models:(nonnull NSArray*)models;

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



@interface KHCollectionDataBinding : KHDataBinding <UICollectionViewDataSource, UICollectionViewDelegate, UICollectionViewDelegateFlowLayout, UIScrollViewDelegate>
{

    //  用來判斷說是否已經初始完成，不然在初始前就做 insert 的動畫，會掛掉
    BOOL _hasInit;
    
    NSMutableArray *_headerModelList;
    
    NSMutableArray *_footerModelList;
    
    //  key: mapping model class name / value: reusableView class
    NSMutableDictionary *_reusableViewDic;
    
    //  key: reusableView class name / value: size Value
    NSMutableDictionary *_reusableViewSizeDic;
}

@property (nonnull,nonatomic) UICollectionView *collectionView;
@property (nullable,nonatomic) id delegate;
@property (nullable,nonatomic) UICollectionViewFlowLayout *layout;

//- (nonnull instancetype)initWithCollectionView:(nonnull UICollectionView*)collectionView delegate:(nullable id)delegate registerClass:(nullable NSArray<Class>*)cellClasses;

- (CGSize)getCellSizeWithModel:(nonnull id)model;
- (void)setCellSize:(CGSize)cellSize model:(nonnull id)model;
- (void)setCellSize:(CGSize)cellSize models:(nonnull NSArray*)models;


- (void)registerReusableView:(nonnull Class)reusableViewClass;
- (void)registerReusableView:(nonnull Class)reusableViewClass size:(CGSize)size;
- (void)setReusableView:(nonnull Class)reusableViewClass size:(CGSize)size;
- (CGSize)getReusableViewSize:(nonnull Class)reusableViewClass;

- (void)setHeaderModel:(nonnull id)headerModel atIndex:(NSInteger)sectionIndex;
- (void)setHeaderModels:(nonnull NSArray*)headerModels;

- (void)setFooterModel:(nonnull id)headerModel atIndex:(NSInteger)sectionIndex;
- (void)setFooterModels:(nonnull NSArray*)headerModels;

@end

